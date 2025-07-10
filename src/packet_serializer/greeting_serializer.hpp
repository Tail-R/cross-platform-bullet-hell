#pragma once

#include <vector>
#include <cstddef>
#include <optional>
#include "../packet_template/greeting.hpp"

/*
    Serializer
*/
std::vector<std::byte> serialize_client_hello(const ClientHello& payload);
std::vector<std::byte> serialize_server_accept(const ServerAccept& payload);
std::vector<std::byte> serialize_client_goodbye(const ClientGoodbye& payload);
std::vector<std::byte> serialize_server_goodbye(const ServerGoodbye& payload);

/*
    Deserializer
*/
std::optional<ClientHello> deserialize_client_hello(const std::vector<std::byte>& buffer);
std::optional<ServerAccept> deserialize_server_accept(const std::vector<std::byte>& buffer);
std::optional<ClientGoodbye> deserialize_client_goodbye(const std::vector<std::byte>& buffer);
std::optional<ServerGoodbye> deserialize_server_goodbye(const std::vector<std::byte>& buffer);