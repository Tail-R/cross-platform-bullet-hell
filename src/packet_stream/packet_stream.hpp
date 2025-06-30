#pragma once

#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <atomic>
#include <optional>
#include <memory>

#include "../socket/socket.hpp"
#include "../packet_template/frame.hpp"

class PacketStreamClient {
public:
    explicit PacketStreamClient(std::shared_ptr<ClientSocket> socket);
    ~PacketStreamClient();

    void start();
    void stop();

    std::optional<FrameSnapshot> poll_frame();
    // std::optional<ServerMessage> poll_message();
    // bool send_message(ClientMessage);

private:
    void receive_loop();
    void process_buffer();

    std::shared_ptr<ClientSocket>   m_socket;
    std::atomic<bool>               m_running;
    std::thread                     m_recv_thread;
    
    std::vector<std::byte>          m_buffer;

    std::mutex                      m_frame_mutex;
    std::queue<FrameSnapshot>       m_frame_queue;

    // std::mutex                      m_message_mutex;
    // std::queue<ServerMessage>       m_message_queue;      
};