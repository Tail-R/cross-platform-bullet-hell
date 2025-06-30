#pragma once

#include "../input_manager/input_snapshot.hpp"

/*
    Input
*/
struct ClientInput {
    uint32_t        client_id;
    InputDirection  direction;
};

constexpr size_t CLIENT_INPUT_SIZE = 8;
static_assert(sizeof(ClientInput) == CLIENT_INPUT_SIZE);