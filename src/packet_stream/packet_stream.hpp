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

    // Delete copy constructor and copy assignment operator
    PacketStreamClient(const PacketStreamClient&) = delete;
    PacketStreamClient& operator=(const PacketStreamClient&) = delete;

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

    std::atomic<uint32_t>           m_send_sequence;
};

class PacketStreamServer {
public:
    explicit PacketStreamServer(std::shared_ptr<ClientConnection> connection);
    ~PacketStreamServer();

    // Delete copy constructor and copy assignment operator
    PacketStreamServer(const PacketStreamServer&) = delete;
    PacketStreamServer& operator=(const PacketStreamServer&) = delete;

    void start();
    void stop();

    std::optional<Packet> poll_packet();
    bool send_packet(const Packet& packet);

    bool has_exception() const;

private:
    void receive_loop();
    void process_buffer();

    std::shared_ptr<ClientConnection>   m_connection;
    std::atomic<bool>                   m_running;
    std::thread                         m_recv_thread;

    std::vector<std::byte>              m_buffer;
    std::mutex                          m_packet_mutex;
    std::queue<Packet>                  m_packet_queue;

    std::atomic<uint32_t>               m_send_sequence;

    std::exception_ptr                  m_thread_exception;
};
