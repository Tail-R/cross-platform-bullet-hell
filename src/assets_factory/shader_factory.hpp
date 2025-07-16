#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <sol/sol.hpp>
#include "../shader/shader.hpp"

class ShaderFactory {
public:
    ShaderFactory();
    ~ShaderFactory();

    std::shared_ptr<Shader> get_shader(const std::string& shader_path);
    void load_shader(sol::state& lua, const std::string& shader_path);

    void clear_cache();

private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shader_cache;
};