game_vars = require("game_vars")

return {
    type = game_vars.types.enemy,
    name = game_vars.enemy_names.default,

    dimensions = {
        mesh    = "rect_2d.lua",
        width   = 48,
        height  = 64
    },

    texture_atlas = {
        image = "zunmon_3002.png",

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
            shader = "red_aura.lua",
            region = "normal",
            duration = 0.0,
            loop = false
        }
    }
}
