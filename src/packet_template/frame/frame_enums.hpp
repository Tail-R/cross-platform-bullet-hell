#pragma once

#include <cstdint>

enum class SnapshotType : uint8_t {
    Unknown,
    Stage,
    Player,
    Enemy,
    Boss,
    Bullet,
    Item
};

/***** Frame ******************************/
// Game mode
enum class GameMode : uint8_t {
    Default,
    Single,
    Match,
    Agent,
    Replay
};

// Game variant
enum class GameVariant : uint8_t {
    Default
};

// Game difficulty
enum class GameDifficulty : uint8_t {
    Default,
    Easy,
    Normal,
    Hard,
    Lunatic
};

// Game state
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

inline GameState operator|(GameState lhs, GameState rhs) {
    return static_cast<GameState>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
    );
}

inline GameState operator&(GameState lhs, GameState rhs) {
    return static_cast<GameState>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
    );
}

/***** Stage ******************************/
// Stage name
enum class StageName : uint8_t {
    Default = 0
};

// Stage state
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

inline StageState operator|(StageState lhs, StageState rhs) {
    return static_cast<StageState>(
        static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)
    );
}

inline StageState operator&(StageState lhs, StageState rhs) {
    return static_cast<StageState>(
        static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)
    );
}

/***** Player ******************************/
// Player name
enum class PlayerName : uint8_t {
    Default = 0
};

// Player state
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

/***** Enemy ******************************/
// Enemy name
enum class EnemyName : uint8_t {
    Default = 0
};

// Enemy state
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

/***** Boss ******************************/
// Boss name
enum class BossName : uint8_t {
    Default = 0
};

// Boss state
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

/***** Bullet ******************************/
// Bullet name
enum class BulletName : uint8_t {
    Default         = 0,
    
    // Normal bullet
    NormalRed       = 1,
    NormalOrange    = 2,
    NormalYellow    = 3,
    NormalGreen     = 4,
    NormalCyan      = 5,
    NormalBlue      = 6,
    NormalMagenta   = 7,
    NormalWhite     = 8,

    // Big bullet
    BigRed          = 9,
    BigOrange       = 10,
    BigYellow       = 11,
    BigGreen        = 12,
    BigCyan         = 13,
    BigBlue         = 14,
    BigMagenta      = 15,
    BigWhite        = 16,

    // Rice bullet
    RiceRed         = 17,
    RiceOrange      = 18,
    RiceYellow      = 19,
    RiceGreen       = 20,
    RiceCyan        = 21,
    RiceBlue        = 22,
    RiceMagenta     = 23,
    RiceWhite       = 24,

    // Wedge bullet
    WedgeRed        = 25,
    WedgeOrange     = 26,
    WedgeYellow     = 27,
    WedgeGreen      = 28,
    WedgeCyan       = 29,
    WedgeBlue       = 30,
    WedgeMagenta    = 31,
    WedgeWhite      = 32
};

// Bullet state
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

/***** Item ******************************/
// Item name
enum class ItemName : uint8_t {
    Default = 0
};

// Item state
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