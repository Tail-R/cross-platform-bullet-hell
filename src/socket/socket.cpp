#include <array>
#include <limits>
#include "socket.hpp"

namespace {
    constexpr size_t TEMP_BUFFER_SIZE = 4096;

    ssize_t socket_send(SOCKET sock, const std::vector<std::byte>& bytes) {
        // Check for overflow
#ifdef _WIN32
        if (bytes.size() > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            return SOCKET_ERROR;
        }

        int safe_size = static_cast<int>(bytes.size());
#else
        size_t safe_size = bytes.size();
#endif
        return send(
            sock,
            /*
                Convert std::byte* into const char*
            */
            reinterpret_cast<const char*>(bytes.data()),
            safe_size,
            0
        );
    }

    ssize_t socket_recv(SOCKET sock, std::byte* buffer, size_t size) {
        // Check for overflow
#ifdef _WIN32
        if (size > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            return SOCKET_ERROR;
        }

        int safe_size = static_cast<int>(size);
#else
        size_t safe_size = size;
#endif
        return recv(
            sock,
            /*
                Convert std::byte* into const char*
            */
            reinterpret_cast<char*>(buffer),
            safe_size,
            0
        );
    }

    std::optional<std::vector<std::byte>> socket_recv_exact(SOCKET sock, size_t size) {
        auto buffer = std::vector<std::byte>();
        buffer.reserve(size);

        while (buffer.size() < size)
        {
            // Temporary buffer
            std::array<std::byte, TEMP_BUFFER_SIZE> temp_buffer;

            // Calclate the remaining size
            size_t remaining = size - buffer.size();
            size_t to_read = std::min(temp_buffer.size(), remaining);

            ssize_t received = socket_recv(sock, temp_buffer.data(), to_read);

            if (received == 0)
            // Graceful shutdown
            {
                return std::nullopt;
            }
            else if (received == SOCKET_ERROR)
            {
                return std::nullopt;
            }

            buffer.insert(
                buffer.end(),
                temp_buffer.begin(),
                temp_buffer.begin() + received
            );
        }

        return buffer;
    }

    void close_socket(SOCKET sock) {
        if (sock == INVALID_SOCKET)
        {
            return;
        }

#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
    }
}

#ifdef _WIN32
void WinsockManager::initialize() {
    static WinsockManager instance; // Initialized once (Singleton pattern)
    (void)instance; // Supress warnings
}

WinsockManager::WinsockManager() {
    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        throw std::runtime_error("WSAStartup failed");
    }
}

WinsockManager::~WinsockManager() {
    WSACleanup();
}
#endif

ClientSocket::ClientSocket(std::string_view server_addr, uint16_t server_port)
    : m_server_addr(server_addr)
    , m_server_port(server_port)
    , m_server_sock(INVALID_SOCKET)
    , m_server_connected(false)
{}

ClientSocket::~ClientSocket() {
    disconnect();
}

bool ClientSocket::connect_to_server() {
    if (m_server_connected)
    {
        disconnect();

        return false;
    }

#ifdef _WIN32
    WinsockManager::initialize();
#endif

    // Create socket
    m_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_server_sock == INVALID_SOCKET)
    {
        return false;
    }

    // Create address
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_server_port);

    auto pton_result = inet_pton(
        AF_INET,
        m_server_addr.data(),
        &server_addr.sin_addr
    );

    if (pton_result <= 0)
    {
        close_socket(m_server_sock);
        
        return false;
    }

    // Try to connect to server
    auto conn_result = connect(
        m_server_sock,
        reinterpret_cast<sockaddr*>(&server_addr),
        sizeof(server_addr)
    );

    if (conn_result == SOCKET_ERROR)
    {
        close_socket(m_server_sock);

        return false;
    }

    m_server_connected = true;

    return true;
}

void ClientSocket::abort() {
    /*
        At this point recv returns 0 on both Windows and Linux (POSIX)
        The 2 means to stop both reading and writing
    */
    shutdown(m_server_sock, 2);
}

void ClientSocket::disconnect() {
    if (m_server_sock != INVALID_SOCKET)
    {
        close_socket(m_server_sock);
        m_server_sock = INVALID_SOCKET;
        m_server_connected = false;
    }
}

ssize_t ClientSocket::send_data(const std::vector<std::byte>& data) {
    if (!m_server_connected)
    {
        return SOCKET_ERROR;
    }

    return socket_send(m_server_sock, data);
}

ssize_t ClientSocket::recv_data(std::byte* buffer, size_t size) {
    if (!m_server_connected)
    {
        return SOCKET_ERROR;
    }
    
    return socket_recv(m_server_sock, buffer, size);
}

std::optional<std::vector<std::byte>> ClientSocket::recv_exact(size_t size) {
    if (!m_server_connected)
    {
        return std::nullopt;
    }

    return socket_recv_exact(m_server_sock, size);
}

ClientConnection::ClientConnection(SOCKET client_sock)
    : m_client_sock(client_sock)
    , m_client_connected(false)
{
    if (m_client_sock != INVALID_SOCKET)
    {
        m_client_connected = true;
    }
}

ClientConnection::~ClientConnection() {
    disconnect();
}

ClientConnection::ClientConnection(ClientConnection&& other) noexcept
    : m_client_sock(other.m_client_sock)
    , m_client_connected(other.m_client_connected)
{
    other.m_client_sock = INVALID_SOCKET;
    other.m_client_connected = false;
}

ClientConnection& ClientConnection::operator=(ClientConnection&& other) noexcept
{
    // Self-assignment check
    if (this == &other)
    {
        return *this;
    }

    m_client_sock = other.m_client_sock;
    m_client_connected = other.m_client_connected;

    other.m_client_sock = INVALID_SOCKET;
    other.m_client_connected = false;

    return *this;
}

void ClientConnection::disconnect() {
    if (m_client_sock != INVALID_SOCKET)
    {
        close_socket(m_client_sock);
        m_client_sock = INVALID_SOCKET;
        m_client_connected = false;
    }
}

ssize_t ClientConnection::send_data(const std::vector<std::byte>& data) {
    if (!m_client_connected)
    {
        return SOCKET_ERROR;
    }

    return socket_send(m_client_sock, data);
}

ssize_t ClientConnection::recv_data(std::byte* buffer, size_t size) {
    if (!m_client_connected)
    {
        return SOCKET_ERROR;
    }

    return socket_recv(m_client_sock, buffer, size);
}

std::optional<std::vector<std::byte>> ClientConnection::recv_exact(size_t size) {
    if (!m_client_connected)
    {
        return std::nullopt;
    }

    return socket_recv_exact(m_client_sock, size);
}

ServerSocket::ServerSocket(uint16_t server_port)
    : m_server_port(server_port)
    , m_listen_sock(INVALID_SOCKET)
    , m_initialized(false)
{}

ServerSocket::~ServerSocket() {
    disconnect();
}

bool ServerSocket::initialize() {
#ifdef _WIN32
    WinsockManager::initialize();
#endif

    // Create listen socket
    m_listen_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (m_listen_sock == INVALID_SOCKET)
    {
        return false;
    }

    // Create address
    sockaddr_in server_hint = {};
    server_hint.sin_family = AF_INET;
    server_hint.sin_port = htons(m_server_port);
    server_hint.sin_addr.s_addr = INADDR_ANY;

    // Binding address to the listen socket
    auto bind_result = bind(
        m_listen_sock,
        reinterpret_cast<sockaddr*>(&server_hint),
        sizeof(server_hint)
    );

    if (bind_result == SOCKET_ERROR)
    {
        close_socket(m_listen_sock);
        return false;
    }

    // Start listening
    auto listen_result = listen(
        m_listen_sock,
        SOMAXCONN
    );

    if (listen_result == SOCKET_ERROR)
    {
        close_socket(m_listen_sock);
        return false;
    }

    m_initialized = true;

    return true;
}

void ServerSocket::disconnect() {
    if (m_listen_sock != INVALID_SOCKET)
    {
        close_socket(m_listen_sock);
        m_listen_sock = INVALID_SOCKET;
        m_initialized = false;
    }
}

std::optional<ClientConnection> ServerSocket::accept_client() {
    if (!m_initialized)
    {
        return std::nullopt;
    }
    
    sockaddr_in client = {};

#ifdef _WIN32
    int client_size = sizeof(client);
#else
    socklen_t client_size = sizeof(client);
#endif

    // Accept client
    SOCKET client_socket = accept(
        m_listen_sock,
        reinterpret_cast<sockaddr*>(&client),
        &client_size
    );

    if (client_socket == INVALID_SOCKET)
    {
        return std::nullopt;
    }

    return ClientConnection(client_socket);
}