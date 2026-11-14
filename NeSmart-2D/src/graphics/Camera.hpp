#pragma once

#include <glm/mat4x4.hpp>

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

    private:
        glm::mat4 m_projection{1.0f};
    };

} // namespace nesmart::graphics
