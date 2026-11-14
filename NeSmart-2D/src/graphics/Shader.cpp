#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader.hpp"

namespace nesmart::graphics
{
    Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    {
        std::string vertexSource = readFile(vertexPath);
        std::string fragmentSource = readFile(fragmentPath);
        unsigned int vertexShader = compile(GL_VERTEX_SHADER, vertexSource);
        unsigned int fragmentShader = compile(GL_FRAGMENT_SHADER, fragmentSource);

        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertexShader);
        glAttachShader(m_ID, fragmentShader);
        glLinkProgram(m_ID);

        int success;
        char infoLog[512];

        glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
            throw std::runtime_error("Shader program linking failed: " + std::string(infoLog));
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    Shader::~Shader() { glDeleteProgram(m_ID); }

    GLuint Shader::compile(GLuint type, const std::string& source)
    {
        GLuint shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
        }
        return shader;
    }

    std::string Shader::readFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open shader file: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    GLint Shader::getUniformLocation(const std::string& name) const
    {
        GLint location = glGetUniformLocation(m_ID, name.c_str());
        if (location == -1)
        {
            std::cerr << "Warning : Uniform '" << name << "' not found in shader program." << std::endl;
        }
        return location;
    }
    
    void Shader::use() const { glUseProgram(m_ID); }

    void Shader::setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
    }

    void Shader::setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(m_ID, name.c_str()), static_cast<int>(value));
    }

    void Shader::setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y);
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3f(glGetUniformLocation(m_ID, name.c_str()), value.x, value.y, value.z);
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
    }

} // namespace nesmart::graphics