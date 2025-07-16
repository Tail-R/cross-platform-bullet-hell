#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../sprite/sprite_tag.hpp"

glm::mat4 make_model_matrix(float x, float y, float angle, float scale_x, float scale_y);

class SpriteTransformer {
public:
    float x = 0.0f;
    float y = 0.0f;
    float angle = 0.0f;
    float scale_x = 1.0f;
    float scale_y = 1.0f;

    glm::mat4 get_model_matrix() const;
};