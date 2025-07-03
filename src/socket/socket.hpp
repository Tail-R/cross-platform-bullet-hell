#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <optional>
#include <atomic>

/*
    To-Do: Support send_data recv_data from multiple threads
*/

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>

    extern "C" int WSAAPI inet_pton(int af, const char* src, void* dst);

    /*
        Initializer for the Winsock API. This class is used as a
        static instance to ensure that the Winsock API is
        initialized and cleaned up exactly once.
    */  
    class WinsockManager {
    public:
        static void initialize();
    private:
        WinsockManager();
        ~WinsockManager();
    };
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>

    constexpr int INVALID_SOCKET  = -1;
    constexpr int SOCKET_ERROR    = -1;

    using SOCKET = int;
#endif

constexpr int SOCKET_RECV_TIMEOUT = -2;
constexpr int SOCKET_SEND_TIMEOUT = -2;

class ClientSocket {
public:
    ClientSocket(std::string_view server_addr, uint16_t server_port);
    ~ClientSocket();

    // Disable the copy constructor and copy assignment operator
    ClientSocket(const ClientSocket&) = delete;
    ClientSocket& operator=(const ClientSocket&) = delete;

    bool connect_to_server();
    void abort();
    void disconnect();

    ssize_t send_data(const std::vector<std::byte>& data);
    ssize_t recv_data(std::byte* buffer, size_t size);
    std::optional<std::vector<std::byte>> recv_exact(size_t size);

private:
    std::string_view    m_server_addr;
    uint16_t            m_server_port;
    SOCKET              m_server_sock;
    std::atomic<bool>   m_server_connected;
};

// A class to communicate with the ClientSocket
class ClientConnection {
public:
    ClientConnection(SOCKET client_sock);
    ~ClientConnection();

    // Delete copy constructor and copy assignment operator
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection& operator=(const ClientConnection&) = delete;

    // Define move constructor and move assignment operator
    ClientConnection(ClientConnection&& other) noexcept;
    ClientConnection& operator=(ClientConnection&& other) noexcept;

    void abort();
    void disconnect();

    ssize_t send_data(const std::vector<std::byte>& data);
    ssize_t recv_data(std::byte* buffer, size_t size);
    std::optional<std::vector<std::byte>> recv_exact(size_t size);
    
private:
    SOCKET              m_client_sock;
    std::atomic<bool>   m_client_connected;
};

class ServerSocket {
public:
    ServerSocket(uint16_t server_port);
    ~ServerSocket();

    // Delete the copy constructor and copy assignment operator
    ServerSocket(const ServerSocket&) = delete;
    ServerSocket& operator=(const ServerSocket&) = delete;

    bool initialize();
    void abort();
    void disconnect();

    std::optional<ClientConnection> accept_client();

private:
    uint16_t            m_server_port;
    SOCKET              m_listen_sock;
    std::atomic<bool>   m_initialized;
};