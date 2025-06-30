#pragma once

#include <vector>
#include <cstddef>
#include <optional>
#include "../packet_template/header.hpp"

/*
    Serializer
*/
std::vector<std::byte> serialize_packet_header(const PacketHeader& header);

/*
    Deserializer
*/
std::optional<PacketHeader> deserialize_packet_header(const std::vector<std::byte>& buffer);