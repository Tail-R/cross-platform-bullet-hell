#include <iostream>
#include <fstream>
#include <sstream>
#include "shader.hpp"

namespace {
    /*
        Default fallback shaders used on compile/link failure
        Uses OpenGL 3.3 for broad compatibility
    */
    constexpr std::string_view default_vertex_shader = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        void main() {
            gl_Position = vec4(aPos, 1.0);
        }
    )";

    constexpr std::string_view default_fragment_shader = R"(
        #version 330 core
        out vec4 FragColor;
        void main() {
            FragColor = vec4(1.0, 0.0, 1.0, 1.0);
        }
    )";

    constexpr size_t INFO_LOG_BUFF_SIZE = 512;

    struct GLStatusResult {
        bool success = false;
        std::optional<std::string> info_log = std::nullopt;
    };

    GLStatusResult check_compile_status(GLuint shader) {
        GLint success = 0;
        char info_log[INFO_LOG_BUFF_SIZE] = {};

        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            GLsizei len = 0;
            glGetShaderInfoLog(shader, INFO_LOG_BUFF_SIZE, &len, info_log);

            return {
                false,
                std::string(info_log, static_cast<size_t>(len))
            };
        }

        return {
            true,
            std::nullopt
        };
    }

    GLStatusResult check_link_status(GLuint program) {
        GLint success = 0;
        char info_log[INFO_LOG_BUFF_SIZE] = {};

        glGetProgramiv(program, GL_LINK_STATUS, &success);

        if (!success)
        {
            GLsizei len = 0;
            glGetProgramInfoLog(program, INFO_LOG_BUFF_SIZE, &len, info_log);

            return {
                false,
                std::string(info_log, static_cast<size_t>(len))
            };
        }

        return {
            true,
            std::nullopt
        };
    }

    UniformType glenum_to_uniform_type(GLenum type) {
        switch (type)
        {
            case GL_BOOL:       return UniformType::Bool;
            case GL_INT:        return UniformType::Int;
            case GL_FLOAT:      return UniformType::Float;
            case GL_FLOAT_VEC2: return UniformType::Vec2;
            case GL_FLOAT_VEC3: return UniformType::Vec3;
            case GL_FLOAT_VEC4: return UniformType::Vec4;
            case GL_FLOAT_MAT4: return UniformType::Mat4;
            default:            return UniformType::Unknown;
        }
    }
}

UniformType get_uniform_type(const UniformValue& value) {
    return std::visit([](auto&& arg) -> UniformType {
        using T = std::decay_t<decltype(arg)>;

        if      constexpr (std::is_same_v<T, bool>)         return UniformType::Bool;
        else if constexpr (std::is_same_v<T, int>)          return UniformType::Int;
        else if constexpr (std::is_same_v<T, float>)        return UniformType::Float;
        else if constexpr (std::is_same_v<T, glm::vec2>)    return UniformType::Vec2;
        else if constexpr (std::is_same_v<T, glm::vec3>)    return UniformType::Vec3;
        else if constexpr (std::is_same_v<T, glm::vec4>)    return UniformType::Vec4;
        else if constexpr (std::is_same_v<T, glm::mat4>)    return UniformType::Mat4;
        else                                                return UniformType::Unknown;
    }, value);
}

Shader::Shader()
    : m_program_id(0)
{
    m_uniform_location_cache.clear();
}

Shader::~Shader() {
    delete_program();
}

void Shader::load_from_file(
    std::string_view vertex_shader_path,
    std::string_view fragment_shader_path
) {
    delete_program();
    m_uniform_location_cache.clear();

    m_program_id = glCreateProgram();

    // Attempt to load shader source files (fallbacks used if failed)
    auto vertex_shader_code = try_load_shader_source(vertex_shader_path)
        .value_or(std::string(default_vertex_shader));

    auto fragment_shader_code = try_load_shader_source(fragment_shader_path)
        .value_or(std::string(default_fragment_shader));

    // Compiling shaders
    auto vertex_shader_opt = try_compile_shader(GL_VERTEX_SHADER, vertex_shader_code);
    auto fragment_shader_opt = try_compile_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

    // Fallback to default shader source if compilation fails
    if (!vertex_shader_opt.has_value())
    {
        std::cerr << "[Shader] WARNING: Falling back to default vertex shader" << "\n";
        vertex_shader_opt = try_compile_shader(GL_VERTEX_SHADER, default_vertex_shader);
    }

    if (!fragment_shader_opt.has_value())
    {
        std::cerr << "[Shader] WARNING: Falling back to default fragment shader" << "\n";
        fragment_shader_opt = try_compile_shader(GL_FRAGMENT_SHADER, default_fragment_shader);
    }

    if (!vertex_shader_opt || !fragment_shader_opt)
    {
        throw std::runtime_error("[Shader] CRITICAL: Error while compiling GLSL shader");
    }

    // Unwrapping
    auto vertex_shader = vertex_shader_opt.value();
    auto fragment_shader = fragment_shader_opt.value();

    // Linking shaders
    glAttachShader(m_program_id, vertex_shader);
    glAttachShader(m_program_id, fragment_shader);
    glLinkProgram(m_program_id);

    auto link_status = check_link_status(m_program_id);

    // Verify linking result and log any errors
    if (!link_status.success)
    {
        if (link_status.info_log.has_value())
        {
            std::cerr << "[Shader] ERROR: " << link_status.info_log.value() << "\n";
        }

        throw std::runtime_error("[Shader] CRITICAL: Error while linking GLSL shaders");
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Cache uniforms
    for (auto uni_ident : get_uniform_identities())
    {
        m_uniform_location_cache[uni_ident.name] = uni_ident.location;
    }
}

void Shader::load_default_shader() {
    delete_program();
    m_uniform_location_cache.clear();

    auto vertex_shader_opt = try_compile_shader(GL_VERTEX_SHADER, default_vertex_shader);
    auto fragment_shader_opt = try_compile_shader(GL_FRAGMENT_SHADER, default_fragment_shader);

    if (!vertex_shader_opt || !fragment_shader_opt)
    {
        throw std::runtime_error("[Shader] CRITICAL: Error while compiling GLSL shader");
    }

    // Unwrapping
    auto vertex_shader = vertex_shader_opt.value();
    auto fragment_shader = fragment_shader_opt.value();

    // Linking shaders
    m_program_id = glCreateProgram();
    glAttachShader(m_program_id, vertex_shader);
    glAttachShader(m_program_id, fragment_shader);
    glLinkProgram(m_program_id);

    auto link_status = check_link_status(m_program_id);

    // Verify linking result and log any errors
    if (!link_status.success)
    {
        if (link_status.info_log.has_value())
        {
            std::cerr << "[Shader] ERROR: " << link_status.info_log.value() << "\n";
        }

        throw std::runtime_error("[Shader] CRITICAL: Error while linking GLSL shaders");
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::use() const {
    glUseProgram(m_program_id);
}

void Shader::set_uniform(const std::string& name, bool value) const {
    auto location_opt = name_to_location(name);

    if (location_opt.has_value())
    {
        glUniform1i(location_opt.value(), static_cast<GLboolean>(value));

        return;
    }
    
    std::cerr << "[Shader] WARNING: The shader doesn't have an uniform: " << name << "\n";
}

void Shader::set_uniform(const std::string& name, int value) const {
    auto location_opt = name_to_location(name);

    if (location_opt.has_value())
    {
        glUniform1i(location_opt.value(), static_cast<GLint>(value));

        return;
    }
    
    std::cerr << "[Shader] WARNING: The shader doesn't have an uniform: " << name << "\n";
}

void Shader::set_uniform(const std::string& name, float value) const {
    auto location_opt = name_to_location(name);

    if (location_opt.has_value())
    {
        glUniform1f(location_opt.value(), static_cast<GLfloat>(value));

        return;
    }
    
    std::cerr << "[Shader] WARNING: The shader doesn't have an uniform: " << name << "\n";
}

void Shader::set_uniform(const std::string& name, const glm::vec2& vec) const {
    auto location_opt = name_to_location(name);

    if (location_opt.has_value())
    {
        glUniform2fv(location_opt.value(), 1, &vec[0]);

        return;
    }
    
    std::cerr << "[Shader] WARNING: The shader doesn't have an uniform: " << name << "\n";
}

void Shader::set_uniform(const std::string& name, const glm::vec3& vec) const {
    auto location_opt = name_to_location(name);

    if (location_opt.has_value())
    {
        glUniform3fv(location_opt.value(), 1, &vec[0]);

        return;
    }
    
    std::cerr << "[Shader] WARNING: The shader doesn't have an uniform: " << name << "\n";
}

void Shader::set_uniform(const std::string& name, const glm::vec4& vec) const {
    auto location_opt = name_to_location(name);

    if (location_opt.has_value())
    {
        glUniform4fv(location_opt.value(), 1, &vec[0]);

        return;
    }
    
    std::cerr << "[Shader] WARNING: The shader doesn't have an uniform: " << name << "\n";
}

void Shader::set_uniform(const std::string& name, const glm::mat4& mat) const {
    auto location_opt = name_to_location(name);

    if (location_opt.has_value())
    {
        glUniformMatrix4fv(location_opt.value(), 1, GL_FALSE, &mat[0][0]);

        return;
    }
    
    std::cerr << "[Shader] WARNING: The shader doesn't have an uniform: " << name << "\n";
}

std::vector<std::string> Shader::get_uniform_names() const {
    std::vector<std::string> uniform_names{};
    uniform_names.reserve(m_uniform_location_cache.size());

    // Collect uniform names from the cache
    for (auto uniform_pair : m_uniform_location_cache)
        uniform_names.push_back(uniform_pair.first);

    return uniform_names;
}

void Shader::set_uniform(const std::string& name, UniformValue value) const {
    const auto uniform_type = get_uniform_type(value);

    switch (uniform_type)
    {
        case UniformType::Bool:     { set_uniform(name, std::get<bool>(value));      break; }
        case UniformType::Int:      { set_uniform(name, std::get<int>(value));       break; }
        case UniformType::Float:    { set_uniform(name, std::get<float>(value));     break; }
        case UniformType::Vec2:     { set_uniform(name, std::get<glm::vec2>(value)); break; }
        case UniformType::Vec3:     { set_uniform(name, std::get<glm::vec3>(value)); break; }
        case UniformType::Vec4:     { set_uniform(name, std::get<glm::vec4>(value)); break; }
        case UniformType::Mat4:     { set_uniform(name, std::get<glm::mat4>(value)); break; }
        default:
        {
            std::cerr << "[Shader] WARNING: Invalid uniform type" << "\n";
        }
    }
}

std::optional<GLint> Shader::name_to_location(const std::string& name) const {
    auto it = m_uniform_location_cache.find(name);
    auto end = m_uniform_location_cache.end();

    return it != end ? std::make_optional(it->second) : std::nullopt;
}

std::vector<UniformIdentity> Shader::get_uniform_identities() {
    GLint uniform_count = 0;
    glGetProgramiv(m_program_id, GL_ACTIVE_UNIFORMS, &uniform_count);

    GLint max_name_length = 0;
    glGetProgramiv(m_program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);

    std::vector<UniformIdentity> shaders;
    std::vector<GLchar> name_buf(max_name_length);

    for (GLint i = 0; i < uniform_count; i++)
    {
        GLsizei length;
        GLint   size;
        GLenum  type;

        glGetActiveUniform(m_program_id, i, max_name_length, &length, &size, &type, name_buf.data());

        /*
            Get uniform name
        */
        std::string uniform_name(name_buf.data(), length);

        // Remove the last '[N]' of VecN and MatN 
        if (uniform_name.size() >= 3 && uniform_name.compare(uniform_name.size() - 3, 3, "[0]") == 0)
        {
            uniform_name = uniform_name.substr(0, uniform_name.size() - 3);
        }

        /*
            Get uniform type
        */
        auto uniform_type = glenum_to_uniform_type(type);

        if (uniform_type == UniformType::Unknown)
        {
            continue;
        }

        /*
            Get uniform location
        */
        GLint uniform_location = glGetUniformLocation(m_program_id, uniform_name.c_str());

        if (uniform_location == -1) {
            std::cerr << "[Shader] ERROR: Couldn't find the location of uniform: " << uniform_name << "\n";

            continue;
        }

        shaders.push_back(UniformIdentity{
            uniform_name,
            uniform_type,
            uniform_location
        });
    }

    return shaders;
}

std::optional<std::string> Shader::try_load_shader_source(std::string_view shader_path) const {
    std::ifstream file(shader_path.data());
    std::stringstream ss;

    if (!file.is_open())
    {
        std::cerr << "[Shader] ERROR: Failed to load shader file: " << shader_path << "\n";

        return std::nullopt;
    }

    ss << file.rdbuf();

    return ss.str();
}

std::optional<GLuint> Shader::try_compile_shader(GLenum shader_type, std::string_view shader_code) const {
    GLuint shader = glCreateShader(shader_type);
    const char* source = shader_code.data();

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    auto compile_status = check_compile_status(shader);

    // Check the compile status
    if (!compile_status.success)
    {
        std::string_view shader_type_str = 
            shader_type == GL_VERTEX_SHADER ? "vertex" : 
            shader_type == GL_FRAGMENT_SHADER ? "fragment" : "unknown";

        std::cerr << "[Shader] ERROR: Failed to compile " << shader_type_str << " shader: ";

        if (compile_status.info_log.has_value())
        {
            std::cerr << compile_status.info_log.value();
        }

        std::cerr << "\n";

        return std::nullopt;
    }

    return shader;
}

void Shader::delete_program() {
    if (m_program_id != 0)
    {
        glDeleteProgram(m_program_id);
        m_program_id = 0;
    }
}