#include "graphics/Camera.hpp"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "simulation/World.hpp"

namespace nesmart::graphics
{
    Camera::Camera(int screenWidth, int screenHeight)
    {
        float width = static_cast<float>(screenWidth);
        float height = static_cast<float>(screenHeight);
        m_screenSize = {width, height};

        // Screen pixels per world unit: the largest scale at which the whole safe area still fits.
        float scale = std::min(width / simulation::SAFE_WIDTH, height / simulation::SAFE_HEIGHT);

        // Half of the visible world area; the limiting axis equals half of the safe area exactly.
        float halfWidth = width / (2.0f * scale);
        float halfHeight = height / (2.0f * scale);

        // Bottom is larger than top on purpose: world y points down, the origin is the top-left corner.
        m_projection = glm::ortho(simulation::SAFE_CENTER_X - halfWidth, simulation::SAFE_CENTER_X + halfWidth,
                                  simulation::SAFE_CENTER_Y + halfHeight, simulation::SAFE_CENTER_Y - halfHeight);

        // Same flipped y as above: (0, 0) is the top-left pixel of the screen.
        m_screenProjection = glm::ortho(0.0f, width, height, 0.0f);
    }

} // namespace nesmart::graphics
