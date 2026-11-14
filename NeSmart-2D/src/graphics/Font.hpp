#pragma once

#include <array>
#include <string>

#include <GL/glew.h>
#include <glm/vec2.hpp>

namespace nesmart::graphics
{
    // A TrueType font rendered once, at one pixel size, into a single one-channel atlas texture (printable ASCII).
    // FreeType is only used inside the constructor; afterwards the font is just a texture plus per-glyph metrics.
    // Throws std::runtime_error if the file cannot be loaded or the glyphs do not fit the atlas.
    class Font
    {
    public:
        // Sizes are in pixels. Unit-quad convention matches Renderer: the quad is y-down and its UV t runs 1 (top)
        // to 0 (bottom), so uvSize.y is negative and uvOffset points at the glyph's bottom-left in the atlas.
        struct Glyph
        {
            glm::vec2 uvOffset; // uv = uvOffset + quadUV * uvSize
            glm::vec2 uvSize;
            glm::vec2 size;    // bitmap size
            glm::vec2 bearing; // x: pen to bitmap left edge; y: baseline to bitmap top edge (up)
            float advance;     // pen movement to the next glyph
        };

        Font(const std::string& path, unsigned int pixelHeight);
        ~Font();

        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        void bind(unsigned int unit = 0) const;

        // Characters outside printable ASCII fall back to '?'.
        const Glyph& glyph(char c) const;

        // Sum of the glyph advances: the pen distance the string occupies.
        float width(const std::string& text) const;

    private:
        static constexpr int FIRST_CHAR = 32;
        static constexpr int LAST_CHAR = 126;
        static constexpr std::size_t GLYPH_COUNT = LAST_CHAR - FIRST_CHAR + 1;

        GLuint m_id = 0;
        std::array<Glyph, GLYPH_COUNT> m_glyphs{};
    };

} // namespace nesmart::graphics
