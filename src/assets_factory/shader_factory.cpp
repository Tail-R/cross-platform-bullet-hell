#include <iostream>
#include "shader_factory.hpp"
#include "../config_constants.hpp"

namespace {
    std::optional<sol::table> load_lua_shader(sol::state& lua, const std::string& path) {
        try
        {
            sol::object result = lua.script_file(path);

            if (result.is<sol::table>())
            {
                return result.as<sol::table>();
            }
            else
            {
                std::cerr << "[ShaderFactory] ERROR: " << path << " did not return a table\n";
            }
        }
        catch (const sol::error& e)
        {
            std::cerr << "[ShaderFactory] ERROR: Lua error while loading " << path << ": " << e.what() << "\n";
        }

        return std::nullopt;
    }

    std::shared_ptr<Shader> create_default_shader() {
        auto default_shader = std::make_shared<Shader>();
        default_shader->load_default_shader();

        return default_shader;
    }
}

ShaderFactory::ShaderFactory() = default;
ShaderFactory::~ShaderFactory() = default;

std::shared_ptr<Shader> ShaderFactory::get_shader(const std::string& shader_path) {
    const auto it = m_shader_cache.find(shader_path);

    if (it != m_shader_cache.end())
    {
        return it->second;
    }

    std::cerr << "[ShaderFactory] WARNING: Shader not preloaded: " << shader_path << "\n";
    
    // Fallback Shader created
    auto fallback_shader = create_default_shader();
    m_shader_cache[shader_path] = fallback_shader;

    return fallback_shader;
}

void ShaderFactory::load_shader(sol::state& lua, const std::string& shader_path) {
    auto shader_table_opt = load_lua_shader(lua, shader_path);

    if (!shader_table_opt.has_value())
    {
        auto fallback_shader = create_default_shader();
        m_shader_cache[shader_path] = fallback_shader;

        return;
    }

    const sol::table& shader_lua = shader_table_opt.value();

    // Retrieve the values from Lua table
    const std::string vertex_name = shader_lua["vertex"].get_or(std::string{});
    const std::string fragment_name = shader_lua["fragment"].get_or(std::string{});

    if (vertex_name.empty() || fragment_name.empty())
    {
        std::cerr << "[ShaderFactory] WARNING: Missing vertex or fragment field in: "
                  << shader_path << "\n";

        // Fallback Shader created
        auto fallback_shader = create_default_shader();
        m_shader_cache[shader_path] = fallback_shader;

        return;
    }

    const auto vertex_shader_path = std::string(
        assets_constants::SHADER_DIR
    ) + "/" + vertex_name;

    const auto fragment_shader_path = std::string(
        assets_constants::SHADER_DIR
    ) + "/" + fragment_name;

    auto shader_ptr = std::make_shared<Shader>();

    shader_ptr->load_from_file(
        vertex_shader_path,
        fragment_shader_path
    );

    m_shader_cache[shader_path] = shader_ptr;
}

void ShaderFactory::clear_cache() {
    m_shader_cache.clear();
}