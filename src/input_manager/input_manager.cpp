#include <utility> // std::pair
#include <array>
#include "input_manager.hpp"
#include "default_keymapping.hpp"

namespace {
    InputDirection resolve_direction(bool up, bool down, bool left, bool right) {
        if (up && right)    { return InputDirection::UpRight;   }
        if (up && left)     { return InputDirection::UpLeft;    }
        if (down && right)  { return InputDirection::DownRight; }
        if (down && left)   { return InputDirection::DownLeft;  }
        if (up)             { return InputDirection::Up;        }
        if (down)           { return InputDirection::Down;      }
        if (left)           { return InputDirection::Left;      }
        if (right)          { return InputDirection::Right;     }

        return InputDirection::Stop;
    }

    // Mapping between SDL Scancodes and directional Arrow enums
    const std::array arrow_pairs {
        std::pair{KEY_UP,    Arrow::Up      },
        std::pair{KEY_RIGHT, Arrow::Right   },
        std::pair{KEY_DOWN,  Arrow::Down    },
        std::pair{KEY_LEFT,  Arrow::Left    }
    };
}

InputManager::InputManager() {
    clear_input_snapshot();
}

InputManager::~InputManager() = default;

void InputManager::collect_input_events() {
    // Reset all input states to default before collecting new frame input
    clear_input_snapshot();

    // The current key state managed by SDL2
    const auto keystate = SDL_GetKeyboardState(nullptr);

    // A lambda to inspect the current key state
    auto is_down = [&](SDL_Scancode key) -> bool {
        return keystate[key] != 0;
    };

    auto any_arrow_held = false;

    // Update held arrows
    for (const auto& [key, arrow] : arrow_pairs)
    {
        if (is_down(key))
        {
            uint32_t bit = static_cast<uint32_t>(arrow);

            m_input_snapshot.menu.arrows.held.set(bit);
            m_input_snapshot.game.arrows.held.set(bit);

            any_arrow_held = true;
        }
    }

    if (any_arrow_held)
    {
        m_input_snapshot.menu.held.set(static_cast<uint32_t>(MenuAction::Navigate));
        m_input_snapshot.game.held.set(static_cast<uint32_t>(GameAction::Move));
    }

    // Update held menu action
    if (is_down(KEY_CONFIRM))   { m_input_snapshot.menu.held.set(static_cast<uint32_t>(MenuAction::Confirm)); }
    if (is_down(KEY_CANCEL))    { m_input_snapshot.menu.held.set(static_cast<uint32_t>(MenuAction::Cancel));  }

    // Update held game action
    if (is_down(KEY_SHOOT))     { m_input_snapshot.game.held.set(static_cast<uint32_t>(GameAction::Shoot));       }
    if (is_down(KEY_SPELL))     { m_input_snapshot.game.held.set(static_cast<uint32_t>(GameAction::Spell));       }
    if (is_down(KEY_FOCUS))     { m_input_snapshot.game.held.set(static_cast<uint32_t>(GameAction::Focus));       }
    if (is_down(KEY_OPEN_MENU)) { m_input_snapshot.game.held.set(static_cast<uint32_t>(GameAction::OpenMenu));    }

    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        // Handle quit events, such as when the window is closed
        if (e.type == SDL_QUIT)
        {
            m_input_snapshot.quit_request = true;
            
            continue;
        }

        // Handle keyboard key pressed/released events and update corresponding input state
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
            const auto is_pressed  = e.type == SDL_KEYDOWN && !e.key.repeat;
            const auto is_released = e.type == SDL_KEYUP;

            const auto code = e.key.keysym.scancode;

            // Update arrow pressed/released
            for (const auto& [key, arrow] : arrow_pairs)
            {
                if (code == key)
                {
                    uint32_t arrow_bit = static_cast<uint32_t>(arrow);

                    if (is_pressed)
                    {
                        m_input_snapshot.menu.arrows.pressed.set(arrow_bit);
                        m_input_snapshot.game.arrows.pressed.set(arrow_bit);
                        m_input_snapshot.menu.pressed.set(static_cast<uint32_t>(MenuAction::Navigate));
                        m_input_snapshot.game.pressed.set(static_cast<uint32_t>(GameAction::Move));
                    }
                    else if (is_released)
                    {
                        m_input_snapshot.menu.arrows.released.set(arrow_bit);
                        m_input_snapshot.game.arrows.released.set(arrow_bit);
                        m_input_snapshot.menu.released.set(static_cast<uint32_t>(MenuAction::Navigate));
                        m_input_snapshot.game.released.set(static_cast<uint32_t>(GameAction::Move));
                    }
                }
            }

            // Update menu pressed/released
            auto update_menu = [&](SDL_Scancode key, MenuAction action) {
                if (code == key)
                {
                    uint32_t action_bit = static_cast<uint32_t>(action);

                    if (is_pressed)
                    {
                        m_input_snapshot.menu.pressed.set(action_bit);
                    }
                    else if (is_released)
                    {
                        m_input_snapshot.menu.released.set(action_bit);
                    }
                }
            };

            update_menu(KEY_CONFIRM,    MenuAction::Confirm);
            update_menu(KEY_CANCEL,     MenuAction::Cancel);            

            // Update game pressed/released
            auto update_game = [&](SDL_Scancode key, GameAction action) {
                if (code == key)
                {
                    uint32_t action_bit = static_cast<uint32_t>(action);

                    if (is_pressed)
                    {
                        m_input_snapshot.game.pressed.set(action_bit);
                    }
                    else if (is_released)
                    {
                        m_input_snapshot.game.released.set(action_bit);
                    }
                }
            };

            update_game(KEY_SHOOT,      GameAction::Shoot);
            update_game(KEY_SPELL,      GameAction::Spell);
            update_game(KEY_FOCUS,      GameAction::Focus);
            update_game(KEY_OPEN_MENU,  GameAction::OpenMenu);            
        }
    }
}

const MenuInput& InputManager::get_menu_input() const {
    return m_input_snapshot.menu;
}

const GameInput& InputManager::get_game_input() const {
    return m_input_snapshot.game;
}

const InputSnapshot& InputManager::get_input_snapshot() const {
    return m_input_snapshot;
}

bool InputManager::get_quit_request() const {
    return m_input_snapshot.quit_request;
}

void InputManager::clear_input_snapshot() {
    m_input_snapshot = {};
}