#pragma once

#include <cstdint>
#include <functional>

enum class SnapshotType : uint8_t {
    Unknown,
    Stage,
    Player,
    Enemy,
    Boss,
    Bullet,
    Item
};

// A tag for resource
struct SpriteTag {
    SnapshotType    type;
    uint8_t         name;

    // == operator is necessary to use this struct as a key for std::unordered_map
    bool operator==(const SpriteTag& other) const {
        return type == other.type && name == other.name;
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