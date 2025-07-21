#include <iostream>
#include "mesh.hpp"

Mesh::Mesh(
    const std::vector<GLfloat>& vertices,
    const std::vector<GLuint>& indices,
    const std::vector<VertexAttribute>& attributes)
    : m_index_count(static_cast<GLsizei>(indices.size()))
    , m_vertex_size(0)
    , m_texcoord_offset(0)
    , m_texcoord_stride(0)
    , m_texcoord_vertex_count(0) {

    for (auto attr : attributes)
    {
        m_vertex_size += attr.size;
    }

    glGenVertexArrays(1, &m_vao);

    if (m_vao == 0)
    {
        std::cerr << "[Mesh] Failed to generate VAO!" << "\n";
    }

    glGenBuffers(1, &m_vbo);

    if (m_vbo == 0)
    {
        std::cerr << "[Mesh] Failed to generate VBO!" << "\n";
    }

    glGenBuffers(1, &m_ebo);

    if (m_ebo == 0)
    {
        std::cerr << "[Mesh] Failed to generate EBO!" << "\n";
    }

    glBindVertexArray(m_vao);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Attributes
    GLsizei offset = 0;
    GLsizei stride = m_vertex_size * sizeof(GLfloat);
    bool found_texcoord = false;

    for (GLsizei i = 0; i < static_cast<GLsizei>(attributes.size()); i++)
    {
        glEnableVertexAttribArray(i);

        glVertexAttribPointer(
            i,                                  // Index of the attribute
            attributes[i].size,                 // Size
            GL_FLOAT,                           // Type
            GL_FALSE,                           // Normalized
            stride,                             // Stride
            reinterpret_cast<void*>(static_cast<uintptr_t>(offset * sizeof(GLfloat)))   // Offset to the start
        );

        if (attributes[i].name == "texcoord" && !found_texcoord)
        {
            m_texcoord_offset = offset;
            m_texcoord_stride = attributes[i].size;
            m_texcoord_vertex_count = static_cast<GLsizei>(vertices.size()) / m_vertex_size;

            found_texcoord = true;
        }

        offset += attributes[i].size;
    }

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept {
    swap_attributes(&other);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other)
    {
        cleanup();
        swap_attributes(&other);
    }

    return *this;
}

void Mesh::bind() const {
    glBindVertexArray(m_vao);
}

void Mesh::unbind() const {
    glBindVertexArray(0);
}

void Mesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::set_texcoord(const std::vector<GLfloat>& texcoord) {
    if (m_texcoord_offset < 0)
    {
        std::cerr << "[Mesh] ERROR: Invalid texcoord offset" << "\n";

        return;
    }

    if (static_cast<GLsizei>(texcoord.size()) != m_texcoord_vertex_count * m_texcoord_stride)
    {
        std::cerr << "[Mesh] ERROR: Invalid texcoord data size" << "\n";

        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // If the texture coordinates are at the edge of a vertex, send all UV vertices at the same time.
    // if (m_texcoord_offset == m_vertex_size - m_texcoord_stride) {
    //     glBufferSubData(
    //         GL_ARRAY_BUFFER,
    //         m_texcoord_offset * sizeof(GLfloat),
    //         texcoord.size() * sizeof(GLfloat),
    //         texcoord.data()
    //     );
    // }
    // else
    {
        for (GLsizei i = 0; i < m_texcoord_vertex_count; i++)
        {
            GLsizeiptr offset = (i * m_vertex_size + m_texcoord_offset) * sizeof(GLfloat);

            glBufferSubData(
                GL_ARRAY_BUFFER,
                offset,
                sizeof(GLfloat) * m_texcoord_stride,
                texcoord.data() + i * m_texcoord_stride
            );
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::cleanup() {
    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
    }
    
    if (m_ebo != 0)
    {
        glDeleteBuffers(1, &m_ebo);
    }
    
    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
}

void Mesh::swap_attributes(Mesh* other) {
    this->m_vao                     = other->m_vao;
    this->m_vbo                     = other->m_vbo;
    this->m_ebo                     = other->m_ebo;
    this->m_index_count             = other->m_index_count;
    this->m_vertex_size             = other->m_vertex_size;
    this->m_texcoord_offset         = other->m_texcoord_offset;
    this->m_texcoord_stride         = other->m_texcoord_stride;
    this->m_texcoord_vertex_count   = other->m_texcoord_vertex_count;

    other->m_vao                    = 0;
    other->m_vbo                    = 0;
    other->m_ebo                    = 0;
    other->m_index_count            = 0;
    other->m_vertex_size            = 0;
    other->m_texcoord_offset        = 0;
    other->m_texcoord_stride        = 0;
    other->m_texcoord_vertex_count  = 0;
}