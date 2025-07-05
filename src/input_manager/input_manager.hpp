#pragma

#include "input_snapshot.hpp"

class InputManager {
public:
    InputManager();
    ~InputManager();

    void collect_input_events();

    const MenuInput& get_menu_input() const;
    const GameInput& get_game_input() const;
    const InputSnapshot& get_input_snapshot() const;

    bool get_quit_request() const;

private:
    struct InputSnapshot m_input_snapshot;

    void clear_input_snapshot();
};