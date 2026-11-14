#pragma once

#include <string>
#include <GL/glew.h>

namespace nesmart::graphics
{
    // Owns a 2D GL texture loaded from an image file (PNG, JPG, ...), always stored as RGBA.
    // Images are flipped on load, so UV (0, 0) is the bottom-left of the image (OpenGL convention).
    // Throws std::runtime_error if the file cannot be loaded.
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

        // Width divided by height of the original image, for drawing it undistorted at any size.
        float aspectRatio() const { return static_cast<float>(m_width) / static_cast<float>(m_height); }

    private:
        GLuint m_id = 0;
        int m_width = 0;
        int m_height = 0;
    };

} // namespace nesmart::graphics
