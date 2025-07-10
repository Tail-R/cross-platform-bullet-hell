#pragma once

#include <variant>
#include "header.hpp"
#include "greeting.hpp"
#include "game.hpp"
#include "frame.hpp"
#include "input.hpp"

using PacketPayload = std::variant<
    ClientHello,
    ServerAccept,
    ClientGoodbye,
    ServerGoodbye,
    ClientGameRequest,
    ServerGameResponse,
    ClientReconnectRequest,
    ServerReconnectResponse,
    FrameSnapshot,
    ClientInput
>;

struct Packet {
    PacketHeader    header;
    PacketPayload   payload;
};

PayloadType get_payload_type(const PacketPayload& payload);

template <typename T>
Packet make_packet(const T& payload) {
    const auto payload_type = get_payload_type(payload);

    Packet pkt;
    
    pkt.header.magic_number     = PACKET_MAGIC_NUMBER;
    pkt.header.sequence_number  = 0;
    pkt.header.payload_size     = 0;
    pkt.header.payload_type     = payload_type;

    pkt.payload = payload;

    return pkt;
}