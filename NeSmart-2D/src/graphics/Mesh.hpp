#pragma once

#include <vector>
#include <GL/glew.h>

namespace nesmart::graphics
{
    // A shape stored on the GPU: one VAO and one VBO holding interleaved float vertices.
    // The data is uploaded once in the constructor and never changes. Where and how the shape appears on
    // screen is decided at draw time by shader uniforms and textures, not by the mesh.
    // Throws std::runtime_error if the vertex data does not match the attribute layout.
    class Mesh
    {
    public:
        // attributeSizes lists the float count of each vertex attribute, in shader location order.
        // Example: {2, 2} is a position (x, y) at location 0 followed by a texture coordinate (s, t) at location 1.
        // mode is the GL primitive type the vertices are ordered for, for example GL_TRIANGLES,
        // GL_TRIANGLE_STRIP or GL_LINE_LOOP.
        Mesh(const std::vector<float>& vertices, const std::vector<int>& attributeSizes, GLenum mode);
        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        void draw() const;

    private:
        GLuint m_vao = 0;
        GLuint m_vbo = 0;
        GLsizei m_vertexCount = 0;
        GLenum m_mode = GL_TRIANGLES;
    };

} // namespace nesmart::graphics
