#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <sol/sol.hpp>
#include <glm/glm.hpp>

#include "../renderer/renderer.hpp"

#include "../sprite/sprite.hpp"
#include "../sprite/sprite_tag.hpp"

#include "../assets_factory/mesh_factory.hpp"
#include "../assets_factory/shader_factory.hpp"
#include "../assets_factory/texture_factory.hpp"
#include "../packet_template/packet_template.hpp"

// A resolver that takes a frame snapshot and resolves the required renderable instances
class RenderableResolver {
public:
    bool load_sprites(sol::state& lua, const std::string& registry_path);
    std::vector<RenderableInstance> resolve(const FrameSnapshot& frame);

private:
    std::optional<RenderableInstance> make_instance(const StageSnapshot& stage);
    std::optional<RenderableInstance> make_instance(const PlayerSnapshot& player);
    std::optional<RenderableInstance> make_instance(const EnemySnapshot& enemy);
    std::optional<RenderableInstance> make_instance(const BossSnapshot& stage);
    std::optional<RenderableInstance> make_instance(const BulletSnapshot& bullet);
    std::optional<RenderableInstance> make_instance(const ItemSnapshot& item);

    std::unordered_map<SpriteTag, Sprite> m_sprite_cache;

    MeshFactory     m_mesh_factory;
    ShaderFactory   m_shader_factory;
    TextureFactory  m_texture_factory;
};