game_vars = require("game_vars")

return {
    type = game_vars.types.player,
    name = game_vars.player_names.default,

    dimensions = {
        mesh    = "rect_2d.lua",
        width   = 48,
        height  = 64
    },

    texture_atlas = {
        image = "zunmon_3005.png",

        regions = {
            normal = {
                u0 = 0.0,
                v0 = 0.0,
                u1 = 1.0,
                v1 = 1.0
            }
        }
    },

    animations = {
        idle = {
            shader = "green_aura.lua",
            region = "normal",
            duration = 0.0,
            loop = false
        },
        dead = {
            shader = "transparent.lua",
            region = "normal",
            duration = 0.0,
            loop = false
        }
    }
}
