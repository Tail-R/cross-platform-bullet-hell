#pragma once

#include <cstdint>
#include <cstddef>
#include <bitset>

enum class InputDevice : uint8_t {
    None,
    Keyboard,
    Gamepad,
    Mouse,
    Count
};

/*
    This class abstracts the current input direction.
*/
enum class InputDirection : uint32_t {
    Stop,
    Up,
    Right,
    Down,
    Left,
    UpRight,
    DownRight,
    DownLeft,
    UpLeft,
    Count
};

enum class MenuAction : uint8_t {
    None,
    Navigate,
    Confirm,
    Cancel,
    Count
};

enum class GameAction : uint8_t {
    None,
    Move,
    Shoot,
    Spell,
    Focus,
    OpenMenu,
    Count
};

struct MenuInput {
    std::bitset<static_cast<size_t>(MenuAction::Count)> held;
    std::bitset<static_cast<size_t>(MenuAction::Count)> pressed;
    std::bitset<static_cast<size_t>(MenuAction::Count)> released;

    InputDirection direction;
};

struct GameInput {
    std::bitset<static_cast<size_t>(GameAction::Count)> held;
    std::bitset<static_cast<size_t>(GameAction::Count)> pressed;
    std::bitset<static_cast<size_t>(GameAction::Count)> released;

    InputDirection direction;
};

struct InputSnapshot {
    MenuInput menu;
    GameInput game;

    bool quit_request{false};
};