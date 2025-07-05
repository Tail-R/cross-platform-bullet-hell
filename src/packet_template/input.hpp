#pragma once

#include "../input_manager/input_snapshot.hpp"

/*
    Input
*/
struct ClientInput {
    uint32_t    client_id;
    uint32_t    frame_timestamp;
    GameInput   state;
};

constexpr size_t CLIENT_INPUT_SIZE = 32;
static_assert(sizeof(ClientInput) == CLIENT_INPUT_SIZE);