#pragma once

#include <glm/glm.hpp>

namespace nesmart::graphics
{
    class Texture;

    struct Sprite
    {
        const Texture* texture = nullptr;
        glm::vec2 position{0.0f};         
        glm::vec2 size{1.0f};             
        float rotation = 0.0f;            
        glm::vec2 origin{0.0f};           
        bool flipX = false;
        bool flipY = false;

        glm::mat4 model() const;
    };

    Sprite makeSprite(const Texture& texture, float width, glm::vec2 position, glm::vec2 origin);

} // namespace nesmart::graphics
