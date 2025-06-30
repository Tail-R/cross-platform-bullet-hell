#include <iostream>
#include <sol/sol.hpp>
#include "shader_factory.hpp"
#include "../config_constants.hpp"

ShaderFactory::ShaderFactory() = default;
ShaderFactory::~ShaderFactory() = default;

std::shared_ptr<Shader> ShaderFactory::get_shader(const std::string& shader_path) {
    const auto it = m_shader_cache.find(shader_path);

    if (it != m_shader_cache.end())
    {
        return it->second;
    }

    std::cerr << "[ShaderFactory] Shader not preloaded: " << shader_path << "\n";
    
    // Fallback Shader created
    auto default_shader_ptr = std::make_shared<Shader>();
    default_shader_ptr->load_default_shader();

    m_shader_cache[shader_path] = default_shader_ptr;

    return default_shader_ptr;
}

void ShaderFactory::load_shader(const std::string& shader_path) {
    // Resolve the actual shader location from the Lua file
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    // Try to load Lua file
    sol::object load_result;

    try {
        load_result = lua.script_file(shader_path);
    } catch (const sol::error& e) {
        std::cerr << "[ShaderFactory] Lua error while loading " << shader_path
                  << ": " << e.what() << "\n";

        // Fallback Shader created
        auto default_shader_ptr = std::make_shared<Shader>();
        default_shader_ptr->load_default_shader();

        m_shader_cache[shader_path] = default_shader_ptr;

        return;
    }

    // Check the value type
    sol::table shader_lua;

    if (load_result.is<sol::table>())
    {
        shader_lua = load_result.as<sol::table>();
    }
    else
    {
        std::cerr << "[ShaderFactory] " << shader_path << " did not return a table" << "\n";

        // Fallback Shader created
        auto default_shader_ptr = std::make_shared<Shader>();
        default_shader_ptr->load_default_shader();

        m_shader_cache[shader_path] = default_shader_ptr;

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
        auto default_shader_ptr = std::make_shared<Shader>();
        default_shader_ptr->load_default_shader();

        m_shader_cache[shader_path] = default_shader_ptr;

        return;
    }

    const auto vertex_shader_path = std::string(
        assets_constants::SHADER_DIR
    ) + "/" + vertex_rel;

    const auto fragment_shader_path = std::string(
        assets_constants::SHADER_DIR
    ) + "/" + fragment_rel;

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