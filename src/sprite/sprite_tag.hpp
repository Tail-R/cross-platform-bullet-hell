#pragma once

#include <cstdint>
#include <functional>

enum class SpriteType : uint8_t {
    Unknown,
    Stage,
    Player,
    Enemy,
    Boss,
    Bullet,
    Item
};

using SpriteName = uint8_t;

// A tag for resource
struct SpriteTag {
    SpriteType  type;
    SpriteName  name;
    uint32_t    id;

    /*
        The '== operator' is necessary to use this struct as a key for std::unordered_map.

        Note: 'operator==' and 'std::hash' intentionally ignore 'id'.
        This allows sharing resource entries in unordered_map<SpriteTag, Resource>
        by grouping keys with same 'type' and 'name'.
        Use 'strong_eq()' if 'id' must also match.
    */
    bool operator==(const SpriteTag& other) const {
        const auto expr_1 = type == other.type;
        const auto expr_2 = name == other.name;

        return expr_1 && expr_2;
    }

    bool explicit_eq(const SpriteTag& other) const {
        const auto expr_1 = type == other.type;
        const auto expr_2 = name == other.name;
        const auto expr_3 = id   == other.id;

        return expr_1 && expr_2 && expr_3;
    }
};

namespace std {
    // std::hash<SpriteTag> is necessary to use this struct as a key for std::unordered_map
    template<>
    struct hash<SpriteTag> {
        size_t operator()(const SpriteTag& tag) const noexcept {
            return (std::hash<uint8_t>()(static_cast<uint8_t>(tag.type)) << 8)
                 ^ std::hash<uint8_t>()(tag.name);
        }
    };
}