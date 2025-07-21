local stage = require("stage")
local player = require("player")
local enemy = require("enemy")
local bullets = require("bullets")

return {
    stages  = { stage },
    players = { player },
    enemies = { enemy },
    bosses  = {
        unpack(bullets)
    },
    bullets = {},
    items   = {}
}
