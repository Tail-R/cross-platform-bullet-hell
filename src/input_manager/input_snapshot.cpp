#include "input_snapshot.hpp"

InputDirection get_direction_from_arrows(const ArrowState& arrows) {
    const auto up       = arrows.held.test(static_cast<size_t>(Arrow::Up));
    const auto right    = arrows.held.test(static_cast<size_t>(Arrow::Right));
    const auto down     = arrows.held.test(static_cast<size_t>(Arrow::Down));
    const auto left     = arrows.held.test(static_cast<size_t>(Arrow::Left));
 
    if (up && right)    { return InputDirection::UpRight;   }
    if (down && right)  { return InputDirection::DownRight; }
    if (down && left)   { return InputDirection::DownLeft;  }
    if (up && left)     { return InputDirection::UpLeft;    }
    if (up)             { return InputDirection::Up;        }
    if (right)          { return InputDirection::Right;     }
    if (down)           { return InputDirection::Down;      }
    if (left)           { return InputDirection::Left;      }

    return InputDirection::Stop;
}