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
    bool is_running() const;

    // Returns the latest frame
    std::optional<FrameSnapshot> poll_frame();
    std::optional<Packet> poll_packet();

    bool send_packet(const Packet& packet);

    // Returns std::exception_ptr if there is an exception in the receive thread
    std::exception_ptr get_recv_exception() const;

private:
    void receive_loop();
    void process_buffer();

    std::shared_ptr<ClientSocket>   m_socket;
    std::atomic<bool>               m_running;
    std::thread                     m_recv_thread;
    
    std::vector<std::byte>          m_buffer;

    /*
        Packet Queue (Frame Snapshot Only)
        Frame shot packets are different from other messages in that
        they prioritize drawing the latest frame over guaranteeing arrival,
        so they have a dedicated queue. (For example, if there are multiple frames in the queue,
        the drawing thread will only get the latest frame in the queue and discard the rest.)
    */
    std::mutex                      m_frame_mutex;
    std::deque<FrameSnapshot>       m_frame_queue;

    // Packet queue (General)
    std::mutex                      m_packet_mutex;
    std::queue<Packet>              m_packet_queue;

    std::atomic<uint32_t>           m_send_sequence;

    std::exception_ptr              m_recv_thread_exception;
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
    bool is_running() const;

    std::optional<Packet> poll_packet();
    bool send_packet(const Packet& packet);

    // Returns std::exception_ptr if there is an exception in the receive thread
    std::exception_ptr get_recv_exception() const;

private:
    void receive_loop();
    void process_buffer();

    std::shared_ptr<ClientConnection>   m_connection;
    std::atomic<bool>                   m_running;
    std::thread                         m_recv_thread;

    std::vector<std::byte>              m_buffer;

    // Packet queue
    std::mutex                          m_packet_mutex;
    std::queue<Packet>                  m_packet_queue;

    std::atomic<uint32_t>               m_send_sequence;

    std::exception_ptr                  m_recv_thread_exception;
};
