game_vars = require("game_vars")

return {
    type = game_vars.types.stage,
    name = game_vars.stage_names.default,

    dimensions = {
        mesh    = "rect_2d.lua",
        width   = game_vars.width,
        height  = game_vars.height
    },

    texture_atlas = {
        image = "ibaraki_bamboo_forests.png",

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
            shader = "black_aura.lua",
            region = "normal",
            duration = 0.0,
            loop = false
        }
    }
}
