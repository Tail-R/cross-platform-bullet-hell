#include <iostream>
#include <cmath>        // std::sqrt
#include <algorithm>    // std::clamp
#include "game_server.hpp"
#include "game_logic_constants.hpp"
#include "../packet_stream/packet_stream.hpp"
#include "../packet_template/packet_template.hpp"

GameServerMaster::GameServerMaster(uint16_t server_port, size_t max_instances)
    : m_ready_to_accept(false)
    , m_running(false)
    , m_max_instances(max_instances)
    , m_active_instances(0)
{
    m_server_socket = std::make_shared<ServerSocket>(
        server_port
    );

    std::cout << "[GameServerMaster] DEBUG: Game server has been started" << "\n";
}

GameServerMaster::~GameServerMaster() {
    stop();

    std::cout << "[GameServerMaster] DEBUG: Game server shutdown complete" << "\n";
}

bool GameServerMaster::initialize() {
    return m_server_socket->initialize();
}

void GameServerMaster::run() {
    if (!m_running)
    {
        m_running = true;

        accept_loop();
    }
}

void GameServerMaster::run_async() {
    if (!m_running)
    {
        m_running = true;
        m_accept_thread = std::thread(&GameServerMaster::accept_loop, this);

        std::cout << "[GameServerMaster] DEBUG: Accept thread has been created" << "\n";
    }
}

void GameServerMaster::stop() {
    if (m_running)
    {
        m_running = false;

        m_server_socket->disconnect();

        // Accept thread
        if (m_accept_thread.joinable())
        { 
            m_accept_thread.join();

            std::cout << "[GameServerMaster] DEBUG: Accept thread has been joined" << "\n";
        }
    }
}

bool GameServerMaster::wait_for_accept_ready(size_t timeout_msec, size_t max_attempts) {
    size_t attempt = 0;

    while (true)
    {
        attempt++;

        if (m_ready_to_accept)
        {
            return true;
        }

        if (attempt > max_attempts)
        {
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(timeout_msec));
    }
}

void GameServerMaster::accept_loop() {
    m_ready_to_accept = true;

    while (m_running)
    {
        std::cerr << "[GameServerMaster] DEBUG: Now accept will block this thread" << "\n";

        // Block until the client to connect
        auto client_opt = m_server_socket->accept_client();

        std::cerr << "[GameServerMaster] DEBUG: Thread is back from accept" << "\n";

        if (!client_opt.has_value())
        {
            std::cerr << "[GameServerMaster] ERROR: Invalid connection attempt from the client" << "\n";

            continue;
        }

        auto client_conn = std::make_shared<ClientConnection>(
            std::move(client_opt.value())
        );

        std::cout << "[GameServerMaster] DEBUG: client_conn accepted" << "\n";
        
        auto current = m_active_instances.load();

        // CAS (Compare-And-Swap)
        if (m_active_instances >= m_max_instances || !m_active_instances.compare_exchange_strong(current, current + 1))
        {
            std::cerr << "[GameServerMaster] DEBUG: The maximum number of instances has been reached"
                      << " and the client connection has been refused." << "\n";

            client_conn->disconnect();

            continue;
        }

        // Create thread
        auto worker_thread = std::thread([this, client_conn]() {
            handle_client(client_conn);
            m_active_instances.fetch_sub(1);
        });
            
        worker_thread.detach();

        std::cout << "[GameServerMaster] DEBUG: Game Instance has been created" << "\n"
                  << "[GameServerMaster] DEBUG: " << m_active_instances << " instances are active" << "\n";
    }

    m_ready_to_accept = false;
}

void apply_player_input(
    PlayerSnapshot& player,
    const InputDirection& input,
    float speed = game_logic_constants::PLAYER_SPEED
) {
    constexpr float inv_sqrt2 = 0.70710678f;

    float dx = 0.0f;
    float dy = 0.0f;

    switch (input)
    {
        case InputDirection::Up:        { dy = +1.0f;                       break; }
        case InputDirection::Down:      { dy = -1.0f;                       break; }
        case InputDirection::Right:     { dx = +1.0f;                       break; }
        case InputDirection::Left:      { dx = -1.0f;                       break; }
        case InputDirection::UpRight:   { dx = +inv_sqrt2; dy = +inv_sqrt2; break; }
        case InputDirection::DownRight: { dx = +inv_sqrt2; dy = -inv_sqrt2; break; }
        case InputDirection::UpLeft:    { dx = -inv_sqrt2; dy = +inv_sqrt2; break; }
        case InputDirection::DownLeft:  { dx = -inv_sqrt2; dy = -inv_sqrt2; break; }

        case InputDirection::Stop:
        default: return;
    }

    player.pos = {
        player.pos.x = std::clamp(player.pos.x + dx * speed, -192.0f, 192.f),
        player.pos.y = std::clamp(player.pos.y + dy * speed, -224.0f, 224.0f)
    };
}

void GameServerMaster::handle_client(std::shared_ptr<ClientConnection> client_conn) {
    PacketStreamServer stream(client_conn);
    stream.start();

    FrameSnapshot frame = {};
    frame.player_count = 1;

    PlayerSnapshot player = {};
    frame.player_vector.push_back(player);

    ArrowState arrow_state = {};

    auto quit = false;

    // msec / FPS
    constexpr auto target_frame_duration = std::chrono::milliseconds(1000 / 60);

    // Game logic loop
    while (m_running && !quit)
    {
        auto frame_start = std::chrono::steady_clock::now();

        // Check if the recv thread is alive
        if (stream.has_exception())
        {
            break;
        }

        // Process the packet queue
        while (true)
        {
            std::optional<Packet> packet_opt = stream.poll_packet();

            if (!packet_opt.has_value())
            {
                break;
            }

            Packet packet = std::move(*packet_opt);

            switch (packet.header.payload_type)
            {
                case PayloadType::ClientHello:
                {
                    std::cout << "[GameServerMaster] DEBUG: Received ClientHello" << "\n";

                    break;
                }

                case PayloadType::ClientGameRequest:
                {
                    std::cout << "[GameServerMaster] DEBUG: Received ClientGameRequest" << "\n";

                    break;
                }

                case PayloadType::ClientInput:
                {
                    // std::cout << "[GameServerMaster] DEBUG: Received ClientInput" << "\n";

                    const auto input_snapshot = std::get<ClientInput>(packet.payload);
                    arrow_state.held |= input_snapshot.game_input.arrows.pressed;
                    arrow_state.held &= ~input_snapshot.game_input.arrows.released;

                    break;
                }

                case PayloadType::ClientGoodBye:
                {
                    std::cout << "[GameServerMaster] DEBUG: Received ClientGoodBye" << "\n";

                    quit = true;

                    break;
                }

                default:
                {
                    std::cerr << "[GameServerMaster] DEBUG: Unexpected message type: "
                            << static_cast<uint32_t>(packet.header.payload_type) << "\n";
                    break;
                }
            }
        }

        auto direction = get_direction_from_arrows(arrow_state);
        apply_player_input(frame.player_vector[0], direction);
                
        const auto packet = make_packet<FrameSnapshot>(frame);
        stream.send_packet(packet);

        // Adjust the frame rate
        auto frame_end = std::chrono::steady_clock::now();
        auto frame_duration = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_start);

        if (frame_duration < target_frame_duration)
        {
            std::this_thread::sleep_for(target_frame_duration - frame_duration);
        }
    }

    std::cout << "[GameServerMaster] DEBUG: Game Instance has been terminated successfully" << "\n";
}