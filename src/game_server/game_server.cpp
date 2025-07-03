#include <iostream>
#include <cmath>        // std::sqrt
#include <algorithm>    // std::clamp
#include "game_server.hpp"
#include "../packet_stream/packet_stream.hpp"
#include "../packet_template/packet_template.hpp"

/*
    GameServerMaster
*/
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
        m_accept_thread = std::thread(GameServerMaster::accept_loop, this);

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
    while (m_running)
    {
        std::cerr << "[GameServerMaster] DEBUG: Now accept will block this thread" << "\n";
        /*
            Block until the client to connect
        */
        m_ready_to_accept = true;
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

        std::cout << "[GameServer] DEBUG: client_conn accepted\n";

        if (m_active_instances >= m_max_instances)
        {
            std::cerr << "[GameServer] DEBUG: The maximum number of instances has been reached"
                      << " and the client connection has been refused." << "\n";

            client_conn->disconnect();

            continue;
        }

        m_active_instances.fetch_add(1);

        // Create thread
        auto worker_thread = std::thread(
            GameServerMaster::handle_client,
            this,
            client_conn
        );

        worker_thread.detach();

        std::cout << "[GameServer] DEBUG: Game Instance has been created" << "\n"
                  << "[GameServer] DEBUG: " << m_active_instances << " instances are active" << "\n";
    }

    m_ready_to_accept = false;
}

void apply_player_input(PlayerSnapshot& player, InputDirection input, float speed = 0.02) {
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

    while (m_running)
    {
        std::optional<Packet> packet_opt = stream.poll_packet();

        if (!packet_opt.has_value())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        Packet packet = std::move(*packet_opt);

        switch (packet.header.payload_type)
        {
            case PayloadType::ClientHello:
            {
                std::cout << "[GameServer] DEBUG: Received ClientHello\n";

                break;
            }

            case PayloadType::ClientGameRequest:
            {
                std::cout << "[GameServer] DEBUG: Received ClientGameRequest\n";

                break;
            }

            case PayloadType::ClientInput:
            {
                const auto input_snapshot    = std::get<ClientInput>(packet.payload);
                const auto input             = input_snapshot.direction;

                apply_player_input(frame.player_vector[0], input);
                // std::cout << "x: " << frame.player_vector[0].pos.x << " y: " << frame.player_vector[0].pos.y << "\n";
                
                const auto packet = make_packet<FrameSnapshot>(frame);

                stream.send_packet(packet);

                break;
            }

            case PayloadType::ClientGoodBye:
            {
                std::cout << "[GameServer] DEBUG: Received ClientGoodBye\n";

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

    stream.stop();
    client_conn->disconnect();

    m_active_instances.fetch_sub(1);

    std::cout << "[GameServer] DEBUG: Game Instance has been terminated successfully" << "\n";
}