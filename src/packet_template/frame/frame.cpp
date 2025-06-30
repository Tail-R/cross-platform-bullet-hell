#include <iostream>
#include <string>
#include <sstream>
#include "frame.hpp"

std::string frame_to_json_str(const FrameSnapshot& frame) {
    std::ostringstream oss;

    oss << "{";

    /*
        Convert FrameSnapshot attributes into json string
    */
    oss << "\"frame\":{"
        << "\"client_id\":"     << static_cast<float>(frame.client_id)      << ","
        << "\"opponent_id\":"   << static_cast<float>(frame.opponent_id)    << ","
        << "\"timestamp\":"     << static_cast<float>(frame.timestamp)      << ","
        << "\"score\":"         << static_cast<float>(frame.score)          << ","
        << "\"mode\":"          << static_cast<float>(frame.mode)           << ","
        << "\"difficulty\":"    << static_cast<float>(frame.difficulty)     << ","
        << "\"state\":"         << static_cast<float>(frame.state)
        << "}"
        << ",";

    /*
        Convert Stage attributes into json string
    */
    oss << "\"stage\":{"
        << "\"id\":"            << static_cast<float>(frame.stage.id)           << ","
        << "\"name\":"          << static_cast<float>(frame.stage.name)         << ","
        << "\"state\":"         << static_cast<float>(frame.stage.state)        << ","
        << "\"next_stage\":"    << static_cast<float>(frame.stage.next_stage)   << ","
        << "\"timestamp\":"     << static_cast<float>(frame.stage.timestamp)
        << "},";

    /*
        Convert Player attributes into json string
    */
    oss << "\"player_count\":" << frame.player_count << ",";

    for (uint32_t i = 0; i < frame.player_count; i++)
    {
        oss << "\"player_" << i << "\":{"
        << "\"id\":"                << static_cast<float>(frame.player_vector[i].id)                << ","
        << "\"name\":"              << static_cast<float>(frame.player_vector[i].name)              << ","
        << "\"state\":"             << static_cast<float>(frame.player_vector[i].state)             << ","
        << "\"attack_pattern\":"    << static_cast<float>(frame.player_vector[i].attack_pattern)    << ","
        << "\"pos\":{"
        << "\"x\":"                 << static_cast<float>(frame.player_vector[i].pos.x)             << ","
        << "\"y\":"                 << static_cast<float>(frame.player_vector[i].pos.y)
        << "},"
        << "\"vel\":{"
        << "\"x\":"                 << static_cast<float>(frame.player_vector[i].vel.x)             << ","
        << "\"y\":"                 << static_cast<float>(frame.player_vector[i].vel.y)
        << "},"
        << "\"radius\":"            << static_cast<float>(frame.player_vector[i].radius)            << ","
        << "\"angle\":"             << static_cast<float>(frame.player_vector[i].angle)             << ","
        << "\"current_spell\":"     << static_cast<float>(frame.player_vector[i].current_spell)     << ","
        << "\"lives\":"             << static_cast<float>(frame.player_vector[i].lives)             << ","
        << "\"bombs\":"             << static_cast<float>(frame.player_vector[i].bombs)             << ","
        << "\"power\":"             << static_cast<float>(frame.player_vector[i].power)
        << "},";
    }

    /*
        Convert Enemy attributes into json string
    */
    oss << "\"enemy_count\":" << frame.enemy_count << ",";

    for (uint32_t i = 0; i < frame.enemy_count; i++)
    {
        oss << "\"enemy_" << i << "\":{"
        << "\"id\":"                << static_cast<float>(frame.enemy_vector[i].id)             << ","
        << "\"name\":"              << static_cast<float>(frame.enemy_vector[i].name)           << ","
        << "\"state\":"             << static_cast<float>(frame.enemy_vector[i].state)          << ","
        << "\"attack_pattern\":"    << static_cast<float>(frame.enemy_vector[i].attack_pattern) << ","
        << "\"pos\":{"
        << "\"x\":"                 << static_cast<float>(frame.enemy_vector[i].pos.x)          << ","
        << "\"y\":"                 << static_cast<float>(frame.enemy_vector[i].pos.y)
        << "},"
        << "\"vel\":{"
        << "\"x\":"                 << static_cast<float>(frame.enemy_vector[i].vel.x)          << ","
        << "\"y\":"                 << static_cast<float>(frame.enemy_vector[i].vel.y)
        << "},"
        << "\"radius\":"            << static_cast<float>(frame.enemy_vector[i].radius)         << ","
        << "\"angle\":"             << static_cast<float>(frame.enemy_vector[i].angle )         << ","
        << "\"health\":"            << static_cast<float>(frame.enemy_vector[i].health)
        << "},";
    }

    /*
        Convert Boss attributes into json string
    */
    oss << "\"boss_count\":" << frame.boss_count << ",";

    for (uint32_t i = 0; i < frame.boss_count; i++)
    {
        oss << "\"boss_" << i << "\":{"
        << "\"id\":"                << static_cast<float>(frame.boss_vector[i].id)              << ","
        << "\"name\":"              << static_cast<float>(frame.boss_vector[i].name)            << ","
        << "\"state\":"             << static_cast<float>(frame.boss_vector[i].state)           << ","
        << "\"attack_pattern\":"    << static_cast<float>(frame.boss_vector[i].attack_pattern)  << ","
        << "\"pos\":{"
        << "\"x\":"                 << static_cast<float>(frame.boss_vector[i].pos.x)           << ","
        << "\"y\":"                 << static_cast<float>(frame.boss_vector[i].pos.y)
        << "},"
        << "\"vel\":{"
        << "\"x\":"                 << static_cast<float>(frame.boss_vector[i].vel.x)           << ","
        << "\"y\":"                 << static_cast<float>(frame.boss_vector[i].vel.y)
        << "},"
        << "\"radius\":"            << static_cast<float>(frame.boss_vector[i].radius)          << ","
        << "\"angle\":"             << static_cast<float>(frame.boss_vector[i].angle)           << ","
        << "\"health\":"            << static_cast<float>(frame.boss_vector[i].health)          << ","
        << "\"current_spell\":"     << static_cast<float>(frame.boss_vector[i].current_spell)   << ","
        << "\"phase\":"             << static_cast<float>(frame.boss_vector[i].phase)
        << "},";
    }

    /*
        Convert Bullet attributes into json string
    */
    oss << "\"bullet_count\":"<< frame.bullet_count << ",";

    for (uint32_t i = 0; i < frame.bullet_count; i++)
    {
        oss << "\"bullet_" << i << "\":{"
        << "\"id\":"                << static_cast<float>(frame.bullet_vector[i].id)                << ","
        << "\"name\":"              << static_cast<float>(frame.bullet_vector[i].name)              << ","
        << "\"state\":"             << static_cast<float>(frame.bullet_vector[i].state)             << ","
        << "\"flight_pattern\":"    << static_cast<float>(frame.bullet_vector[i].flight_pattern)    << ","
        << "\"owner\":"             << static_cast<float>(frame.bullet_vector[i].owner)             << ","
        << "\"pos\":{"
        << "\"x\":"                 << static_cast<float>(frame.bullet_vector[i].pos.x)             << ","
        << "\"y\":"                 << static_cast<float>(frame.bullet_vector[i].pos.y)
        << "},"
        << "\"vel\":{"
        << "\"x\":"                 << static_cast<float>(frame.bullet_vector[i].vel.x)             << ","
        << "\"y\":"                 << static_cast<float>(frame.bullet_vector[i].vel.y)
        << "},"
        << "\"radius\":"            << static_cast<float>(frame.bullet_vector[i].radius)            << ","
        << "\"angle\":"             << static_cast<float>(frame.bullet_vector[i].angle)             << ","
        << "\"damage\":"            << static_cast<float>(frame.bullet_vector[i].damage)
        << "},";
    }
    
    /*
        Convert Item attributes into json string
    */
    if (frame.item_count > 0)
    {
        oss << "\"item_count\":" << frame.item_count << ",";
    }
    else
    {
        oss << "\"item_count\":" << frame.item_count;
    }

    for (uint32_t i = 0; i < frame.item_count; i++)
    {
        oss << "\"item_" << i << "\":{"
        << "\"id\":"                << static_cast<float>(frame.item_vector[i].id)              << ","
        << "\"name\":"              << static_cast<float>(frame.item_vector[i].name)            << ","
        << "\"state\":"             << static_cast<float>(frame.item_vector[i].state)           << ","
        << "\"flight_pattern\":"    << static_cast<float>(frame.item_vector[i].flight_pattern)  << ","
        << "\"pos\":{"
        << "\"x\":"                 << static_cast<float>(frame.item_vector[i].pos.x)           << ","
        << "\"y\":"                 << static_cast<float>(frame.item_vector[i].pos.y)
        << "},"
        << "\"vel\":{"
        << "\"x\":"                 << static_cast<float>(frame.item_vector[i].vel.x)           << ","
        << "\"y\":"                 << static_cast<float>(frame.item_vector[i].vel.y)
        << "},"
        << "\"radius\":"            << static_cast<float>(frame.item_vector[i].radius)          << ","
        << "\"angle\":"             << static_cast<float>(frame.item_vector[i].angle)           << ","
        << "\"score\":"             << static_cast<float>(frame.item_vector[i].score);

        if (i != (frame.item_count - 1))
        {
            oss << "},";
        }
        else
        {
            oss << "}";
        }
    }

    oss << "}";
        
    return oss.str();
}

void print_frame(const FrameSnapshot& frame) {
    std::cout << frame_to_json_str(frame) << "\n";
}