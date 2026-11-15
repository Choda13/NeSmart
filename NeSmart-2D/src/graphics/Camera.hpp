#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace nesmart::graphics
{
    // Turns world space (see simulation/World.hpp) into normalized device coordinates.
    //
    // The safe area is scaled uniformly to fit the screen, so it is always fully visible and circles stay
    // round. On a screen with a different aspect ratio the extra width or height shows more of the
    // background around the safe area. The matrix is computed once, as the window never resizes.
    class Camera
    {
    public:
        Camera(int screenWidth, int screenHeight);

        const glm::mat4& projection() const { return m_projection; }

        // Screen space for HUD: units are screen pixels, origin at the top-left corner, y down.
        // Independent of the world and the safe area.
        const glm::mat4& screenProjection() const { return m_screenProjection; }

        // Screen size in pixels, for anchoring HUD items to the corners.
        glm::vec2 screenSize() const { return m_screenSize; }

    private:
        glm::mat4 m_projection{1.0f};
        glm::mat4 m_screenProjection{1.0f};
        glm::vec2 m_screenSize{0.0f};
    };

} // namespace nesmart::graphics
