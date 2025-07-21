local game_vars = require("game_vars")

local colors = {
    "red", "orange", "yellow", "green", "cyan", "blue", "magenta", "white"
}

local bullet_types = {
    {
        prefix = "normal_",
        size = { width = 20, height = 20 },
        image = "normal_bullet.png",
        name_table = game_vars.bullet_names.normal,
    },
    {
        prefix = "big_",
        size = { width = 60, height = 60 },
        image = "big_bullet.png",
        name_table = game_vars.bullet_names.big,
    },
    {
        prefix = "rice_",
        size = { width = 10, height = 10 },
        image = "rice_bullet.png",
        name_table = game_vars.bullet_names.rice,
    },
    {
        prefix = "wedge_",
        size = { width = 20, height = 20 },
        image = "wedge_bullet.png",
        name_table = game_vars.bullet_names.wedge,
    }
}

local bullets = {}

for _, bullet_type in ipairs(bullet_types) do
    for i, color in ipairs(colors) do
        local name_key = bullet_type.prefix .. color
        
        table.insert(bullets, {
            type = game_vars.types.bullet,
            name = bullet_type.name_table[name_key],

            dimensions = {
                mesh = "rect_2d.lua",
                width = bullet_type.size.width,
                height = bullet_type.size.height,
            },

            texture_atlas = {
                image = bullet_type.image,
                regions = {
                    [name_key] = {
                        u0 = 0.125 * (i - 1),
                        v0 = 0.0,
                        u1 = 0.125,
                        v1 = 1.0,
                    }
                }
            },

            animations = {
                idle = {
                    shader = "default.lua",
                    region = name_key,
                    duration = 0.0,
                    loop = false,
                }
            }
        })
    end
end

return bullets
