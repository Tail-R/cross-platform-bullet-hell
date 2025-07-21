#pragma once

#include "../packet_template/frame.hpp"

class PlayerEntity {
public:
    PlayerEntity(uint8_t id, uint8_t name);

    void set_id();
    void set_name();
    void set_state(PlayerState state);
    void set__attack_pattern(uint8_t pattern);

    PlayerSnapshot snapshot();

private:
    PlayerSnapshot m_snapshot;
};