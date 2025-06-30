#include <iostream>
#include "packet_stream.hpp"
#include "../packet_template/header.hpp"
#include "../packet_serializer/frame_serializer.hpp"

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