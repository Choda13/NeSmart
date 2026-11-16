#pragma once

#include <string>
#include <GL/glew.h>

namespace nesmart::graphics
{
    class Texture
    {
    public:
        explicit Texture(const std::string& path);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void bind(unsigned int unit = 0) const;

        int width() const { return m_width; }
        int height() const { return m_height; }

        float aspectRatio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

    private:
        GLuint m_id = 0;
        int m_width = 0;
        int m_height = 0;
    };

} // namespace nesmart::graphics
