#pragma once

#include <vector>
#include <cstddef>
#include <optional>
#include "../packet_template/game.hpp"

/*
    Serializer
*/
std::vector<std::byte> serialize_client_game_request(const ClientGameRequest&);
std::vector<std::byte> serialize_server_game_response(const ServerGameResponse&);
std::vector<std::byte> serialize_client_reconnect_request(const ClientReconnectRequest&);
std::vector<std::byte> serialize_server_reconnect_response(const ServerReconnectResponse&);

/*
    Deserializer
*/
std::optional<ClientGameRequest> deserialize_client_game_request(const std::vector<std::byte>&);
std::optional<ServerGameResponse> deserialize_server_game_response(const std::vector<std::byte>&);
std::optional<ClientReconnectRequest> deserialize_client_reconnect_request(const std::vector<std::byte>&);
std::optional<ServerReconnectResponse> deserialize_server_reconnect_response(const std::vector<std::byte>&);
