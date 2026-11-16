#pragma once

#include <string>

#include <glm/glm.hpp>

#include "graphics/Font.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"

namespace nesmart::graphics
{
    class TextRenderer
    {
    public:
        TextRenderer(Shader& shader, Font& font);

        void begin(const glm::mat4& projection);

        void drawCentered(const std::string& text, glm::vec2 center, const glm::vec3& color);

        void drawAt(const std::string& text, glm::vec2 topLeft, const glm::vec3& color, float scale = 1.0f);

        // Size of the text as drawAt() lays it out: pen width, and the digit height that drawAt() treats as its box.
        float width(const std::string& text, float scale = 1.0f) const;
        float height(float scale = 1.0f) const;

    private:
        Shader& m_shader;
        Font& m_font;
        Mesh m_quad;
    };

} // namespace nesmart::graphics
