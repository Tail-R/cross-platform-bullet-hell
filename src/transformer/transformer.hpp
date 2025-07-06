#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class SpriteTransformer {
public:
    uint32_t id;

    float x = 0.0f;
    float y = 0.0f;
    float angle = 0.0f;
    float scale_x = 1.0f;
    float scale_y = 1.0f;

    glm::mat4 get_model_matrix() const;
};