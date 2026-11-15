#include "graphics/TextRenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace nesmart::graphics
{
    TextRenderer::TextRenderer(Shader& shader, Font& font) : m_shader(shader), m_font(font), m_quad(Mesh::unitQuad())
    {
    }

    void TextRenderer::begin(const glm::mat4& projection)
    {
        m_shader.use();
        m_shader.setMat4("uProjection", projection);
        m_shader.setInt("uTexture", 0);
        m_font.bind(0);
    }

    void TextRenderer::drawCentered(const std::string& text, glm::vec2 center, const glm::vec3& color)
    {
        m_shader.setVec3("uColor", color);

        // Vertical centring uses the digit height, which suits numbers (the world is y-down, so the baseline is lower).
        const float baseline = center.y + m_font.glyph('0').bearing.y * 0.5f;
        float penX = center.x - m_font.width(text) * 0.5f;

        for (const char c : text)
        {
            const Font::Glyph& glyph = m_font.glyph(c);

            const glm::vec2 topLeft{penX + glyph.bearing.x, baseline - glyph.bearing.y};
            const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(topLeft, 0.0f)) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(glyph.size, 1.0f));

            m_shader.setMat4("uModel", model);
            m_shader.setVec2("uUVOffset", glyph.uvOffset);
            m_shader.setVec2("uUVSize", glyph.uvSize);
            m_quad.draw();

            penX += glyph.advance;
        }
    }

} // namespace nesmart::graphics
