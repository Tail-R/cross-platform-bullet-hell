#include <iostream>
#include <cmath>        // std::sqrt
#include <algorithm>    // std::clamp
#include "game_server.hpp"
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
}

GameServerMaster::~GameServerMaster() {
    stop();
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

        std::cout << "[GameServer] DEBUG: Accept thread has been created" << "\n";
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

            std::cout << "[GameServer] DEBUG: Accept thread has been joined" << "\n";
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
            std::cerr << "[GameServer] ERROR: Invalid connection attempt from the client" << "\n";

            continue;
        }

        auto client_conn = std::make_shared<ClientConnection>(
            std::move(client_opt.value())
        );

        std::cout << "[GameServer] DEBUG: client_conn accepted" << "\n";
        
        auto current = m_active_instances.load();

        // CAS (Compare-And-Swap)
        if (m_active_instances >= m_max_instances || !m_active_instances.compare_exchange_strong(current, current + 1))
        {
            std::cerr << "[GameServer] DEBUG: The maximum number of instances has been reached"
                      << " and the client connection has been refused." << "\n";

            client_conn->disconnect();

            continue;
        }

        // Create thread
        try
        {
            auto worker_thread = std::thread(
                &GameServerMaster::handle_client,
                this,
                client_conn
            );
            
            worker_thread.detach();
        }
        catch (const std::exception& e)
        {
            m_active_instances.fetch_sub(1);
            std::cerr << "[GameServer] ERROR: Exception during thread creation: " << e.what() << "\n";
        }

        std::cout << "[GameServer] DEBUG: Game Instance has been created" << "\n"
                  << "[GameServer] DEBUG: " << m_active_instances << " instances are active" << "\n";
    }

    m_ready_to_accept = false;
}

InputDirection get_direction_from_arrows(const ArrowState& arrows) {
    const auto up       = arrows.held.test(static_cast<size_t>(Arrow::Up));
    const auto right    = arrows.held.test(static_cast<size_t>(Arrow::Right));
    const auto down     = arrows.held.test(static_cast<size_t>(Arrow::Down));
    const auto left     = arrows.held.test(static_cast<size_t>(Arrow::Left));
 
    if (up && right)    { return InputDirection::UpRight;   }
    if (down && right)  { return InputDirection::DownRight; }
    if (down && left)   { return InputDirection::DownLeft;  }
    if (up && left)     { return InputDirection::UpLeft;    }
    if (up)             { return InputDirection::Up;        }
    if (right)          { return InputDirection::Right;     }
    if (down)           { return InputDirection::Down;      }
    if (left)           { return InputDirection::Left;      }

    return InputDirection::Stop;
}

void apply_player_input(PlayerSnapshot& player, const InputDirection& input, float speed = 0.02) {
    float dx = 0.0f;
    float dy = 0.0f;

    switch (input)
    {
        case InputDirection::Up:        { dy = +1;            break; }
        case InputDirection::Down:      { dy = -1;            break; }
        case InputDirection::Right:     { dx = +1;            break; }
        case InputDirection::Left:      { dx = -1;            break; }
        case InputDirection::UpRight:   { dx = +1; dy = +1;   break; }
        case InputDirection::DownRight: { dx = +1; dy = -1;   break; }
        case InputDirection::UpLeft:    { dx = -1; dy = +1;   break; }
        case InputDirection::DownLeft:  { dx = -1; dy = -1;   break; }

        default: return;
    }

    if (dx != 0 && dy != 0)
    {
        const float norm = std::sqrt(2.0f);

        dx /= norm;
        dy /= norm;
    }

    player.pos.x += dx * speed;
    player.pos.y += dy * speed;

    player.pos.x = std::clamp(player.pos.x, -1.0f, 1.0f);
    player.pos.y = std::clamp(player.pos.y, -1.0f, 1.0f);

    player.vel.x = dx * speed;
    player.vel.y = dy * speed;
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

    // Game logic loop
    while (m_running && !quit)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));

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
                    std::cout << "[GameServer] DEBUG: Received ClientHello" << "\n";

                    break;
                }

                case PayloadType::ClientGameRequest:
                {
                    std::cout << "[GameServer] DEBUG: Received ClientGameRequest" << "\n";

                    break;
                }

                case PayloadType::ClientInput:
                {
                    std::cout << "[GameServer] DEBUG: Received ClientInput" << "\n";

                    const auto input_snapshot = std::get<ClientInput>(packet.payload);
                    arrow_state.held |= input_snapshot.game_input.arrows.pressed;
                    arrow_state.held &= ~input_snapshot.game_input.arrows.released;

                    break;
                }

                case PayloadType::ClientGoodBye:
                {
                    std::cout << "[GameServer] DEBUG: Received ClientGoodBye" << "\n";

                    quit = true;

                    break;
                }

                default:
                {
                    std::cerr << "[GameServer] DEBUG: Unexpected message type: "
                            << static_cast<uint32_t>(packet.header.payload_type) << "\n";
                    break;
                }
            }
        }

        auto direction = get_direction_from_arrows(arrow_state);

        apply_player_input(frame.player_vector[0], direction);
                
        const auto packet = make_packet<FrameSnapshot>(frame);

        stream.send_packet(packet);
    }

    m_active_instances.fetch_sub(1);

    std::cout << "[GameServer] DEBUG: Game Instance has been terminated successfully" << "\n";
}