#include <iostream>
#include "packet_stream.hpp"
#include "../packet_serializer/packet_serializer.hpp"

namespace {
    constexpr size_t TEMP_BUFFER_SIZE = 4096;
}

PacketStreamClient::PacketStreamClient(std::shared_ptr<ClientSocket> socket)
    : m_socket(std::move(socket))
    , m_running(false)
{}

PacketStreamClient::~PacketStreamClient() {
    stop();
}

/*
    Launch the worker thread that receives bytes from server
*/
void PacketStreamClient::start() {
    if (!m_running)
    {
        m_running = true;
        
        m_recv_thread = std::thread(&PacketStreamClient::receive_loop, this);
        std::cout << "[PacketStreamClient] DEBUG: Receive thread has been created" << "\n";
    }
}

/*
    Tell the worker threads to stop processing the byte stream
    and waits for them to return.
*/
void PacketStreamClient::stop() {
    if (m_running)
    {
        m_running = false;

        // Abort recv blocking
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

}

bool PacketStreamClient::send_packet(const Packet& packet) {
    std::vector<std::byte> payload_bytes;

    switch (packet.header.payload_type)
    {
        case PayloadType::Hello:
            payload_bytes = serialize_client_hello(std::get<ClientHello>(packet.payload));

            break;

        case PayloadType::Accept:
            payload_bytes = serialize_server_accept(std::get<ServerAccept>(packet.payload));

            break;

        case PayloadType::GoodBye:
            if (std::holds_alternative<ClientGoodBye>(packet.payload))
            {
                payload_bytes = serialize_client_goodbye(std::get<ClientGoodBye>(packet.payload));
            }
            else
            {
                payload_bytes = serialize_server_goodbye(std::get<ServerGoodBye>(packet.payload));
            }

            break;

        case PayloadType::GameRequest:
            payload_bytes = serialize_client_game_request(std::get<ClientGameRequest>(packet.payload));

            break;

        case PayloadType::GameResponse:
            payload_bytes = serialize_server_game_response(std::get<ServerGameResponse>(packet.payload));

            break;

        case PayloadType::ReconnectRequest:
            payload_bytes = serialize_client_reconnect_request(std::get<ClientReconnectRequest>(packet.payload));

            break;

        case PayloadType::ReconnectResponse:
            payload_bytes = serialize_server_reconnect_response(std::get<ServerReconnectResponse>(packet.payload));

            break;

        case PayloadType::Input:
            payload_bytes = serialize_client_input(std::get<ClientInput>(packet.payload));

            break;

        case PayloadType::FrameSnapshot:
        {
            auto frame_bytes_opt = serialize_frame(std::get<FrameSnapshot>(packet.payload));

            if (!frame_bytes_opt.has_value())
            {
                std::cerr << "[PacketStreamClient] Failed to serialize frame. The data can not be sent." << "\n";

                return false;
            }
            
            payload_bytes = frame_bytes_opt.value();
        }

            break;

        default:
            std::cerr << "[PacketStreamClient] Unknown PayloadType. The data can not be sent." << "\n";

            return false;
    }

    PacketHeader header = packet.header;
    header.payload_size = static_cast<uint32_t>(payload_bytes.size());
    header.magic_number = PACKET_MAGIC_NUMBER;

    std::vector<std::byte> buffer;
    auto header_bytes = serialize_packet_header(header);

    buffer.insert(buffer.end(), header_bytes.begin(), header_bytes.end());
    buffer.insert(buffer.end(), payload_bytes.begin(), payload_bytes.end());

    return m_socket->send_data(buffer);
}

void PacketStreamClient::receive_loop() {
    std::byte temp_buffer[TEMP_BUFFER_SIZE];

    while (m_running)
    {
        ssize_t bytes_read = m_socket->recv_data(temp_buffer, TEMP_BUFFER_SIZE);

        if (bytes_read <= 0)
        {
            continue;
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

    /*
        Only process if there is more data in the buffer
        than the size of the packet header
    */
    while (m_buffer.size() - offset >= PACKET_HEADER_SIZE)
    {
        PacketHeader header = {};

        // Read magic number
        memcpy(
            &header,
            m_buffer.data() + offset,
            PACKET_HEADER_SIZE
        );

        // Check magic number
        if (header.magic_number != PACKET_MAGIC_NUMBER)
        {
            // Increment the offset and retry
            offset++;

            continue;
        }

        // The packet receive is incomplete
        if (m_buffer.size() - offset < PACKET_HEADER_SIZE + header.payload_size)
        {
            break;
        }

        // Read the payload
        auto payload_start = m_buffer.begin() + offset + PACKET_HEADER_SIZE;
        auto payload_end = m_buffer.begin() + offset + PACKET_HEADER_SIZE + header.payload_size;

        std::vector<std::byte> payload(payload_start, payload_end);

        const auto payload_type = static_cast<PayloadType>(header.payload_type);

        // Deserialize the payload and push it onto the queue
        switch (payload_type)
        {
            // Frame snapshot
            case PayloadType::FrameSnapshot:
            {
                auto frame_opt = deserialize_frame(payload);

                if (frame_opt.has_value())
                {
                    std::lock_guard<std::mutex> lock(m_frame_mutex);

                    m_frame_queue.push(frame_opt.value());
                }
                else
                {
                    std::cerr << "[PacketStreamClient] Failed to deserialize payload" << "\n";
                }

                break;
            }

            default:
                std::cerr << "[PacketStreamClient] Unknown packet type: " << static_cast<uint32_t>(header.payload_type) << "\n";
        }

        offset += PACKET_HEADER_SIZE + header.payload_size;
    }

    // Erase
    if (offset > 0)
    {
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + offset);
    }
}