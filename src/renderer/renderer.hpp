#pragma once

#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

#include "../mesh/mesh.hpp"
#include "../shader/shader.hpp"
#include "../texture/texture2d.hpp"

struct RenderableResource {
    std::shared_ptr<Mesh>       mesh;
    std::shared_ptr<Shader>     shader;
    std::shared_ptr<Texture2D>  texture;
};

struct RenderableInstance {
    std::shared_ptr<RenderableResource>             resource;
    std::unordered_map<std::string, UniformValue>   uniforms;
};

class Renderer {
public:
    void draw(const std::vector<RenderableInstance>& renderable_instances);
};