#include "renderable_resolver.hpp"
#include "../config_constants.hpp"
#include "../sprite/sprite_loader.hpp"
#include "../transformer/transformer.hpp"
#include "../game_server/game_logic_constants.hpp"

bool RenderableResolver::load_sprites(sol::state& lua, const std::string& registry_path) {
    // Clear sprite cache
    m_sprite_cache.clear();

    // Clear resource cache
    m_mesh_factory.clear_cache();
    m_shader_factory.clear_cache();
    m_texture_factory.clear_cache();

    // Load sprites from lua file
    auto sprites_opt = load_sprite_registry(lua, registry_path);

    if (!sprites_opt.has_value())
    {
        return false;
    }

    for (const auto& sprite : sprites_opt.value())
    {
        const auto mesh_name = std::string(assets_constants::MESH_DIR) + "/" + sprite.dimensions.mesh;

        // Load mesh
        m_mesh_factory.load_mesh(lua, mesh_name);

        // Load shader
        for (const auto& anim : sprite.animations)
        {
            const auto shader_name = std::string(assets_constants::SHADER_DIR) + "/" + anim.second.shader;
            m_shader_factory.load_shader(lua, shader_name);
        }

        const auto texture_name = std::string(assets_constants::TEXTURE_DIR) + "/" + sprite.texture_atlas.image;

        // Load texture
        m_texture_factory.load_texture(texture_name);

        // Create a tag for the sprite
        auto tag = SpriteTag {
            sprite.type,
            sprite.name,
            0
        };

        // Register the sprite
        m_sprite_cache[tag] = sprite;
    }

    return true;
}

std::vector<RenderableInstance> RenderableResolver::resolve(const FrameSnapshot& frame) {
    const auto sprite_count = 1 + // stage
            frame.player_count +
            frame.enemy_count  +
            frame.boss_count   +
            frame.bullet_count +
            frame.item_count;

    auto renderable_instances = std::vector<RenderableInstance>{};
    renderable_instances.reserve(sprite_count);

    // Stage
    {
        auto stage = frame.stage;
        auto instance_opt = make_instance(stage);

        if (instance_opt.has_value())
            renderable_instances.push_back(instance_opt.value());
    }

    // Players
    for (const auto& player : frame.player_vector)
    {
        auto instance_opt = make_instance(player);

        if (instance_opt.has_value())
            renderable_instances.push_back(instance_opt.value());
    }

    // Enemies
    for (const auto& enemy : frame.enemy_vector)
    {
        auto instance_opt = make_instance(enemy);

        if (instance_opt.has_value())
            renderable_instances.push_back(instance_opt.value());
    }

    // Bosses
    for (const auto& boss : frame.boss_vector)
    {
        auto instance_opt = make_instance(boss);

        if (instance_opt.has_value())
            renderable_instances.push_back(instance_opt.value());
    }

    // Bullets
    for (const auto& bullet : frame.bullet_vector)
    {
        auto instance_opt = make_instance(bullet);

        if (instance_opt.has_value())
            renderable_instances.push_back(instance_opt.value());  
    }

    // Items
    for (const auto& item : frame.item_vector)
    {
         auto instance_opt = make_instance(item);

        if (instance_opt.has_value())
            renderable_instances.push_back(instance_opt.value());      
    }

    return renderable_instances;
}

std::optional<RenderableInstance> RenderableResolver::make_instance(const StageSnapshot& stage) {
    return std::nullopt;    // Implement soon
}

std::optional<RenderableInstance> RenderableResolver::make_instance(const PlayerSnapshot& player) {
    auto sprite_pair = m_sprite_cache.find(SpriteTag{
        SpriteType::Player,
        player.name,
        player.id
    });

    if (sprite_pair == m_sprite_cache.end())
    {
        std::cerr << "[RenderableResolver] ERROR: Sprite not found: " << player.name << "\n";

        return std::nullopt;
    }

    auto sprite = sprite_pair->second;

    /*
        Resolve resource names
    */
   
    // Resolve mesh
    const auto mesh_name = std::string(assets_constants::MESH_DIR) + "/" + sprite.dimensions.mesh;

    // Resolve shader
    const auto shader_key = "idle";

    auto shader_pair = sprite.animations.find(shader_key);
    if (shader_pair == sprite.animations.end())
    {
        std::cerr << "[RenderableResolver] ERROR: Sprite { name: '"
                  << static_cast<int>(sprite.name)
                  << "', "
                  << "type: '"
                  << static_cast<int>(sprite.type)
                  << "' } doesn't have a field: " << shader_key << "\n"; 

        return std::nullopt;
    }

    const auto shader_name = std::string(assets_constants::SHADER_DIR) + "/" + shader_pair->second.shader;

    // Resolve texture
    const auto texture_name = std::string(assets_constants::TEXTURE_DIR) + "/" + sprite.texture_atlas.image;

    /*
        Retrieve resource cache
    */
    const auto mesh     = m_mesh_factory.get_mesh(mesh_name);
    const auto shader   = m_shader_factory.get_shader(shader_name);
    const auto texture  = m_texture_factory.get_texture(texture_name);

    if (mesh == nullptr)
    {
        std::cerr << "[RenderableResolver] ERROR: Failed to resolve resource: " << mesh_name << "\n"; 

        return std::nullopt;
    }

    if (shader == nullptr)
    {
        std::cerr << "[RenderableResolver] ERROR: Failed to resolve shader: " << shader_name << "\n"; 

        return std::nullopt;
    }

    if (texture == nullptr)
    {
        std::cerr << "[RenderableResolver] ERROR: Failed to resolve texture: " << texture_name << "\n"; 

        return std::nullopt;
    }

    auto resource = RenderableResource{
        mesh,
        shader,
        texture
    }; 

    /*
        Create uniforms
    */

    // Calculate the player scale based on the view port size (px)
    const auto player_scale_x = sprite.dimensions.width / game_logic_constants::GAME_WIDTH * sprite.dimensions.scale;
    const auto player_scale_y = sprite.dimensions.height / game_logic_constants::GAME_HEIGHT * sprite.dimensions.scale;

    // Create model matrix
    // auto model = make_model_matrix(
    //     player.pos.x,
    //     player.pos.y,
    //     player.angle,
    //     player_scale_x,
    //     player_scale_y
    // );

    auto offset = glm::vec2(player.pos.x, player.pos.y);

    // Create time
    /*
        To-Do: Implement an AnimationTracker like this
        time = anim_tracker.get_anim time(name, id);
    */
    auto time = 0.0f;

    auto uniforms = std::unordered_map<std::string, UniformValue>{
        {"offset", offset},
        {"time", time}        
    };

    return RenderableInstance{
        std::make_shared<RenderableResource>(resource),
        uniforms
    };
}

std::optional<RenderableInstance> RenderableResolver::make_instance(const EnemySnapshot& enemy) {
    return std::nullopt;    // Implement soon
}

std::optional<RenderableInstance> RenderableResolver::make_instance(const BossSnapshot& stage) {
    return std::nullopt;    // Implement soon
}

std::optional<RenderableInstance> RenderableResolver::make_instance(const BulletSnapshot& bullet) {
    return std::nullopt;    // Implement soon
}

std::optional<RenderableInstance> RenderableResolver::make_instance(const ItemSnapshot& item) {
    return std::nullopt;    // Implement soon
}