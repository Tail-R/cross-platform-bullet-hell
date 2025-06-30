#include <iostream>
#include <cstring>
#include "frame_serializer.hpp"

namespace {
    template <typename T>
    const std::byte* copy_bytes_to_t(T* dest, const std::byte* src) {
        static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
        memcpy(dest, src, sizeof(T));

        return src + sizeof(T);
    }
}

std::optional<std::vector<std::byte>> serialize_frame(const FrameSnapshot& frame) {
    auto player_count_validation = frame.player_count != frame.player_vector.size(); 
    auto enemy_count_validation = frame.enemy_count != frame.enemy_vector.size();
    auto boss_count_validation = frame.bullet_count != frame.bullet_vector.size();
    auto bullet_count_validation = frame.bullet_count != frame.bullet_vector.size();
    auto item_count_validation = frame.item_count != frame.item_vector.size();

    // Check if the number of objects and actual size of objects are same
    if (player_count_validation || enemy_count_validation || boss_count_validation ||
        bullet_count_validation || item_count_validation)
    {
        std::cerr << "[serialize_frame] Failed to serialize frame" << "\n";
        std::cerr << "[serialize_frame] The number of objects and the size of objects does not match" << "\n";
        
        return std::nullopt;
    }

    // Calculate the total size of the packet (frame)
    auto packet_size = FRAME_SNAPSHOT_FIXED_HEADER_SIZE +
        STAGE_SNAPSHOT_SIZE +
        sizeof(frame.player_count) +
        PLAYER_SNAPSHOT_SIZE * frame.player_count +
        sizeof(frame.enemy_count) +
        ENEMY_SNAPSHOT_SIZE * frame.enemy_count +
        sizeof(frame.boss_count) +
        BOSS_SNAPSHOT_SIZE * frame.boss_count +
        sizeof(frame.bullet_count) +
        BULLET_SNAPSHOT_SIZE * frame.bullet_count +
        sizeof(frame.item_count) +
        ITEM_SNAPSHOT_SIZE * frame.item_count;

    std::vector<std::byte> bytes(packet_size);
    auto bytes_offset = bytes.data();

    // Pack the fixed header of frame object
    memcpy(
        bytes_offset,
        &frame,
        FRAME_SNAPSHOT_FIXED_HEADER_SIZE
    );

    bytes_offset += FRAME_SNAPSHOT_FIXED_HEADER_SIZE;

    // Pack the stage object
    memcpy(
        bytes_offset,
        &frame.stage,
        STAGE_SNAPSHOT_SIZE
    );

    bytes_offset += STAGE_SNAPSHOT_SIZE;

    // Pack the player objects
    memcpy(
        bytes_offset,
        &frame.player_count,
        sizeof(frame.player_count)
    );

    bytes_offset += sizeof(frame.player_count);

    memcpy(
        bytes_offset,
        frame.player_vector.data(),
        PLAYER_SNAPSHOT_SIZE * frame.player_count
    );

    bytes_offset += PLAYER_SNAPSHOT_SIZE * frame.player_count;

    // Pack the enemy objects
    memcpy(
        bytes_offset,
        &frame.enemy_count,
        sizeof(frame.enemy_count)
    );

    bytes_offset += sizeof(frame.enemy_count);

    memcpy(
        bytes_offset,
        frame.enemy_vector.data(),
        ENEMY_SNAPSHOT_SIZE * frame.enemy_count
    );

    bytes_offset += ENEMY_SNAPSHOT_SIZE * frame.enemy_count;

    // Pack the boss objects
    memcpy(
        bytes_offset,
        &frame.boss_count,
        sizeof(frame.boss_count)
    );

    bytes_offset += sizeof(frame.boss_count);

    memcpy(
        bytes_offset,
        frame.boss_vector.data(),
        BOSS_SNAPSHOT_SIZE * frame.boss_count
    );

    bytes_offset += BOSS_SNAPSHOT_SIZE * frame.boss_count;

    // Pack the bullet objects
    memcpy(
        bytes_offset,
        &frame.bullet_count,
        sizeof(frame.bullet_count)
    );

    bytes_offset += sizeof(frame.bullet_count);

    memcpy(
        bytes_offset,
        frame.bullet_vector.data(),
        BULLET_SNAPSHOT_SIZE * frame.bullet_count
    );

    bytes_offset += BULLET_SNAPSHOT_SIZE * frame.bullet_count;

    // Pack the item objects
    memcpy(
        bytes_offset,
        &frame.item_count,
        sizeof(frame.item_count)
    );

    bytes_offset += sizeof(frame.item_count);

    memcpy(
        bytes_offset,
        frame.item_vector.data(),
        ITEM_SNAPSHOT_SIZE * frame.item_count
    );

    bytes_offset += ITEM_SNAPSHOT_SIZE * frame.item_count;

    return bytes;
}

std::optional<FrameSnapshot> deserialize_frame(const std::vector<std::byte>& bytes) {
    FrameSnapshot frame = {};
    auto bytes_offset = bytes.data();

    // Copy the fixed area of the frame object
    bytes_offset = copy_bytes_to_t(&frame.client_id,    bytes_offset);
    bytes_offset = copy_bytes_to_t(&frame.opponent_id,  bytes_offset);
    bytes_offset = copy_bytes_to_t(&frame.timestamp,    bytes_offset);
    bytes_offset = copy_bytes_to_t(&frame.score,        bytes_offset);
    bytes_offset = copy_bytes_to_t(&frame.mode,         bytes_offset);
    bytes_offset = copy_bytes_to_t(&frame.difficulty,   bytes_offset);
    bytes_offset = copy_bytes_to_t(&frame.state,        bytes_offset);
    bytes_offset = copy_bytes_to_t(&frame.reserved_01,  bytes_offset);
    // Copy the stage object
    bytes_offset = copy_bytes_to_t(&frame.stage, bytes_offset);

    // Acquire the number of players and then copy the player objects
    bytes_offset = copy_bytes_to_t(&frame.player_count, bytes_offset);
    frame.player_vector.resize(frame.player_count);

    memcpy(
        frame.player_vector.data(),
        bytes_offset,
        PLAYER_SNAPSHOT_SIZE * frame.player_count
    );

    bytes_offset += PLAYER_SNAPSHOT_SIZE * frame.player_count;

    // Acquire the number of enemies and then copy the enemy objects
    bytes_offset = copy_bytes_to_t(&frame.enemy_count, bytes_offset);
    frame.enemy_vector.resize(frame.enemy_count);

    memcpy(
        frame.enemy_vector.data(),
        bytes_offset,
        ENEMY_SNAPSHOT_SIZE * frame.enemy_count
    );

    bytes_offset += ENEMY_SNAPSHOT_SIZE * frame.enemy_count;

    // // Acquire the number of bosses and then copy the boss objects
    bytes_offset = copy_bytes_to_t(&frame.boss_count, bytes_offset);
    frame.boss_vector.resize(frame.boss_count);

    memcpy(
        frame.boss_vector.data(),
        bytes_offset,
        BOSS_SNAPSHOT_SIZE * frame.boss_count
    );

    bytes_offset += BOSS_SNAPSHOT_SIZE * frame.boss_count;

    // // Acquire the number of bullets and then copy the bullet objects
    bytes_offset = copy_bytes_to_t(&frame.bullet_count, bytes_offset);
    frame.bullet_vector.resize(frame.bullet_count);

    memcpy(
        frame.bullet_vector.data(),
        bytes_offset,
        BULLET_SNAPSHOT_SIZE * frame.bullet_count
    );

    bytes_offset += BULLET_SNAPSHOT_SIZE * frame.bullet_count;

    // Acquire the number of items and then copy the item objects
    bytes_offset = copy_bytes_to_t(&frame.item_count, bytes_offset);
    frame.item_vector.resize(frame.item_count);

    memcpy(
        frame.item_vector.data(),
        bytes_offset,
        ITEM_SNAPSHOT_SIZE * frame.item_count
    );

    bytes_offset += ITEM_SNAPSHOT_SIZE * frame.item_count;

    return frame;
}