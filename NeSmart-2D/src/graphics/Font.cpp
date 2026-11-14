#include "graphics/Font.hpp"

#include <cstring>
#include <memory>
#include <stdexcept>
#include <vector>

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace nesmart::graphics
{
    namespace
    {
        constexpr int ATLAS_SIZE = 512;
        constexpr int PADDING = 1; // gap between glyphs so linear filtering does not bleed neighbours together
    } // namespace

    Font::Font(const std::string& path, unsigned int pixelHeight)
    {
        FT_Library rawLibrary = nullptr;
        if (FT_Init_FreeType(&rawLibrary) != 0)
        {
            throw std::runtime_error("Failed to initialize FreeType");
        }
        std::unique_ptr<FT_LibraryRec_, decltype(&FT_Done_FreeType)> library(rawLibrary, &FT_Done_FreeType);

        FT_Face rawFace = nullptr;
        if (FT_New_Face(library.get(), path.c_str(), 0, &rawFace) != 0)
        {
            throw std::runtime_error("Failed to load font: " + path);
        }
        std::unique_ptr<FT_FaceRec_, decltype(&FT_Done_Face)> face(rawFace, &FT_Done_Face);

        FT_Set_Pixel_Sizes(face.get(), 0, pixelHeight);

        // Atlas is kept top-down (row 0 first), the same order FreeType delivers bitmaps in.
        std::vector<unsigned char> atlas(static_cast<std::size_t>(ATLAS_SIZE) * ATLAS_SIZE, 0);
        constexpr float ATLAS_F = static_cast<float>(ATLAS_SIZE);

        int penX = PADDING;
        int penY = PADDING;
        int rowHeight = 0;

        for (std::size_t i = 0; i < GLYPH_COUNT; ++i)
        {
            if (FT_Load_Char(face.get(), FIRST_CHAR + static_cast<FT_ULong>(i), FT_LOAD_RENDER) != 0)
            {
                throw std::runtime_error("Failed to render glyph from font: " + path);
            }

            const FT_GlyphSlot slot = face->glyph;
            const int width = static_cast<int>(slot->bitmap.width);
            const int height = static_cast<int>(slot->bitmap.rows);

            if (penX + width + PADDING > ATLAS_SIZE)
            {
                penX = PADDING;
                penY += rowHeight + PADDING;
                rowHeight = 0;
            }
            if (penY + height + PADDING > ATLAS_SIZE)
            {
                throw std::runtime_error("Font atlas too small for the requested pixel height: " + path);
            }

            if (width > 0)
            {
                for (int row = 0; row < height; ++row)
                {
                    std::memcpy(&atlas[static_cast<std::size_t>(penY + row) * ATLAS_SIZE + penX],
                                slot->bitmap.buffer + static_cast<std::ptrdiff_t>(row) * slot->bitmap.pitch,
                                static_cast<std::size_t>(width));
                }
            }

            Glyph& glyph = m_glyphs[i];
            glyph.uvOffset = {penX / ATLAS_F, (penY + height) / ATLAS_F};
            glyph.uvSize = {width / ATLAS_F, -height / ATLAS_F};
            glyph.size = {static_cast<float>(width), static_cast<float>(height)};
            glyph.bearing = {static_cast<float>(slot->bitmap_left), static_cast<float>(slot->bitmap_top)};
            glyph.advance = static_cast<float>(slot->advance.x >> 6); // 26.6 fixed point -> pixels

            penX += width + PADDING;
            if (height > rowHeight)
            {
                rowHeight = height;
            }
        }

        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // One byte per pixel, so rows are not 4-byte aligned.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_SIZE, ATLAS_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, atlas.data());
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    Font::~Font() { glDeleteTextures(1, &m_id); }

    void Font::bind(unsigned int unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, m_id);
    }

    const Font::Glyph& Font::glyph(char c) const
    {
        const int code = static_cast<unsigned char>(c);
        if (code < FIRST_CHAR || code > LAST_CHAR)
        {
            return m_glyphs['?' - FIRST_CHAR];
        }
        return m_glyphs[static_cast<std::size_t>(code - FIRST_CHAR)];
    }

    float Font::width(const std::string& text) const
    {
        float total = 0.0f;
        for (const char c : text)
        {
            total += glyph(c).advance;
        }
        return total;
    }

} // namespace nesmart::graphics
