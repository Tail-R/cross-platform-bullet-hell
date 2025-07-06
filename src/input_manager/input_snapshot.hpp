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

 // The basic unit of action in this game
enum class InputDirection : uint16_t {
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

/*
    This class abstracts the current input direction.
    It's also represents gamepad input.
*/
enum class Arrow : uint8_t {
    Up,
    Right,
    Down,
    Left,
    Count
};

struct ArrowState {
    std::bitset<static_cast<size_t>(Arrow::Count)> held;
    std::bitset<static_cast<size_t>(Arrow::Count)> pressed;
    std::bitset<static_cast<size_t>(Arrow::Count)> released;
};

InputDirection get_direction_from_arrows(const ArrowState& arrows);

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

    ArrowState arrows;
};

struct GameInput {
    std::bitset<static_cast<size_t>(GameAction::Count)> held;
    std::bitset<static_cast<size_t>(GameAction::Count)> pressed;
    std::bitset<static_cast<size_t>(GameAction::Count)> released;

    ArrowState arrows;
};

struct InputSnapshot {
    MenuInput menu;
    GameInput game;

    bool quit_request{false};
};