#pragma once

#include <cstdint>

// A Magic number to synchronize the byte stream
constexpr uint32_t PACKET_MAGIC_NUMBER = 0x7F3B29D1;

/*
    Packet header (8bytes)
*/
struct PacketHeader {
    uint32_t magic_number;
    uint32_t sequence_number;
    uint32_t payload_size;
    uint32_t payload_type;
};

constexpr size_t PACKET_HEADER_SIZE = 16;
static_assert(sizeof(PacketHeader) == PACKET_HEADER_SIZE);

enum class PayloadType : uint32_t {
    Unknown,
    Hello,
    Accept,
    GoodBye,
    // AuthRequest,
    // AuthResponse,
    GameRequest,
    GameResponse,
    // MatchRequest,
    // MatchResponse
    ReconnectRequest,
    ReconnectResponse,
    Input,
    FrameSnapshot,
    // Chat,
    // Info,
    // Error
};