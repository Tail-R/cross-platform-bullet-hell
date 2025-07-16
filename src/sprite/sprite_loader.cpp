#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <unordered_map>
#include <sol/sol.hpp>
#include "sprite.hpp"

namespace {
    Sprite parse_sprite(sol::table sprite_tbl) {
        Sprite sprite{};

        // Type
        auto type_opt = sprite_tbl["type"];
        if (type_opt.valid())
        {
            sprite.type = static_cast<SpriteType>(type_opt.get<int>());
        }
        else
        {
            std::cerr << "[parse_sprite] WARNING: 'type' is missing." << "\n";
        }

        // Name
        sprite.name = sprite_tbl.get_or("name", 0);

        // Dimensions
        sol::optional<sol::table> dim_opt = sprite_tbl["dimensions"];

        if (!dim_opt.has_value())
        {
            std::cerr << "[parse_sprite] ERROR: Missing 'dimensions'." << "\n";

            return sprite;
        }

        sol::table dim = dim_opt.value();
        sprite.dimensions.mesh   = dim.get_or("mesh",   std::string{});
        sprite.dimensions.width  = dim.get_or("width",  0.0f);
        sprite.dimensions.height = dim.get_or("height", 0.0f);
        sprite.dimensions.scale  = dim.get_or("scale",  1.0f);

        // Texture atlas
        sol::optional<sol::table> atlas_opt = sprite_tbl["texture_atlas"];
        
        if (atlas_opt.has_value())
        {
            sol::table atlas = atlas_opt.value();

            // Image
            sprite.texture_atlas.image = atlas.get_or("image", std::string{});

            // Regions
            sol::optional<sol::table> regions_opt = atlas["regions"];

            if (regions_opt.has_value())
            {
                sol::table regions = regions_opt.value();

                for (auto &[key, value] : regions)
                {
                    if (!value.is<sol::table>())
                    {
                        std::cerr << "[parse_sprite] WARNING: Invalid region table. Skipped." << "\n";

                        continue;
                    }

                    std::string region_name = key.as<std::string>();
                    sol::table region_tbl = value.as<sol::table>();

                    SpriteTextureRegion region{};
                    region.u0 = region_tbl.get_or("u0", 0.0f);
                    region.v0 = region_tbl.get_or("v0", 0.0f);
                    region.u1 = region_tbl.get_or("u1", 0.0f);
                    region.v1 = region_tbl.get_or("v1", 0.0f);

                    sprite.texture_atlas.regions[region_name] = region;
                }
            }
        }

        // Animations
        sol::optional<sol::table> anims_opt = sprite_tbl["animations"];
        if (anims_opt.has_value())
        {
            sol::table anims = anims_opt.value();

            for (auto &[key, value] : anims)
            {
                if (!value.is<sol::table>())
                {
                    std::cerr << "[parse_sprite] WARNING: Invalid animation table. Skipped." << "\n";

                    continue;
                }

                std::string anim_name = key.as<std::string>();
                sol::table anim_tbl = value.as<sol::table>();

                SpriteAnimation anim{};
                anim.shader   = anim_tbl.get_or("shader",   std::string{});
                anim.region   = anim_tbl.get_or("region",   std::string{});
                anim.duration = anim_tbl.get_or("duration", 0.0f);
                anim.loop     = anim_tbl.get_or("loop",     false);

                sprite.animations[anim_name] = anim;
            }
        }

        return sprite;
    }
}

using Sprites = std::vector<Sprite>;

std::optional<Sprites> load_sprite_registry(sol::state &lua, const std::string &registry_path) {
    sol::protected_function_result result = lua.safe_script_file(registry_path, &sol::script_pass_on_error);

    if (!result.valid())
    {
        sol::error err = result;
        std::cerr << "[load_sprite_registry] ERROR: Failed to load '" << registry_path << "'\n";
        std::cerr << "Lua error: " << err.what() << "\n";

        return std::nullopt;
    }

    sol::object obj = result;

    if (!obj.is<sol::table>())
    {
        std::cerr << "[load_sprite_registry] ERROR: Lua file did not return a table.\n";

        return std::nullopt;
    }

    sol::table sprite_list = obj.as<sol::table>();
    Sprites sprites;

    for (auto &[category_key, category_value] : sprite_list)
    {
        std::string category = category_key.as<std::string>();

        if (!category_value.is<sol::table>())
        {
            std::cerr << "[load_sprite_registry] WARNING: Category '" << category << "' is not a table. Skipped." << "\n";

            continue;
        }

        sol::table sprite_array = category_value;

        for (std::size_t i = 1; i <= sprite_array.size(); i++)
        {
            if (!sprite_array[i].is<sol::table>())
            {
                std::cerr << "[load_sprite_registry] WARNING: Invalid sprite entry in category '" << category << "' at index " << i << ". Skipped." << "\n";

                continue;
            }

            sol::table sprite_tbl = sprite_array[i];

            try
            {
                sprites.push_back(parse_sprite(sprite_tbl));
            }
            catch (const std::exception& e)
            {
                std::cerr << "[load_sprite_registry] ERROR: Exception parsing sprite in '" << category << "'[" << i << "]: " << e.what() << "\n";
                
                continue;
            }
        }
    }

    return sprites;
}
