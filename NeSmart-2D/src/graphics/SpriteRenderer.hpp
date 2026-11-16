#pragma once

#include <glm/glm.hpp>

#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/Texture.hpp"

namespace nesmart::graphics
{
    class SpriteRenderer
    {
    public:
        explicit SpriteRenderer(Shader& shader);

        void begin(const glm::mat4& projection);
        void draw(const Sprite& sprite);

    private:
        Shader& m_shader;
        Mesh m_quad;
        const Texture* m_lastTexture = nullptr;
    };

} // namespace nesmart::graphics
