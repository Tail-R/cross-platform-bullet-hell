#include <cstdint>
#include <cstring>
#include <vector>
#include <optional>
#include "game_serializer.hpp"

namespace {
    /*
        Helper: serialize trivial POD types
    */
    template <typename T>
    std::vector<std::byte> serialize_trivial_struct(const T& payload) {
        std::vector<std::byte> buffer(sizeof(T));
        std::memcpy(buffer.data(), &payload, sizeof(T));
        return buffer;
    }

    /*
        Helper: deserialize trivial POD types
    */
    template <typename T>
    std::optional<T> deserialize_trivial_struct(const std::vector<std::byte>& buffer) {
        if (buffer.size() < sizeof(T)) return std::nullopt;

        T result;
        std::memcpy(&result, buffer.data(), sizeof(T));
        return result;
    }
}

/*
    Serialize
*/
std::vector<std::byte> serialize_client_game_request(const ClientGameRequest& payload) {
    return serialize_trivial_struct(payload);
}

std::vector<std::byte> serialize_server_game_response(const ServerGameResponse& payload) {
    return serialize_trivial_struct(payload);
}

std::vector<std::byte> serialize_client_reconnect_request(const ClientReconnectRequest& payload) {
    return serialize_trivial_struct(payload);
}

std::vector<std::byte> serialize_server_reconnect_response(const ServerReconnectResponse& payload) {
    return serialize_trivial_struct(payload);
}

/*
    Deserialize
*/
std::optional<ClientGameRequest> deserialize_client_game_request(const std::vector<std::byte>& buffer) {
    return deserialize_trivial_struct<ClientGameRequest>(buffer);
}

std::optional<ServerGameResponse> deserialize_server_game_response(const std::vector<std::byte>& buffer) {
    return deserialize_trivial_struct<ServerGameResponse>(buffer);
}

std::optional<ClientReconnectRequest> deserialize_client_reconnect_request(const std::vector<std::byte>& buffer) {
    return deserialize_trivial_struct<ClientReconnectRequest>(buffer);
}

std::optional<ServerReconnectResponse> deserialize_server_reconnect_response(const std::vector<std::byte>& buffer) {
    return deserialize_trivial_struct<ServerReconnectResponse>(buffer);
}
