#pragma once

#include <glm/vec2.hpp>

#include "simulation/Route.hpp"

namespace nesmart::simulation
{
    class Bus
    {
    public:
        // World units (pixels) per second.
        static constexpr float SPEED = 200.0f;

        // The shown heading only changes once the route's direction differs from it by more than this (radians).
        static constexpr float ROTATION_THRESHOLD = 0.05f; // about 3 degrees

        explicit Bus(const Route& route);

        void update(float dt, const Route& route);

        glm::vec2 position() const { return m_position; }
        float heading() const { return m_heading; } // radians, y points down, same convention as Route::headingAt
        bool isFacingLeft() const { return m_facingLeft; }

    private:
        void cachePose(const Route& route);

        float m_distance = 0.0f;
        glm::vec2 m_position{0.0f};
        float m_heading = 0.0f;
        bool m_facingLeft = false;
    };

} // namespace nesmart::simulation
