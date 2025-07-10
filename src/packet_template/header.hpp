#pragma once

#include <cstdint>
#include <cstddef>

// A Magic number to synchronize the byte stream
constexpr uint32_t PACKET_MAGIC_NUMBER = 0x7F3B29D1;

enum class PayloadType : uint32_t {
    Unknown,
    ClientHello,
    ServerAccept,
    ClientGoodbye,
    ServerGoodbye,
    // ClientAuthRequest,
    // ServerAuthResponse,
    ClientGameRequest,
    ServerGameResponse,
    // ClientMatchRequest,
    // ServerMatchResponse
    ClientReconnectRequest,
    ServerReconnectResponse,
    ClientInput,
    FrameSnapshot,
    // Chat,
    // Info,
    // Error
};

/*
    Packet header (8bytes)
*/
struct PacketHeader {
    uint32_t    magic_number;
    uint32_t    sequence_number;
    uint32_t    payload_size;
    PayloadType payload_type;
};

constexpr size_t PACKET_HEADER_SIZE = 16;
static_assert(sizeof(PacketHeader) == PACKET_HEADER_SIZE);