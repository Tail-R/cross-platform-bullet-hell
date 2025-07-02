#pragma once

#include <cstdint>
#include <thread>
#include <atomic>
#include "../socket/socket.hpp"

class GameServer {
public:
    GameServer(uint16_t server_port);
    ~GameServer();

    bool initialize();
    void run();
    void stop();

private:
    void accept_loop();
    void handle_client();

    // ServerSocket        m_server_socket;
    std::thread         m_main_thread;
    std::atomic<bool>   m_running;
};