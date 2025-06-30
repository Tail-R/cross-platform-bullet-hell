#pragma once

#include <cstdint>

constexpr uint32_t MAX_CLIENT_NAME_SIZE = 32;

/*
    Hello
*/
struct ClientHello {
    char client_name[MAX_CLIENT_NAME_SIZE];
};

constexpr size_t CLIENT_HELLO_SIZE = 32;
static_assert(sizeof(ClientHello) == CLIENT_HELLO_SIZE);

/*
    Accept
*/
struct ServerAccept {
    uint32_t assigned_client_id;
};

/*
    GoodBye
*/
struct ClientGoodBye {
    uint32_t reason_code;
};

using ServerGoodBye = ClientGoodBye;

constexpr size_t CLIENT_GOOD_BYE_SIZE = 4;
constexpr size_t SERVER_GOOD_BYE_SIZE = 4;

static_assert(
    sizeof(ClientGoodBye) == CLIENT_GOOD_BYE_SIZE &&
    sizeof(ServerGoodBye) == SERVER_GOOD_BYE_SIZE
);