#include <cstdint>
#include <cstring>
#include "header_serializer.hpp"

namespace {
    /*
        Helper: Serialize any trivial struct
    */
    template <typename T>
    std::vector<std::byte> serialize_trivial_struct(const T& payload) {
        std::vector<std::byte> buffer(sizeof(T));
        std::memcpy(buffer.data(), &payload, sizeof(T));

        return buffer;
    }

    /*
        Helper: Deserialize any trivial struct
    */
    template <typename T>
    std::optional<T> deserialize_trivial_struct(const std::vector<std::byte>& buffer) {
        if (buffer.size() < sizeof(T))
        {
            return std::nullopt;
        }

        T result;
        std::memcpy(&result, buffer.data(), sizeof(T));

        return result;
    }
}

/*
    Serialize PacketHeader
*/
std::vector<std::byte> serialize_packet_header(const PacketHeader& header) {
    return serialize_trivial_struct(header);
}

/*
    Deserialize PacketHeader
*/
std::optional<PacketHeader> deserialize_packet_header(const std::vector<std::byte>& buffer) {
    auto header_opt = deserialize_trivial_struct<PacketHeader>(buffer);

    if (!header_opt || header_opt->magic_number != PACKET_MAGIC_NUMBER)
    {
        return std::nullopt;
    }

    return header_opt;
}
