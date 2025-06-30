#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <atomic>
#include <optional>
#include <memory>

#include "../socket/socket.hpp"
#include "../packet_template/packet_template.hpp"

class PacketStreamClient {
public:
    explicit PacketStreamClient(std::shared_ptr<ClientSocket> socket);
    ~PacketStreamClient();

    void start();
    void stop();

    std::optional<FrameSnapshot> poll_frame();
    std::optional<PacketPayload> poll_message();

    bool send_packet(const Packet& packet);

private:
    void receive_loop();
    void process_buffer();

    std::shared_ptr<ClientSocket>   m_socket;
    std::atomic<bool>               m_running;
    std::thread                     m_recv_thread;
    
    std::vector<std::byte>          m_buffer;

    std::mutex                      m_frame_mutex;
    std::queue<FrameSnapshot>       m_frame_queue;

    std::mutex                      m_message_mutex;
    std::queue<PacketPayload>       m_message_queue;      
};

// class PacketStreamServer {
// public:
//     explicit PacketStreamServer(std::shared_ptr<ClientConnection> client_connection);

// private:

// };