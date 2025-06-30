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
}

Shader::Shader()
    : m_program_id(0)
{}

Shader::~Shader() {
    delete_program();
}

void Shader::load_from_file(
    std::string_view vertex_shader_path,
    std::string_view fragment_shader_path
) {
    delete_program();

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
        std::cerr << "[Shader] Falling back to default vertex shader" << "\n";
        vertex_shader_opt = try_compile_shader(GL_VERTEX_SHADER, default_vertex_shader);
    }

    if (!fragment_shader_opt.has_value())
    {
        std::cerr << "[Shader] Falling back to default fragment shader" << "\n";
        fragment_shader_opt = try_compile_shader(GL_FRAGMENT_SHADER, default_fragment_shader);
    }

    if (!vertex_shader_opt || !fragment_shader_opt)
    {
        throw std::runtime_error("Error while compiling GLSL shader");
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
            std::cerr << "[Shader] " << link_status.info_log.value() << "\n";
        }

        throw std::runtime_error("[Shader] Error while linking GLSL shaders");
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::load_default_shader() {
    delete_program();

    auto vertex_shader_opt = try_compile_shader(GL_VERTEX_SHADER, default_vertex_shader);
    auto fragment_shader_opt = try_compile_shader(GL_FRAGMENT_SHADER, default_fragment_shader);

    if (!vertex_shader_opt || !fragment_shader_opt)
    {
        throw std::runtime_error("[Shader] Error while compiling GLSL shader");
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
            std::cerr << "[Shader] " << link_status.info_log.value() << "\n";
        }

        throw std::runtime_error("[Shader] Error while linking GLSL shaders");
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}

void Shader::use() const {
    glUseProgram(m_program_id);
}

GLuint Shader::id() const {
    return m_program_id;
}

void Shader::set_bool(std::string_view name, bool value) const {
    glUniform1i(
        glGetUniformLocation(m_program_id, name.data()),
        static_cast<GLboolean>(value)
    );
}

void Shader::set_int(std::string_view name, int value) const {
    glUniform1i(
        glGetUniformLocation(m_program_id, name.data()),
        static_cast<GLint>(value)
    );
}

void Shader::set_float(std::string_view name, float value) const {
    glUniform1f(
        glGetUniformLocation(m_program_id, name.data()),
        static_cast<GLfloat>(value)
    );
}

void Shader::set_vec2(std::string_view name, const glm::vec2& vec) const {
    glUniform2fv(
        glGetUniformLocation(m_program_id, name.data()),
        1,
        &vec[0]
    );
}

void Shader::set_vec3(std::string_view name, const glm::vec3& vec) const {
    glUniform3fv(
        glGetUniformLocation(m_program_id, name.data()),
        1,
        &vec[0]
    );
}

void Shader::set_vec4(std::string_view name, const glm::vec4& vec) const {
    glUniform4fv(
        glGetUniformLocation(m_program_id, name.data()),
        1,
        &vec[0]
    );
}

void Shader::set_mat4(std::string_view name, const glm::mat4& mat) const {
    glUniformMatrix4fv(
        glGetUniformLocation(m_program_id, name.data()),
        1,
        GL_FALSE,
        &mat[0][0]
    );
}

std::optional<std::string> Shader::try_load_shader_source(std::string_view shader_path) const {
    std::ifstream file(shader_path.data());
    std::stringstream ss;

    if (!file.is_open())
    {
        std::cerr << "[Shader] Failed to load shader file: " << shader_path << "\n";

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

        std::cerr << "[Shader] Failed to compile " << shader_type_str << " shader: ";

        if (compile_status.info_log.has_value())
        {
            std::cerr << "[Shader] " << compile_status.info_log.value();
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