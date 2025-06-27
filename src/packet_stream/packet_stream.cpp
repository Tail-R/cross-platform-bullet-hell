#include <iostream>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <array>
#include "packet_stream.hpp"

namespace {
    constexpr size_t TEMP_BUFFER_SIZE = 4096;
}

PacketStreamClient::PacketStreamClient(std::string_view server_addr, uint16_t server_port, uint32_t magic_number, uint32_t max_packet_size)
    : m_client_socket(server_addr, server_port)
    , m_server_connected(false)
    , m_magic_number(magic_number)
    , m_max_packet_size(max_packet_size)
{}

PacketStreamClient::~PacketStreamClient() {
    disconnect();
}

bool PacketStreamClient::connect_to_server() {
    m_server_connected = m_client_socket.connect_to_server();

    return m_server_connected;
}

void PacketStreamClient::disconnect() {
    if (m_server_connected)
    {
        m_client_socket.disconnect();
        m_server_connected = false;
    }
}

std::optional<FrameSnapshot> PacketStreamClient::retrieve_frame(size_t max_attempts) {
    for (size_t attempt = 0; attempt < max_attempts; attempt++)
    {
        // Insert packet into buffer
        if (!refill_buffer())
        {
            continue;
        }

        while (true)
        {
            if(m_buffer.size() < sizeof(GamePacketHeader))
            {
                break;
            }

            auto packet_header_opt = try_extract_packet_header();

            if (!packet_header_opt)
            {
                continue;
            }

            auto packet_header = packet_header_opt.value();

            if (!is_valid_packet_size(packet_header))
            {
                std::cerr << "[PacketStreamClient] Invalid packet size: " << packet_header.body_size << " bytes" << "\n";
            
                return std::nullopt;
            }

            auto frame_opt = try_extract_frame(packet_header);
            
            return frame_opt;
        }
    }

    return std::nullopt;
}

std::vector<FrameSnapshot> PacketStreamClient::retrieve_all_frames(size_t max_attempts) {
    std::vector<FrameSnapshot> frames;

    for (size_t attempt = 0; attempt < max_attempts; attempt++) {
        if (!refill_buffer())
        {
            break;
        }

        while (true)
        {
            if (m_buffer.size() < sizeof(GamePacketHeader))
            {
                break;
            }

            auto packet_header_opt = try_extract_packet_header();

            if (!packet_header_opt)
            {
                continue;
            }

            auto packet_header = packet_header_opt.value();

            if (!is_valid_packet_size(packet_header))
            {
                std::cerr << "Invalid packet size\n";

                return frames;
            }

            auto frame_opt = try_extract_frame(packet_header);

            if (frame_opt)
            {
                frames.push_back(frame_opt.value());
            }
            else
            {
                break;
            }
        }
    }

    return frames;
}

bool PacketStreamClient::refill_buffer() {
    if (!m_server_connected)
    {
        return false;
    }

    std::array<std::byte, TEMP_BUFFER_SIZE> temp;

    auto bytes_received = m_client_socket.recv_data(temp.data(), temp.size());

    if (bytes_received <= 0)
    {
        disconnect();

        return false;
    }
    
    m_buffer.insert(m_buffer.end(), temp.data(), temp.data() + bytes_received);

    return true;
}

void PacketStreamClient::consume_buffer(size_t size) {
    if (size >= m_buffer.size())
    {
        m_buffer.clear();
    }
    else
    {
        m_buffer.erase(
            m_buffer.begin(),
            m_buffer.begin() + static_cast<ssize_t>(size)
        );
    }
}

std::optional<GamePacketHeader> PacketStreamClient::try_extract_packet_header() {
    if(m_buffer.size() < sizeof(GamePacketHeader))
    {
        return std::nullopt;
    }

    // Read the first 4 bytes of the buffer and check if its a magic number
    GamePacketHeader packet_header;
    memcpy(&packet_header, m_buffer.data(), sizeof(GamePacketHeader));

    if (packet_header.magic_number != m_magic_number)
    {
        m_buffer.erase(m_buffer.begin());

        return std::nullopt;
    }

    return packet_header;
}

std::optional<FrameSnapshot> PacketStreamClient::try_extract_frame(const GamePacketHeader& packet_header) {
    const auto total_packet_size = sizeof(GamePacketHeader) + packet_header.body_size;

    if (m_buffer.size() < total_packet_size)
    {
        const auto bytes_needed = sizeof(GamePacketHeader) + packet_header.body_size - m_buffer.size();

        // Receive extra bytes
        auto extra_packet_opt = m_client_socket.recv_exact(bytes_needed);
        
        if (!extra_packet_opt)
        {
            return std::nullopt;
        }
        
        // Join the rest of buffer and extra bytes
        m_buffer.insert(
            m_buffer.end(),
            extra_packet_opt->begin(),
            extra_packet_opt->end()
        );
    }

    std::vector<std::byte> frame_data(
        m_buffer.begin() + sizeof(GamePacketHeader),
        m_buffer.begin() + sizeof(GamePacketHeader) + packet_header.body_size
    );

    auto frame_opt = deserialize_frame(frame_data);

    if (frame_opt)
    {
        consume_buffer(total_packet_size);
    }

    return frame_opt;
}

bool PacketStreamClient::is_valid_packet_size(const GamePacketHeader& packet_header) {
    // Validation for packet size
    auto expr_1 = packet_header.body_size > 0;
    auto expr_2 = packet_header.body_size + sizeof(GamePacketHeader) <= m_max_packet_size;

    return expr_1 && expr_2;
}
