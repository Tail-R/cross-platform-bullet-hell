#pragma once

#include <cstdint>
#include "game_enums.hpp"
#include "../frame.hpp"

constexpr size_t MAX_MESSAGE_SIZE = 256;

/*
    Game request
*/
struct ClientGameRequest {
    GameMode        play_mode;
    GameVariant     game_variant;
    GameDifficulty  game_difficulty;
    uint8_t         reserved_1;         // Reserved area
};

constexpr size_t CLIENT_GAME_REQUEST_SIZE = 4;
static_assert(sizeof(ClientGameRequest) == CLIENT_GAME_REQUEST_SIZE);

/*
    Game response
*/
struct ServerGameResponse {
    Accepted    accepted;
    uint32_t    session_id;
    uint32_t    reason_size;
    char        reason[MAX_MESSAGE_SIZE];
};

constexpr size_t SERVER_GAME_RESPONSE_SIZE = 12 + 256;
static_assert(sizeof(ServerGameResponse) == SERVER_GAME_RESPONSE_SIZE);

/*
    Reconnect request
*/
struct ClientReconnectRequest {
    uint32_t    client_id;
    uint32_t    session_id;
};

constexpr size_t CLIENT_RECONNECT_REQUEST_SIZE = 8;
static_assert(sizeof(ClientReconnectRequest) == CLIENT_RECONNECT_REQUEST_SIZE);

/*
    Reconnect response
*/
struct ServerReconnectResponse {
    Accepted    accepted;
    uint32_t    reason_size;
    char        reason[MAX_MESSAGE_SIZE];
};

constexpr size_t SERVER_RECONNECT_RESPONSE_SIZE = 8 + 256;
static_assert(sizeof(ServerReconnectResponse) == SERVER_RECONNECT_RESPONSE_SIZE);