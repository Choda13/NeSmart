#include "graphics/Sprite.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "graphics/Texture.hpp"

namespace nesmart::graphics
{
    glm::mat4 Sprite::model() const
    {
        const glm::vec3 scale{flipX ? -size.x : size.x, flipY ? -size.y : size.y, 1.0f};
        return glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f)) *
               glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
               glm::scale(glm::mat4(1.0f), scale) * glm::translate(glm::mat4(1.0f), glm::vec3(-origin, 0.0f));
    }

    Sprite makeSprite(const Texture& texture, float width, glm::vec2 position, glm::vec2 origin)
    {
        Sprite sprite;
        sprite.texture = &texture;
        sprite.position = position;
        sprite.size = {width, width / texture.aspectRatio()};
        sprite.origin = origin;
        return sprite;
    }

} // namespace nesmart::graphics
