#include <iostream>
#include <sol/sol.hpp>
#include "shader_factory.hpp"
#include "../config_constants.hpp"

ShaderFactory::ShaderFactory() {
    clear_cache();
}

ShaderFactory::~ShaderFactory() = default;

std::shared_ptr<Shader> ShaderFactory::get_shader(const std::string& shader_path) {
    const auto it = m_shader_cache.find(shader_path);

    if (it != m_shader_cache.end())
    {
        return it->second;
    }

    std::cerr << "[ShaderFactory] Warning: Shader not preloaded: " << shader_path << "\n";
    
    // Fallback Shader created
    return std::make_shared<Shader>();
}

void ShaderFactory::load_shader(const std::string& shader_path) {
    // Resolve the actual shader location from the Lua file
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    sol::table shader_lua;

    try {
        shader_lua = lua.script_file(shader_path);
    } catch (const sol::error& e) {
        std::cerr << "[ShaderFactory] Lua error while loading " << shader_path
                  << ": " << e.what() << "\n";

        // Fallback Shader created
        m_shader_cache[shader_path] = std::make_shared<Shader>();

        return;
    }

    // Retrieve the values from Lua table
    const std::string vertex_rel = shader_lua["vertex"].get_or(std::string{});
    const std::string fragment_rel = shader_lua["fragment"].get_or(std::string{});

    if (vertex_rel.empty() || fragment_rel.empty())
    {
        std::cerr << "[ShaderFactory] Missing vertex or fragment field in: "
                  << shader_path << "\n";

        // Fallback Shader created
        m_shader_cache[shader_path] = std::make_shared<Shader>();

        return;
    }

    const auto vertex_shader_path = std::string(
        general_constants::ASSETS_DIR
    ) + vertex_rel;

    const auto fragment_shader_path = std::string(
        general_constants::ASSETS_DIR
    ) + fragment_rel;

    m_shader_cache[shader_path] = std::make_shared<Shader>(
        vertex_shader_path,
        fragment_shader_path
    );
}

void ShaderFactory::clear_cache() {
    m_shader_cache.clear();
}