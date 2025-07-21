#pragma once

#include <vector>
#include <glad/glad.h>

struct VertexAttribute {
    std::string name;
    size_t size;
};

class Mesh {
public:
    Mesh(
        const std::vector<GLfloat>& vertices,
        const std::vector<GLuint>& indices,
        const std::vector<VertexAttribute>& attributes
    );

    ~Mesh();

    // Delete copy constructor and copy assignment operator
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Define move constructor and move assignment operator
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void bind() const;
    void unbind() const;
    void draw() const;
    void set_texcoord(const std::vector<GLfloat>& texcoord);

private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;

    GLsizei m_index_count;
    GLsizei m_vertex_size;
    GLsizei m_texcoord_offset;
    GLsizei m_texcoord_stride;
    GLsizei m_texcoord_vertex_count;

    void cleanup();
    void swap_attributes(Mesh* other);
};