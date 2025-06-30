#pragma once

#include <cstdint>
#include "greeting_enums.hpp"

constexpr uint32_t MAX_CLIENT_NAME_SIZE = 32;

/*
    Hello
*/
struct ClientHello {
    uint32_t client_name_size;
    char client_name[MAX_CLIENT_NAME_SIZE];
};

constexpr size_t CLIENT_HELLO_SIZE = 36;
static_assert(sizeof(ClientHello) == CLIENT_HELLO_SIZE);

/*
    Accept
*/
struct ServerAccept {
    uint32_t assigned_client_id;
};

/*
    Goodbye
*/
struct ClientGoodBye {
    GoodByeReasonCode reason_code;
};

constexpr size_t CLIENT_GOODBYE_SIZE = 4;
static_assert(sizeof(ClientGoodBye) == CLIENT_GOODBYE_SIZE);

struct ServerGoodBye {
    GoodByeReasonCode reason_code;
};

constexpr size_t SERVER_GOODBYE_SIZE = 4;
static_assert(sizeof(ServerGoodBye) == SERVER_GOODBYE_SIZE);