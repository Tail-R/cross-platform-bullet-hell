#include <cstdint>
#include <cstring>
#include "greeting_serializer.hpp"

namespace {
    /*
        Helper function that serialize any trivial struct into a std::vector<std::byte>
    */
    template <typename T>
    std::vector<std::byte> serialize_trivial_struct(const T& payload) {
        std::vector<std::byte> buffer(sizeof(T));

        std::memcpy(buffer.data(), &payload, sizeof(T));

        return buffer;
    }

    /*
        Helper function that deserialize any trivial struct from std::vector<std::byte>
    */
    template <typename T>
    std::optional<T> deserialize_trivial_struct(const std::vector<std::byte>& buffer) {
        if (buffer.size() < sizeof(T)) {
            return std::nullopt;
        }

        T result;
        std::memcpy(&result, buffer.data(), sizeof(T));
        return result;
    }
}

/*
    Serializer
*/
std::vector<std::byte> serialize_client_hello(const ClientHello& payload) {
    return serialize_trivial_struct(payload);
}

std::vector<std::byte> serialize_server_accept(const ServerAccept& payload) {
    return serialize_trivial_struct(payload);
}

std::vector<std::byte> serialize_client_goodbye(const ClientGoodBye& payload) {
    return serialize_trivial_struct(payload);
}

std::vector<std::byte> serialize_server_goodbye(const ServerGoodBye& payload) {
    return serialize_trivial_struct(payload);
}

/*
    Deserializer
*/
std::optional<ClientHello> deserialize_client_hello(const std::vector<std::byte>& buffer) {
    if (buffer.size() < CLIENT_HELLO_SIZE)
    {
        return std::nullopt;
    }

    ClientHello result;

    std::memcpy(&result.client_name_size, buffer.data(), sizeof(result.client_name_size));
    std::memcpy(result.client_name,
                buffer.data() + sizeof(result.client_name_size),
                MAX_CLIENT_NAME_SIZE);

    return result;
}

std::optional<ServerAccept> deserialize_server_accept(const std::vector<std::byte>& buffer) {
    return deserialize_trivial_struct<ServerAccept>(buffer);
}

std::optional<ClientGoodBye> deserialize_client_goodbye(const std::vector<std::byte>& buffer) {
    return deserialize_trivial_struct<ClientGoodBye>(buffer);
}

std::optional<ServerGoodBye> deserialize_server_goodbye(const std::vector<std::byte>& buffer) {
    return deserialize_trivial_struct<ServerGoodBye>(buffer);
}