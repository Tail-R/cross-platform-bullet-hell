#include <iostream>
#include "mesh.hpp"

Mesh::Mesh(
    const std::vector<GLfloat>& vertices,
    const std::vector<GLuint>& indices,
    const std::vector<VertexAttribute>& attributes)
    : m_index_count(static_cast<GLsizei>(indices.size())) {
    
    GLsizei vertex_size = 0;

    for (auto attr : attributes)
    {
        vertex_size += attr.size;
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
    GLsizei stride = vertex_size * sizeof(GLfloat);

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
    this->m_vao         = other->m_vao;
    this->m_vbo         = other->m_vbo;
    this->m_ebo         = other->m_ebo;
    this->m_index_count = other->m_index_count;

    other->m_vao         = 0;
    other->m_vbo         = 0;
    other->m_ebo         = 0;
    other->m_index_count = 0;
}