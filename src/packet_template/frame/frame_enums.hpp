#pragma once

#include <cstdint>

/*
    Game state
*/
enum class GameState : uint8_t {
    None        = 0,
    Playing     = 1 << 0,
    Paused      = 1 << 1,
    GameOver    = 1 << 2,
    GameClear   = 1 << 3,
    Reserved_1  = 1 << 4,   // Reserved bit
    Reserved_2  = 1 << 5,   // Reserved bit
    Reserved_3  = 1 << 6,   // Reserved bit
    Reserved_4  = 1 << 7,   // Reserved bit
};

/*
    Stage state
*/
enum class StageState : uint8_t {
    None        = 0,
    Intro       = 1 << 0,
    Main        = 1 << 1,
    Outro       = 1 << 2,
    Reserved_1  = 1 << 3,   // Reserved bit
    Reserved_2  = 1 << 4,   // Reserved bit
    Reserved_3  = 1 << 5,   // Reserved bit
    Reserved_4  = 1 << 6,   // Reserved bit
    Reserved_5  = 1 << 7,   // Reserved bit
};

/*
    Player state
*/
enum class PlayerState : uint8_t {
    None            = 0,
    Visible         = 1 << 0,
    Spawn           = 1 << 1,
    Attacking       = 1 << 2,
    Focusing        = 1 << 3,
    SpellActivated  = 1 << 4,
    Dying           = 1 << 5,
    Dead            = 1 << 6,
    Invincible      = 1 << 7,
};

inline PlayerState operator|(PlayerState lhs, PlayerState rhs) {
    return static_cast<PlayerState>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
    );
}

inline PlayerState operator&(PlayerState lhs, PlayerState rhs) {
    return static_cast<PlayerState>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
    );
}

/*
    Enemy state
*/
enum class EnemyState : uint8_t {
    None            = 0,
    Visible         = 1 << 0,
    Spawn           = 1 << 1,
    Attacking       = 1 << 2,
    Charging        = 1 << 3,
    SpellActivated  = 1 << 4,
    Dying           = 1 << 5,
    Dead            = 1 << 6,
    Invincible      = 1 << 7,
};

inline EnemyState operator|(EnemyState lhs, EnemyState rhs) {
    return static_cast<EnemyState>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
    );
}

inline EnemyState operator&(EnemyState lhs, EnemyState rhs) {
    return static_cast<EnemyState>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
    );
}

/*
    Boss state
*/
enum class BossState : uint8_t {
    None            = 0,
    Visible         = 1 << 0,
    Spawn           = 1 << 1,
    Attacking       = 1 << 2,
    Charging        = 1 << 3,
    SpellActivated  = 1 << 4,
    Dying           = 1 << 5,
    Dead            = 1 << 6,
    Invincible      = 1 << 7,
};

inline BossState operator|(BossState lhs, BossState rhs) {
    return static_cast<BossState>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
    );
}

inline BossState operator&(BossState lhs, BossState rhs) {
    return static_cast<BossState>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
    );
}

/*
    Bullet state
*/
enum class BulletState : uint8_t {
    None        = 0,
    Visible     = 1 << 0,
    Spawn       = 1 << 1,
    Reserved_1  = 1 << 2,   // Reserved bit
    Reserved_2  = 1 << 3,   // Reserved bit
    Reserved_3  = 1 << 4,   // Reserved bit
    Dying       = 1 << 5,
    Dead        = 1 << 6,   
    Reserved_4  = 1 << 7,   // Reserved bit
};

inline BulletState operator|(BulletState lhs, BulletState rhs) {
    return static_cast<BulletState>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
    );
}

inline BulletState operator&(BulletState lhs, BulletState rhs) {
    return static_cast<BulletState>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
    );
}

/*
    Item state
*/
enum class ItemState : uint8_t {
    None        = 0,
    Visible     = 1 << 0,
    Spawn       = 1 << 1,
    Reserved_1  = 1 << 2,   // Reserved bit
    Reserved_2  = 1 << 3,   // Reserved bit
    Reserved_3  = 1 << 4,   // Reserved bit
    Dying       = 1 << 5,
    Dead        = 1 << 6,   
    Reserved_4  = 1 << 7,   // Reserved bit
};

inline ItemState operator|(ItemState lhs, ItemState rhs) {
    return static_cast<ItemState>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
    );
}

inline ItemState operator&(ItemState lhs, ItemState rhs) {
    return static_cast<ItemState>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
    );
}