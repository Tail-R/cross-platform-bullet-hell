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
    ClientGoodBye,
    ServerGoodBye,
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

    PayloadType     payload_type();
};
