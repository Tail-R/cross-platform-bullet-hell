return {
    attributes = {
        { name = "position", size = 3 },
        { name = "texcoord", size = 2 }
    },
    vertices = {
        -- x, y, u, v
        -1.0,  1.0,  0.0,  0.0,  1.0,
        -1.0, -1.0,  0.0,  0.0,  0.0,
         1.0, -1.0,  0.0,  1.0,  0.0,
         1.0,  1.0,  0.0,  1.0,  1.0
    },
    indices = {
        0, 1, 2,
        2, 3, 0
    }
}