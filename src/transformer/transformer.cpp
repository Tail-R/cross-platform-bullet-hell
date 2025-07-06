#include "transformer.hpp"

glm::mat4 SpriteTransformer::get_model_matrix() const {
    auto model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(x, y, 0));
    model = glm::rotate(model, angle, glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(scale_x, scale_y, 1));

    return model;
}