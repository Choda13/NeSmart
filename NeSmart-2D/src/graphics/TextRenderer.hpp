#pragma once

#include <string>

#include <glm/glm.hpp>

#include "graphics/Font.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"

namespace nesmart::graphics
{
    // Draws text with the given text shader and font, one quad per glyph. The shader and font must outlive this
    // object. Call begin() with a projection first.
    class TextRenderer
    {
    public:
        TextRenderer(Shader& shader, Font& font);

        void begin(const glm::mat4& projection);

        // Draws the text centred on the given world position. Glyph sizes are in pixels but placed in world units,
        // so one font pixel is one world unit.
        void drawCentered(const std::string& text, glm::vec2 center, const glm::vec3& color);

        // Draws left-aligned text. topLeft is the top-left corner of the digits' box: the pen starts at its x and
        // the baseline is one digit height below its y. The scale multiplies the font's pixel size.
        void drawAt(const std::string& text, glm::vec2 topLeft, const glm::vec3& color, float scale = 1.0f);

    private:
        Shader& m_shader;
        Font& m_font;
        Mesh m_quad;
    };

} // namespace nesmart::graphics
