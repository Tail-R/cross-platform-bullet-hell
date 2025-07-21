#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <sol/sol.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../renderer/renderer.hpp"
#include "../sprite/sprite.hpp"
#include "../sprite/sprite_tag.hpp"
#include "../assets_factory/mesh_factory.hpp"
#include "../assets_factory/shader_factory.hpp"
#include "../assets_factory/texture_factory.hpp"
#include "../packet_template/packet_template.hpp"
#include "../game_server/game_logic_constants.hpp"

namespace renderable_resolver {
    extern glm::mat4 g_projection;
    extern float g_stage_time;
    extern float g_player_time;
    extern float g_enemy_time;
    extern float g_boss_time;
    extern float g_bullet_time;
    extern float g_item_time;
}

// A resolver that takes a frame snapshot and resolves the required renderable instances
class RenderableResolver {
public:
    bool load_sprites(sol::state& lua, const std::string& registry_path);
    std::vector<RenderableInstance> resolve(const FrameSnapshot& frame);

private:
    std::optional<RenderableInstance> resolve_instance(const StageSnapshot& stage);
    std::optional<RenderableInstance> resolve_instance(const PlayerSnapshot& player);
    std::optional<RenderableInstance> resolve_instance(const EnemySnapshot& enemy);
    std::optional<RenderableInstance> resolve_instance(const BossSnapshot& boss);
    std::optional<RenderableInstance> resolve_instance(const BulletSnapshot& bullet);
    std::optional<RenderableInstance> resolve_instance(const ItemSnapshot& item);

    std::optional<Sprite> find_sprite(const SpriteTag& tag);
    
    std::optional<RenderableResource> resolve_resource(
        const Sprite& sprite,
        const std::string& anim_key
    );

    std::unordered_map<SpriteTag, Sprite> m_sprite_cache;

    MeshFactory     m_mesh_factory;
    ShaderFactory   m_shader_factory;
    TextureFactory  m_texture_factory;
};