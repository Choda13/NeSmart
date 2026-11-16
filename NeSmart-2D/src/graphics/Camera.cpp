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

        float scale = std::min(width / simulation::SAFE_WIDTH, height / simulation::SAFE_HEIGHT);

        float halfWidth = width / (2.0f * scale);
        float halfHeight = height / (2.0f * scale);

        m_projection = glm::ortho(simulation::SAFE_CENTER_X - halfWidth, simulation::SAFE_CENTER_X + halfWidth,
                                  simulation::SAFE_CENTER_Y + halfHeight, simulation::SAFE_CENTER_Y - halfHeight);

        m_screenProjection = glm::ortho(0.0f, width, height, 0.0f);
    }

} // namespace nesmart::graphics
