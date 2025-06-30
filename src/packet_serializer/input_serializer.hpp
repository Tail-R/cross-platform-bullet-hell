#pragma once

#include <vector>
#include <cstddef>
#include <optional>
#include "../packet_template/input.hpp"

/*
    Serializer
*/
std::vector<std::byte> serialize_client_input(const ClientInput& payload);

/*
    Deserializer
*/
std::optional<ClientInput> deserialize_client_input(const std::vector<std::byte>& buffer);
