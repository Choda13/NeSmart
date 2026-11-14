#pragma once
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

namespace nesmart::graphics
{

    class Shader
    {
        GLuint m_ID = 0;

        static std::string readFile(const std::string& path);
        static GLuint compile(GLuint type, const std::string& source);
        GLint getUniformLocation(const std::string& name) const;

    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);

        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        void use() const;

        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setBool(const std::string& name, bool value) const;
        void setVec2(const std::string& name, const glm::vec2& value) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setMat4(const std::string& name, const glm::mat4& value) const;
    };

} // namespace nesmart::graphics
