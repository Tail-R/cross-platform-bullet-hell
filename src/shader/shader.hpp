#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>
#include <glad/glad.h>
#include <glm/glm.hpp>

enum class UniformType : uint8_t {
    Unknown,
    Bool,
    Int,
    Float,
    Vec2,
    Vec3,
    Vec4,
    Mat4
};

using UniformValue = std::variant<
    bool,
    int,
    float,
    glm::vec2,
    glm::vec3,
    glm::vec4,
    glm::mat4
>;

// Mainly used internally
struct UniformIdentity {
    std::string     name;
    UniformType     type;
    GLint           location;
};

UniformType get_uniform_type(const UniformValue& value);

class Shader {
public:
    Shader();
    ~Shader();

    // Delete copy constructor and copy assignment operator
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void load_from_file(
        std::string_view vertex_shader_path,
        std::string_view fragment_shader_path
    );

    void load_default_shader();

    void use() const;

    // Uniform setters
    void set_uniform(const std::string& name, bool value) const;
    void set_uniform(const std::string& name, int value) const;
    void set_uniform(const std::string& name, float value) const;
    void set_uniform(const std::string& name, const glm::vec2& vec) const;
    void set_uniform(const std::string& name, const glm::vec3& vec) const;
    void set_uniform(const std::string& name, const glm::vec4& vec) const;
    void set_uniform(const std::string& name, const glm::mat4& mat) const;
    void set_uniform(const std::string& name, UniformValue value) const;

    std::vector<std::string> get_uniform_names() const;

private:
    std::optional<std::string> try_load_shader_source(std::string_view shader_path) const;
    std::optional<GLuint> try_compile_shader(GLenum shader_type, std::string_view shader_code) const;

    GLuint m_program_id;

    // Retrieve uniform identities from the GPU
    std::vector<UniformIdentity> get_uniform_identities();

    // A cache that holds pairs of uniform names and locations
    std::unordered_map<std::string, GLint> m_uniform_location_cache;
    std::optional<GLint> name_to_location(const std::string& name) const;

    void delete_program();
};