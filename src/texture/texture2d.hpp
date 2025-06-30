#pragma once

#include <string_view>
#include <glad/glad.h>

struct Texture2DConfig {
    GLenum wrap_s           = GL_REPEAT;
    GLenum wrap_t           = GL_REPEAT;
    GLenum min_filter       = GL_LINEAR_MIPMAP_LINEAR;
    GLenum mag_filter       = GL_LINEAR;
    bool flip_vertically    = true;
};

class Texture2D {
public:
    Texture2D();
    ~Texture2D();

    // Delete copy constructor and copy assignment operator
    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    
    // Define move constructor and move assignment operator
    Texture2D(Texture2D&&) noexcept;
    Texture2D& operator=(Texture2D&&) noexcept;

    void load_from_file(
        std::string_view image_path,
        const Texture2DConfig& texture_config = Texture2DConfig()
    );

    // A function to fall back to the default texture if loading fails
    void load_default_texture();

    void bind(GLuint unit = 0) const;
    void unbind() const;

    int width() const;
    int height() const;
    GLuint id() const;

private:
    GLuint  m_texture_id;
    int     m_texture_width;
    int     m_texture_height;
};