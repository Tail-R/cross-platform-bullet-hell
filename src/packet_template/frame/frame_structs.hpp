#pragma once

#include <vector>
#include <cstdint>
#include <cstddef>

/*
    Position2D (8bytes)
*/
struct Position2D {
    float x;
    float y;
};

static_assert(sizeof(Position2D) == 8);

/*
    Velocity2D (8bytes)
*/
struct Velocity2D {
    float x;
    float y;
};

static_assert(sizeof(Velocity2D) == 8);

/*
    Stage snapshot (8bytes)
*/
struct StageSnapshot {
    uint8_t     id;
    StageName   name;
    StageState  state;
    uint8_t     next_stage;

    uint32_t    timestamp;
};

constexpr size_t STAGE_SNAPSHOT_SIZE = 8;
static_assert(sizeof(StageSnapshot) == STAGE_SNAPSHOT_SIZE);

/*
    Player snapshot (32bytes)
*/
struct PlayerSnapshot {
    uint8_t     id;
    PlayerName  name;
    PlayerState state;
    uint8_t     attack_pattern;

    Position2D  pos;
    Velocity2D  vel;
    float       radius;
    float       angle;

    uint8_t     current_spell;
    uint8_t     lives;
    uint8_t     bombs;
    uint8_t     power;
};

constexpr size_t PLAYER_SNAPSHOT_SIZE = 32;
static_assert(sizeof(PlayerSnapshot) == PLAYER_SNAPSHOT_SIZE);

/*
    Enemy snapshot (32bytes)
*/
struct EnemySnapshot {
    uint8_t     id;
    EnemyName   name;
    EnemyState  state;
    uint8_t     attack_pattern;
    
    Position2D  pos;
    Velocity2D  vel;
    float       radius;
    float       angle;
    uint32_t    health;
};

constexpr size_t ENEMY_SNAPSHOT_SIZE = 32;
static_assert(sizeof(EnemySnapshot) == ENEMY_SNAPSHOT_SIZE);

/*
    Boss snapshot (36bytes)
*/
struct BossSnapshot {
    uint8_t     id;
    BossName    name;
    BossState   state;
    uint8_t     attack_pattern;

    Position2D  pos;
    Velocity2D  vel;
    float       radius;
    float       angle;
    uint32_t    health;

    uint8_t     current_spell;
    uint8_t     phase;
    uint8_t     reserved_01;    // Reserved area
    uint8_t     reserved_02;    // Reserved area
};

constexpr size_t BOSS_SNAPSHOT_SIZE = 36;
static_assert(sizeof(BossSnapshot) == BOSS_SNAPSHOT_SIZE);

/*
    Bullet snapshot (32bytes)
*/
struct BulletSnapshot {
    uint32_t    id;
    Position2D  pos;
    Velocity2D  vel;
    float       radius;
    float       angle;
    uint32_t    damage;

    BulletName  name;
    BulletState state;
    uint8_t     flight_pattern;
    uint8_t     owner;
};

constexpr size_t BULLET_SNAPSHOT_SIZE = 36;
static_assert(sizeof(BulletSnapshot) == BULLET_SNAPSHOT_SIZE);

/*
    Item snapshot (32bytes)
*/
struct ItemSnapshot {
    uint8_t     id;
    ItemName    name;
    ItemState   state;
    uint8_t     flight_pattern;

    Position2D  pos;
    Velocity2D  vel;
    float       radius;
    float       angle;
    float       score;
};

constexpr size_t ITEM_SNAPSHOT_SIZE = 32;
static_assert(sizeof(ItemSnapshot) == ITEM_SNAPSHOT_SIZE);

/*
    Frame snapshot
*/
struct FrameSnapshot {
    uint32_t        client_id;
    uint32_t        opponent_id;
    uint32_t        timestamp;
    uint32_t        score;

    GameMode        mode;
    GameVariant     variant;
    GameDifficulty  difficulty;
    GameState       state;

    /***** 16 bytes total *****/

    // Stage snapshot               [8bytes]
    StageSnapshot                   stage;

    // Player snapshot              [32bytes * n]
    uint32_t                        player_count;
    std::vector<PlayerSnapshot>     player_vector;

    // Enemy snapshot               [32bytes * n]
    uint32_t                        enemy_count;
    std::vector<EnemySnapshot>      enemy_vector;

    // Boss snapshot                [36bytes * n]
    uint32_t                        boss_count;
    std::vector<BossSnapshot>       boss_vector;

    // Bullet snapshot              [32bytes * n]
    uint32_t                        bullet_count;
    std::vector<BulletSnapshot>     bullet_vector;

    // Item snapshot                [32bytes * n]
    uint32_t                        item_count;
    std::vector<ItemSnapshot>       item_vector;
};

constexpr size_t FRAME_SNAPSHOT_FIXED_AREA_SIZE = sizeof(uint32_t) * 4
                                                    + sizeof(GameMode)
                                                    + sizeof(GameVariant)
                                                    + sizeof(GameDifficulty)
                                                    + sizeof(GameState);

static_assert(FRAME_SNAPSHOT_FIXED_AREA_SIZE == 20);