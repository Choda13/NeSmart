#pragma once

#include <glm/glm.hpp>

namespace nesmart::graphics
{
    class Texture;

    struct Sprite
    {
        const Texture* texture = nullptr;
        glm::vec2 position{0.0f};         // world position of the origin point
        glm::vec2 size{1.0f};             // world units
        float rotation = 0.0f;            // radians, around the origin point
        glm::vec2 origin{0.0f};           // pivot inside the sprite, 0..1 ((0, 0) top-left, (0.5, 0.5) center)
        bool flipX = false;               // mirror around the origin point
        bool flipY = false;

        // Matrix that maps the unit quad (0..1) onto the sprite in world space.
        glm::mat4 model() const;
    };

} // namespace nesmart::graphics
