#include "graphics/Mesh.hpp"

#include <cstddef>
#include <numeric>
#include <stdexcept>

namespace nesmart::graphics
{
    Mesh::Mesh(const std::vector<float>& vertices, const std::vector<int>& attributeSizes, GLenum mode)
        : m_mode(mode)
    {
        // Validate before touching GL, so a failed constructor leaves nothing to release.
        const int floatsPerVertex = std::accumulate(attributeSizes.begin(), attributeSizes.end(), 0);
        if (floatsPerVertex <= 0 || vertices.empty() || vertices.size() % static_cast<std::size_t>(floatsPerVertex) != 0)
        {
            throw std::runtime_error("Mesh: vertex data does not match the attribute layout");
        }
        m_vertexCount = static_cast<GLsizei>(vertices.size() / static_cast<std::size_t>(floatsPerVertex));

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(),
                     GL_STATIC_DRAW);

        const GLsizei strideBytes = static_cast<GLsizei>(floatsPerVertex * sizeof(float));
        std::size_t offsetFloats = 0;
        for (std::size_t i = 0; i < attributeSizes.size(); ++i)
        {
            const GLuint location = static_cast<GLuint>(i);
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, attributeSizes[i], GL_FLOAT, GL_FALSE, strideBytes,
                                  reinterpret_cast<const void*>(offsetFloats * sizeof(float)));
            offsetFloats += static_cast<std::size_t>(attributeSizes[i]);
        }

        glBindVertexArray(0);
    }

    Mesh::~Mesh()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void Mesh::draw() const
    {
        glBindVertexArray(m_vao);
        glDrawArrays(m_mode, 0, m_vertexCount);
    }

} // namespace nesmart::graphics
