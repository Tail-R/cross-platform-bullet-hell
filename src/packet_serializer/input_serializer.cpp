#include "input_serializer.hpp"

#include <cstdint>
#include <cstring>
#include "input_serializer.hpp"

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
std::vector<std::byte> serialize_client_input(const ClientInput& payload) {
    return serialize_trivial_struct(payload);
}

/*
    Deserialize
*/
std::optional<ClientInput> deserialize_client_input(const std::vector<std::byte>& buffer) {
    return deserialize_trivial_struct<ClientInput>(buffer);
}
