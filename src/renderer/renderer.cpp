#include <iostream>
#include "renderer.hpp"

void Renderer::draw(const std::vector<RenderableInstance>& renderable_instances) {
    for (const auto& renderable : renderable_instances)
    {
        auto mesh = renderable.resource->mesh;
        auto shader = renderable.resource->shader;
        auto texture = renderable.resource->texture;

        if (mesh == nullptr || shader == nullptr || texture == nullptr)
        {
            std::cerr << "[Renderer] WARNING: Missing renderable resource (mesh/shader/texture), skipping" << "\n";

            continue;
        }

        // Bind current resources
        shader->use();
        mesh->bind();
        texture->bind();

        // Set unifroms
        for (const auto& [name, value] : renderable.uniforms)
        {
            shader->set_uniform(name, value);
        }

        mesh->draw();
    }
}