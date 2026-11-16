#pragma once

#include <vector>
#include <GL/glew.h>

namespace nesmart::graphics
{
    class Mesh
    {
    public:
        Mesh(const std::vector<float>& vertices, const std::vector<int>& attributeSizes, GLenum mode);
        ~Mesh();

        static Mesh unitQuad();

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
