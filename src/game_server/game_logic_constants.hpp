#pragma once

#include <cstdint>

namespace game_logic_constants {
    constexpr float GAME_WIDTH              = 384.0f;
    constexpr float GAME_HEIGHT             = 448.0f;
    constexpr float GAME_WIDTH_HALF         = GAME_WIDTH / 2.0f;
    constexpr float GAME_HEIGHT_HALF        = GAME_HEIGHT / 2.0f;

    constexpr float PLAYER_RADIUS           = 10.0f;
    constexpr float PLAYER_SPEED            = 0.02f;
    constexpr float PLAYER_BULLET_RADIUS    = 5.0f;
    constexpr float PLAYER_BULLET_SPEED     = 20.0f;

    constexpr float ENEMY_RADIUS            = 10.0f;
    constexpr float ENEMY_SPEED             = 10.0f;
    constexpr float ENEMY_BULLET_RADIUS     = 5.0f;
    constexpr float ENEMY_BULLET_SPEED      = 20.0f;
}
