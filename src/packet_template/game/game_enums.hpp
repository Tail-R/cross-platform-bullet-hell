#pragma once

#include <cstdint>

enum class PlayMode : uint32_t {
    Default
};

enum class GameVariant : uint32_t {
    Default
};

enum class GameDifficulty : uint32_t {
    Default,
    Easy,
    Normal,
    Hard,
    Lunatic
};

enum class Accepted : uint32_t {
    Rejected,
    Accepted
};