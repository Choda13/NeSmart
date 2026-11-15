#pragma once

#include <glm/glm.hpp>

#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/Texture.hpp"

namespace nesmart::graphics
{
    // Draws sprites with the given sprite shader. Call begin() with a projection, then draw() for each sprite.
    // The shader must outlive this object. Consecutive sprites with the same texture skip the rebind.
    class SpriteRenderer
    {
    public:
        explicit SpriteRenderer(Shader& shader);

        void begin(const glm::mat4& projection);

        // Throws std::runtime_error if the sprite has no texture.
        void draw(const Sprite& sprite);

    private:
        Shader& m_shader;
        Mesh m_quad;
        const Texture* m_lastTexture = nullptr;
    };

} // namespace nesmart::graphics
