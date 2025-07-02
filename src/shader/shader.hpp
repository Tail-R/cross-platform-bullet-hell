#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    ~Shader();

    // Delete copy constructor and copy assignment operator
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // // Delete move constructor and move assignment operator
    // Shader(Shader&&) = delete;
    // Shader& operator=(Shader&&) = delete;

    void load_from_file(
        std::string_view vertex_shader_path,
        std::string_view fragment_shader_path
    );

    void load_default_shader();

    void use() const;
    GLuint id() const;

    // Uniform setters
    void set_bool(std::string_view name, bool value) const;
    void set_int(std::string_view name, int value) const;
    void set_float(std::string_view name, float value) const;
    void set_vec2(std::string_view name, const glm::vec2& vec) const;
    void set_vec3(std::string_view name, const glm::vec3& vec) const;
    void set_vec4(std::string_view name, const glm::vec4& vec) const;
    void set_mat4(std::string_view name, const glm::mat4& mat) const;

private:
    std::optional<std::string> try_load_shader_source(std::string_view shader_path) const;
    std::optional<GLuint> try_compile_shader(GLenum shader_type, std::string_view shader_code) const;

    GLuint m_program_id;

    void delete_program();
};