#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <glm/glm.hpp>

#include "sprite_tag.hpp"

#include "../assets_factory/mesh_factory.hpp"
#include "../assets_factory/shader_factory.hpp"
#include "../assets_factory/texture_factory.hpp"
#include "../packet_template/packet_template.hpp"

using Uniform = std::variant<
    bool,
    int,
    float,
    glm::vec2,
    glm::vec3,
    glm::vec4,
    glm::mat4
>;

struct RenderableResource {
    std::shared_ptr<Mesh>       mesh;
    std::shared_ptr<Shader>     shader;
    std::shared_ptr<Texture2D>  texture;
};

// It will be rendered by renderer
struct RenderableInstance {
    std::shared_ptr<RenderableResource> resource; 

    glm::mat4               model;
    std::vector<Uniform>    uniforms;
};

// A resolver that takes a frame snapshot and resolves the required renderable instances
class RenderableResolver {
public:
    void load_assets(const std::string& recipe);
    std::vector<RenderableInstance> resolve(const FrameSnapshot& frame);

private:
    std::unordered_map<SpriteTag, RenderableResource> resource_cache;
};