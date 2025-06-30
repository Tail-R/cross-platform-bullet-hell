#pragma once

#include <cstdint>

constexpr size_t MAX_MESSAGE_SIZE = 256;

/*
    GameRequest
*/
struct ClientGameRequest {
    uint32_t    client_id;
    uint32_t    game_mode;
    uint32_t    stage_id;
};

/*
    GameResponse
*/
struct ServerGameResponse {
    uint32_t    accepted;
    uint32_t    match_id;
    char        reason[MAX_MESSAGE_SIZE];
};