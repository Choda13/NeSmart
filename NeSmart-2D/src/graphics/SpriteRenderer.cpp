#include "graphics/SpriteRenderer.hpp"

#include <stdexcept>

namespace nesmart::graphics
{
    SpriteRenderer::SpriteRenderer(Shader& shader) : m_shader(shader), m_quad(Mesh::unitQuad())
    {
    }

    void SpriteRenderer::begin(const glm::mat4& projection)
    {
        m_shader.use();
        m_shader.setMat4("uProjection", projection);
        m_shader.setInt("uTexture", 0);
        m_lastTexture = nullptr;
    }

    void SpriteRenderer::draw(const Sprite& sprite)
    {
        if (sprite.texture == nullptr)
        {
            throw std::runtime_error("SpriteRenderer: sprite has no texture");
        }

        if (sprite.texture != m_lastTexture)
        {
            sprite.texture->bind(0);
            m_lastTexture = sprite.texture;
        }

        m_shader.setMat4("uModel", sprite.model());
        m_quad.draw();
    }

} // namespace nesmart::graphics
