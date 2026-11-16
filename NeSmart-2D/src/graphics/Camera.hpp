#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

namespace nesmart::graphics
{
    class Camera
    {
    public:
        Camera(int screenWidth, int screenHeight);

        const glm::mat4& projection() const { return m_projection; }

        const glm::mat4& screenProjection() const { return m_screenProjection; }

        glm::vec2 screenSize() const { return m_screenSize; }

    private:
        glm::mat4 m_projection{1.0f};
        glm::mat4 m_screenProjection{1.0f};
        glm::vec2 m_screenSize{0.0f};
    };

} // namespace nesmart::graphics
