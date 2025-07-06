#include <iostream>
#include <cstring>
#include "packet_stream.hpp"
#include "../packet_serializer/packet_serializer.hpp"

namespace {
    constexpr size_t TEMP_BUFFER_SIZE = 4096;
}

/*
    Client
*/
PacketStreamClient::PacketStreamClient(std::shared_ptr<ClientSocket> socket)
    : m_socket(std::move(socket))
    , m_running(false)
    , m_send_sequence(0)
{}

PacketStreamClient::~PacketStreamClient() {
    stop();
}

void PacketStreamClient::start() {
    if (!m_running)
    {
        m_running = true;

        m_recv_thread = std::thread(&PacketStreamClient::receive_loop, this);
        std::cout << "[PacketStreamClient] DEBUG: Receive thread has been created" << "\n";
    }
}

void PacketStreamClient::stop() {
    if (m_running)
    {
        m_running = false;
        m_socket->abort();

        if (m_recv_thread.joinable())
        {
            m_recv_thread.join();
            std::cout << "[PacketStreamClient] DEBUG: Receive thread has been joined" << "\n";
        }
    }
}

std::optional<FrameSnapshot> PacketStreamClient::poll_frame() {
    std::lock_guard<std::mutex> lock(m_frame_mutex);

    if (m_frame_queue.empty())
    {
        return std::nullopt;
    }

    const auto frame = std::move(m_frame_queue.front());
    m_frame_queue.pop();

    return frame;
}

std::optional<PacketPayload> PacketStreamClient::poll_message() {
    std::lock_guard<std::mutex> lock(m_message_mutex);

    if (m_message_queue.empty())
    {
        return std::nullopt;
    }

    const auto message = std::move(m_message_queue.front());
    m_message_queue.pop();

    return message;
}

bool PacketStreamClient::send_packet(const Packet& packet) {
    const auto actual_type = get_payload_type(packet.payload);

    const auto expr1 = packet.header.payload_type != actual_type;
    const auto expr2 = packet.header.payload_type == PayloadType::Unknown;
    const auto expr3 = actual_type == PayloadType::Unknown;

    if (expr1 || expr2 || expr3)
    {
        std::cerr << "[PacketStreamClient] ERROR: Invalid payload, abort sending. "
                  << "header_type=" << static_cast<int>(packet.header.payload_type)
                  << ", actual_type=" << static_cast<int>(actual_type) << "\n";


        return false;
    }

    std::vector<std::byte> payload_bytes;

    switch (packet.header.payload_type)
    {
        case PayloadType::ClientHello:              { payload_bytes = serialize_client_hello(std::get<ClientHello>(packet.payload)); break; }
        case PayloadType::ClientGoodBye:            { payload_bytes = serialize_client_goodbye(std::get<ClientGoodBye>(packet.payload)); break; }
        case PayloadType::ClientGameRequest:        { payload_bytes = serialize_client_game_request(std::get<ClientGameRequest>(packet.payload)); break; }
        case PayloadType::ClientReconnectRequest:   { payload_bytes = serialize_client_reconnect_request(std::get<ClientReconnectRequest>(packet.payload)); break; }
        case PayloadType::ClientInput:              { payload_bytes = serialize_client_input(std::get<ClientInput>(packet.payload)); break; }
        default:
        {
            std::cerr << "[PacketStreamClient] Invalid PayloadType: "
                      << static_cast<uint32_t>(packet.header.payload_type) << "\n"
                      << "[PacketStreamClient] the packet can not be sent" << "\n";
            return false;
        }
    }

    PacketHeader header = packet.header;
    
    header.magic_number     = PACKET_MAGIC_NUMBER;
    header.sequence_number  = m_send_sequence.fetch_add(1);
    header.payload_size     = static_cast<uint32_t>(payload_bytes.size());
    header.payload_type     = get_payload_type(packet.payload);
    
    auto header_bytes = serialize_packet_header(header);

    std::vector<std::byte> buffer;

    buffer.insert(buffer.end(), header_bytes.begin(), header_bytes.end());
    buffer.insert(buffer.end(), payload_bytes.begin(), payload_bytes.end());

    return m_socket->send_data(buffer);
}

void PacketStreamClient::receive_loop() {
    std::byte temp_buffer[TEMP_BUFFER_SIZE];

    while (m_running)
    {
        ssize_t bytes_read = m_socket->recv_data(temp_buffer, TEMP_BUFFER_SIZE);

        if (bytes_read == SOCKET_RECV_TIMEOUT)
        {
            continue;
        }
        else if (bytes_read == 0)
        {
            std::cerr << "[PacketStreamClient] ERROR: Server disconnected (EOF)" << "\n";

            m_running = false;

            break;
        }
        else if (bytes_read < 0)
        {
            std::cerr << "[PacketStreamClient] ERROR: Recv failed: " << strerror(errno) << "\n";

            m_running = false;

            break;
        }
        
        m_buffer.insert(
            m_buffer.end(),
            temp_buffer,
            temp_buffer + bytes_read
        );

        process_buffer();
    }
}

void PacketStreamClient::process_buffer() {
    size_t offset = 0;

    while (m_buffer.size() - offset >= PACKET_HEADER_SIZE)
    {
        PacketHeader header = {};

        memcpy(&header, m_buffer.data() + offset, PACKET_HEADER_SIZE);

        if (header.magic_number != PACKET_MAGIC_NUMBER)
        {
            offset++;
            continue;
        }

        if (m_buffer.size() - offset < PACKET_HEADER_SIZE + header.payload_size)
        {
            break;
        }

        auto payload_start = m_buffer.begin() + offset + PACKET_HEADER_SIZE;
        auto payload_end = payload_start + header.payload_size;

        const auto payload_type = static_cast<PayloadType>(header.payload_type);
        std::vector<std::byte> payload(payload_start, payload_end);
        std::optional<PacketPayload> message;

        switch (payload_type)
        {
            case PayloadType::ServerAccept:             { if (auto opt = deserialize_server_accept(payload)) message = *opt; break; }
            case PayloadType::ServerGoodBye:            { if (auto opt = deserialize_server_goodbye(payload)) message = *opt; break; }
            case PayloadType::ServerGameResponse:       { if (auto opt = deserialize_server_game_response(payload)) message = *opt; break; }
            case PayloadType::ServerReconnectResponse:  { if (auto opt = deserialize_server_reconnect_response(payload)) message = *opt; break; }
            case PayloadType::FrameSnapshot:
            {
                if (auto opt = deserialize_frame(payload))
                {
                    std::lock_guard<std::mutex> lock(m_frame_mutex);
                    m_frame_queue.push(*opt);
                }
                break;
            }
            default:
            {
                std::cerr << "[PacketStreamClient] Invalid payload type: " 
                          << static_cast<uint32_t>(payload_type) << "\n"
                          << "[PacketStreamClient] Failed to process the buffer" << "\n";
                break;
            }
        }

        if (message.has_value())
        {
            std::lock_guard<std::mutex> lock(m_message_mutex);
            m_message_queue.push(*message);
        }

        offset += PACKET_HEADER_SIZE + header.payload_size;
    }

    if (offset > 0)
    {
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + offset);
    }
}

/*
    Server
*/
PacketStreamServer::PacketStreamServer(std::shared_ptr<ClientConnection> connection)
    : m_connection(std::move(connection))
    , m_running(false)
    , m_send_sequence(0)
    , m_thread_exception(nullptr)
{}

PacketStreamServer::~PacketStreamServer() {
    stop();
}

void PacketStreamServer::start() {
    if (!m_running)
    {
        m_running = true;
        m_thread_exception = nullptr;

        m_recv_thread = std::thread([this]() {
            try
            {
                receive_loop();
            }
            catch (const std::exception& e)
            {
                m_thread_exception = std::current_exception();
                
                std::cerr << "[PacketStreamServer] ERROR: Receive thread threw an exception: " << e.what() << "\n";
            }
        });

        std::cout << "[PacketStreamServer] DEBUG: Receive thread started" << "\n";
    }
}

void PacketStreamServer::stop() {
    if (m_running)
    {
        m_running = false;
        m_connection->abort();

        if (m_recv_thread.joinable())
        {
            m_recv_thread.join();
            
            std::cout << "[PacketStreamServer] DEBUG: Receive thread has been joined" << "\n";

            if (has_exception())
            {
                std::cerr << "[PacketStreamServer] ERROR: Detected stream exception, terminating client instance" << "\n";
            }
        }
    }
}

bool PacketStreamServer::send_packet(const Packet& packet) {
    const auto actual_type = get_payload_type(packet.payload);

    const auto expr1 = packet.header.payload_type != actual_type;
    const auto expr2 = packet.header.payload_type == PayloadType::Unknown;
    const auto expr3 = actual_type == PayloadType::Unknown;

    if (expr1 || expr2 || expr3)
    {
        std::cerr << "[PacketStreamServer] ERROR: Invalid payload, abort sending. "
                  << "header_type=" << static_cast<int>(packet.header.payload_type)
                  << ", actual_type=" << static_cast<int>(actual_type) << "\n";

        return false;
    }

    std::vector<std::byte> payload_bytes;

    switch (packet.header.payload_type)
    {
        case PayloadType::ServerAccept:             { payload_bytes = serialize_server_accept(std::get<ServerAccept>(packet.payload)); break; }
        case PayloadType::ServerGoodBye:            { payload_bytes = serialize_server_goodbye(std::get<ServerGoodBye>(packet.payload)); break; }
        case PayloadType::ServerGameResponse:       { payload_bytes = serialize_server_game_response(std::get<ServerGameResponse>(packet.payload)); break; }
        case PayloadType::ServerReconnectResponse:  { payload_bytes = serialize_server_reconnect_response(std::get<ServerReconnectResponse>(packet.payload)); break; }
        case PayloadType::FrameSnapshot:
        {
            auto frame_bytes_opt = serialize_frame(std::get<FrameSnapshot>(packet.payload));

            if (!frame_bytes_opt.has_value())
            {
                std::cerr << "[PacketStreamServer] ERROR: Failed to serialize frame" << "\n"
                          << "[PacketStreamServer] ERROR: The data can not be sent" << "\n";

                return false;
            }
            payload_bytes = frame_bytes_opt.value();
            break;
        }
        default:
        {
            std::cerr << "[PacketStreamServer] ERROR: Invalid PayloadType" << "\n"
                      << "[PacketStreamServer] ERROR: The data can not be sent" << "\n";

            return false;
        }
    }

    PacketHeader header = packet.header;

    header.magic_number     = PACKET_MAGIC_NUMBER;
    header.sequence_number  = m_send_sequence.fetch_add(1);
    header.payload_size     = static_cast<uint32_t>(payload_bytes.size());
    header.payload_type     = get_payload_type(packet.payload);

    auto header_bytes = serialize_packet_header(header);

    std::vector<std::byte> buffer;

    buffer.insert(buffer.end(), header_bytes.begin(), header_bytes.end());
    buffer.insert(buffer.end(), payload_bytes.begin(), payload_bytes.end());

    return m_connection->send_data(buffer) > 0;
}

std::optional<Packet> PacketStreamServer::poll_packet() {
    std::lock_guard<std::mutex> lock(m_packet_mutex);

    if (m_packet_queue.empty())
    {
        return std::nullopt;
    }

    Packet packet = std::move(m_packet_queue.front());
    m_packet_queue.pop();

    return packet;
}

bool PacketStreamServer::has_exception() const {
    return m_thread_exception != nullptr;
}

void PacketStreamServer::receive_loop() {
    std::byte temp_buffer[TEMP_BUFFER_SIZE];

    while (m_running)
    {
        ssize_t bytes_read = m_connection->recv_data(temp_buffer, TEMP_BUFFER_SIZE);

        if (bytes_read == SOCKET_RECV_TIMEOUT)
        {
            continue;
        }
        else if (bytes_read == 0)
        {
            throw std::runtime_error("[PacketStreamServer] client disconnected");
        }
        else if (bytes_read < 0)
        {
            if (errno == ECONNRESET || errno == EPIPE)
            {
                throw std::runtime_error("[PacketStreamServer] client connection reset");
            }
        }

        m_buffer.insert(m_buffer.end(), temp_buffer, temp_buffer + bytes_read);

        process_buffer();
    }
}

void PacketStreamServer::process_buffer() {
    size_t offset = 0;

    while (m_buffer.size() - offset >= PACKET_HEADER_SIZE)
    {
        PacketHeader header;
        memcpy(&header, m_buffer.data() + offset, PACKET_HEADER_SIZE);

        if (header.magic_number != PACKET_MAGIC_NUMBER)
        {
            offset++;
            continue;
        }

        if (m_buffer.size() - offset < PACKET_HEADER_SIZE + header.payload_size)
        {
            break;
        }

        auto payload_start = m_buffer.begin() + offset + PACKET_HEADER_SIZE;
        auto payload_end   = payload_start + header.payload_size;

        const auto payload_type = static_cast<PayloadType>(header.payload_type);
        std::vector<std::byte> payload(payload_start, payload_end);
        std::optional<PacketPayload> message;

        switch (payload_type)
        {
            case PayloadType::ClientHello:              { if (auto opt = deserialize_client_hello(payload)) message = *opt; break; }
            case PayloadType::ClientGoodBye:            { if (auto opt = deserialize_client_goodbye(payload)) message = *opt; break; }
            case PayloadType::ClientGameRequest:        { if (auto opt = deserialize_client_game_request(payload)) message = *opt; break; }
            case PayloadType::ClientReconnectRequest:   { if (auto opt = deserialize_client_reconnect_request(payload)) message = *opt; break; }
            case PayloadType::ClientInput:              { if (auto opt = deserialize_client_input(payload)) message = *opt; break; }
            default:
            {
                std::cerr << "[PacketStreamServer] ERROR: Invalid payload type: " 
                          << static_cast<uint32_t>(payload_type) << "\n"
                          << "[PacketStreamServer] ERROR: Failed to process the buffer" << "\n";
                break;
            }
        }

        if (message.has_value())
        {
            Packet pkt;
            pkt.header = header;
            pkt.payload = *message;

            std::lock_guard<std::mutex> lock(m_packet_mutex);
            m_packet_queue.push(std::move(pkt));
        }

        offset += PACKET_HEADER_SIZE + header.payload_size;
    }

    if (offset > 0)
    {
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + offset);
    }
}
