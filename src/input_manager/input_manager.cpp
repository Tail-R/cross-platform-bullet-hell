#include "input_manager.hpp"
#include "default_keymapping.hpp"

static InputDirection resolve_direction(bool up, bool down, bool left, bool right) {
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

InputManager::InputManager() {
    clear_input_snapshot();
}

InputManager::~InputManager() = default;

void InputManager::collect_input_events() {
    clear_input_snapshot();

    const auto keystate = SDL_GetKeyboardState(nullptr);

    // A Macro to inspect the keystate
    auto is_down = [&](SDL_Scancode key) -> bool {
        return keystate[key] != 0;
    };

    // Resolve direction
    bool up     = is_down(KEY_UP);
    bool down   = is_down(KEY_DOWN);
    bool left   = is_down(KEY_LEFT);
    bool right  = is_down(KEY_RIGHT);

    auto direction = resolve_direction(up, down, left, right);

    m_input_snapshot.menu.direction = direction;
    m_input_snapshot.game.direction = direction;

    if (direction != InputDirection::Stop)
    {
        m_input_snapshot.menu.held.set(static_cast<size_t>(MenuAction::Navigate));
        m_input_snapshot.game.held.set(static_cast<size_t>(GameAction::Move));
    }

    // Map held menu action
    if (is_down(KEY_CONFIRM))   { m_input_snapshot.menu.held.set(static_cast<size_t>(MenuAction::Confirm)); }
    if (is_down(KEY_CANCEL))    { m_input_snapshot.menu.held.set(static_cast<size_t>(MenuAction::Cancel));  }

    // Map held game action
    if (is_down(KEY_SHOOT))     { m_input_snapshot.game.held.set(static_cast<size_t>(GameAction::Shoot));       }
    if (is_down(KEY_SPELL))     { m_input_snapshot.game.held.set(static_cast<size_t>(GameAction::Spell));       }
    if (is_down(KEY_FOCUS))     { m_input_snapshot.game.held.set(static_cast<size_t>(GameAction::Focus));       }
    if (is_down(KEY_OPEN_MENU)) { m_input_snapshot.game.held.set(static_cast<size_t>(GameAction::OpenMenu));    }

    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            m_input_snapshot.quit_request = true;
            
            continue;
        }

        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
            const auto code = e.key.keysym.scancode;

            if (code == KEY_UP || code == KEY_DOWN || code == KEY_LEFT || code == KEY_RIGHT)
            {
                if (e.type == SDL_KEYDOWN && !e.key.repeat)
                {
                    m_input_snapshot.menu.pressed.set(static_cast<size_t>(MenuAction::Navigate));
                    m_input_snapshot.game.pressed.set(static_cast<size_t>(GameAction::Move));
                }
                else if (e.type == SDL_KEYUP)
                {
                    m_input_snapshot.menu.released.set(static_cast<size_t>(MenuAction::Navigate));
                    m_input_snapshot.game.released.set(static_cast<size_t>(GameAction::Move));
                }
            }

            // Map pressed action
            if (e.type == SDL_KEYDOWN && !e.key.repeat)
            {
                // Menu
                if (code == KEY_CONFIRM)    { m_input_snapshot.menu.pressed.set(static_cast<size_t>(MenuAction::Confirm));  }
                if (code == KEY_CANCEL)     { m_input_snapshot.menu.pressed.set(static_cast<size_t>(MenuAction::Cancel));   }

                // Game
                if (code == KEY_SHOOT)      { m_input_snapshot.game.pressed.set(static_cast<size_t>(GameAction::Shoot));    }
                if (code == KEY_SPELL)      { m_input_snapshot.game.pressed.set(static_cast<size_t>(GameAction::Spell));    }
                if (code == KEY_FOCUS)      { m_input_snapshot.game.pressed.set(static_cast<size_t>(GameAction::Focus));    }
                if (code == KEY_OPEN_MENU)  { m_input_snapshot.game.pressed.set(static_cast<size_t>(GameAction::OpenMenu)); }
            }

            // Map released action
            else if (e.type == SDL_KEYUP)
            {
                // Menu
                if (code == KEY_CONFIRM)    { m_input_snapshot.menu.released.set(static_cast<size_t>(MenuAction::Confirm));  }
                if (code == KEY_CANCEL)     { m_input_snapshot.menu.released.set(static_cast<size_t>(MenuAction::Cancel));   }

                // Game
                if (code == KEY_SHOOT)      { m_input_snapshot.game.released.set(static_cast<size_t>(GameAction::Shoot));    }
                if (code == KEY_SPELL)      { m_input_snapshot.game.released.set(static_cast<size_t>(GameAction::Spell));    }
                if (code == KEY_FOCUS)      { m_input_snapshot.game.released.set(static_cast<size_t>(GameAction::Focus));    }
                if (code == KEY_OPEN_MENU)  { m_input_snapshot.game.released.set(static_cast<size_t>(GameAction::OpenMenu)); }
            }
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