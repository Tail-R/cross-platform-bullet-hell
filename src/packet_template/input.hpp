#pragma once

#include "../input_manager/input_snapshot.hpp"

/*
    ClientInput has an std::bitset member, so its size is environment-dependent.
    Therefore, the serializer/deserializer is designed to convert std::bitset
    to an environment-independent intermediate representation of std::vector type
    before converting it to payload.
*/

/*
    Input
*/
struct ClientInput {
    uint32_t    client_id;
    uint32_t    frame_timestamp;
    GameInput   game_input;
};

constexpr size_t CLIENT_INPUT_SIZE = sizeof(ClientInput);