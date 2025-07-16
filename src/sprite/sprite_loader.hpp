#pragma once

#include <vector>
#include <optional>
#include <sol/sol.hpp>
#include "sprite.hpp"

using Sprites = std::vector<Sprite>;

std::optional<Sprites> load_sprite_registry(sol::state &lua, const std::string &registry_path);