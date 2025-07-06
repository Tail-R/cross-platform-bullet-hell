#include "input_serializer.hpp"

#include <cstdint>
#include <cstring>
#include "input_serializer.hpp"

namespace {
    template <size_t N>
    void serialize_bitset(const std::bitset<N>& bits, std::vector<std::byte>& out) {
        constexpr size_t byte_size = (N + 7) / 8;

        for (size_t i = 0; i < byte_size; i++)
        {
            uint8_t byte = 0;

            for (size_t b = 0; b < 8; b++)
            {
                size_t bit_index = i * 8 + b;

                if (bit_index < N && bits[bit_index])
                {
                    byte |= (1 << b);
                }
            }

            out.push_back(static_cast<std::byte>(byte));
        }
    }

    template <size_t N>
    std::bitset<N> deserialize_bitset(const std::vector<std::byte>& in, size_t& offset) {
        constexpr size_t byte_size = (N + 7) / 8;
        std::bitset<N> bits;

        for (size_t i = 0; i < byte_size; i++)
        {
            uint8_t byte = std::to_integer<uint8_t>(in[offset++]);

            for (size_t b = 0; b < 8; b++)
            {
                size_t bit_index = i * 8 + b;

                if (bit_index < N)
                {
                    bits[bit_index] = (byte >> b) & 1;
                }
            }
        }

        return bits;
    }

    /*
        Arrow state
    */
    void serialize_arrow_state(const ArrowState& arrows, std::vector<std::byte>& out) {
        serialize_bitset(arrows.held,       out);
        serialize_bitset(arrows.pressed,    out);
        serialize_bitset(arrows.released,   out);
    }

    ArrowState deserialize_arrow_state(const std::vector<std::byte>& buffer, size_t& offset) {
        constexpr size_t ARROW_COUNT = static_cast<size_t>(Arrow::Count);

        return ArrowState {
            deserialize_bitset<ARROW_COUNT>(buffer, offset),  // held
            deserialize_bitset<ARROW_COUNT>(buffer, offset),  // pressed
            deserialize_bitset<ARROW_COUNT>(buffer, offset)   // released
        };
    }

    /*
        Game input
    */
    void serialize_game_input(const GameInput& input, std::vector<std::byte>& out) {
        serialize_bitset(input.held,        out);
        serialize_bitset(input.pressed,     out);
        serialize_bitset(input.released,    out);

        serialize_arrow_state(input.arrows, out);
    }

    GameInput deserialize_game_input(const std::vector<std::byte>& buffer, size_t& offset) {
        constexpr size_t GAME_ACTION_COUNT = static_cast<size_t>(GameAction::Count);

        return {
            deserialize_bitset<GAME_ACTION_COUNT>(buffer, offset), // held
            deserialize_bitset<GAME_ACTION_COUNT>(buffer, offset), // pressed
            deserialize_bitset<GAME_ACTION_COUNT>(buffer, offset), // released

            deserialize_arrow_state(buffer, offset) // arrows
        };
    }

    /*
        uint32_t
    */
    void serialize_uint32_t(uint32_t value, std::vector<std::byte>& out) {
        for (size_t i = 0; i < sizeof(uint32_t); i++)
        {
            out.push_back(static_cast<std::byte>((value >> (i * 8)) & 0xFF));
        }
    }

    uint32_t deserialize_uint32_t(const std::vector<std::byte>& buffer, size_t& offset) {
        uint32_t value = 0;

        for (size_t i = 0; i < sizeof(uint32_t); i++)
        {
            value |= static_cast<uint32_t>(std::to_integer<uint8_t>(buffer[offset++])) << (i * 8);
        }

        return value;
    }
}

/*
    Serialize
*/
std::vector<std::byte> serialize_client_input(const ClientInput& payload) {
    std::vector<std::byte> buffer;

    buffer.reserve(32); // Optimization

    serialize_uint32_t(payload.client_id,       buffer);
    serialize_uint32_t(payload.frame_timestamp, buffer);
    serialize_game_input(payload.game_input,    buffer);

    return buffer;
}

/*
    Deserialize
*/
std::optional<ClientInput> deserialize_client_input(const std::vector<std::byte>& buffer) {
    if (buffer.size() > CLIENT_INPUT_SIZE)
    {
        return std::nullopt;
    }

    size_t offset = 0;

    return ClientInput {
        deserialize_uint32_t(buffer,    offset),    // client_id
        deserialize_uint32_t(buffer,    offset),    // frame_timestamp
        deserialize_game_input(buffer,  offset)     // state
    };
}
