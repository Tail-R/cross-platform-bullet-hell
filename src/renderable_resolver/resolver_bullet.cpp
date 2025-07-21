#include "renderable_resolver.hpp"
#include "../config_constants.hpp"
#include "../transformer/transformer.hpp"

std::optional<RenderableInstance> RenderableResolver::resolve_instance(const BulletSnapshot& bullet) {
    using namespace renderable_resolver;

    const auto tag = SpriteTag{
        SpriteType::Bullet,
        static_cast<SpriteName>(bullet.name),
        bullet.id
    };

    /*
        Resolve sprite
    */
    auto sprite_opt = find_sprite(tag);

    if (!sprite_opt.has_value())
    {
        std::cerr << "[RenderableResolver] ERROR: Bullet: " << static_cast<int>(bullet.name)
                  << " not found in sprite cache" << "\n";

        return std::nullopt;
    }

    auto sprite = sprite_opt.value();

    /*
        Resolve animation key
    */
    const auto anim_key = "idle";

    /*
        Resolve resource
    */
    auto resource_opt = resolve_resource(sprite, anim_key);

    if (!resource_opt.has_value())
    {
        std::cerr << "[RenderableResolver] ERROR: Failed to retrieve resource for bullet "
                  << static_cast<int>(sprite.name) << "\n";

        return std::nullopt;
    }

    auto resource = std::make_shared<RenderableResource>(resource_opt.value());

    /*
        Create uniforms
    */
    // Create model matrix
    auto model = make_model_matrix(
        bullet.pos.x,
        bullet.pos.y,
        bullet.angle,
        sprite.dimensions.width,
        sprite.dimensions.height
    );

    // Create time
    /*
        To-Do: Implement an AnimationTracker like this
        time = anim_tracker.get_anim time(name, id);
    */
    g_bullet_time += 0.001f;

    // Create texture region
    auto anim_iter = sprite.animations.find(anim_key);
    auto region_key = anim_iter->second.region;
    auto region_iter = sprite.texture_atlas.regions.find(region_key);
    auto region = region_iter->second;

    auto tex_region = glm::vec4{
        region.u0,
        region.v0,
        region.u1,
        region.v1
    };

    auto uniforms = std::unordered_map<std::string, UniformValue>{
        {"model", model},
        {"projection", g_projection},
        {"tex_region", tex_region},
        {"time", g_bullet_time}
    };

    return RenderableInstance{
        resource,
        uniforms
    };
}