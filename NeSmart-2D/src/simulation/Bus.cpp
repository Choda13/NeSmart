#include "simulation/Bus.hpp"

#include <cmath>

namespace nesmart::simulation
{
    Bus::Bus(const Route& route)
    {
        cachePose(route);
        m_heading = route.headingAt(m_distance); // no threshold on the first frame
        m_facingLeft = std::cos(m_heading) < 0.0f;
    }

    void Bus::update(float dt, const Route& route)
    {
        // fmod keeps the overshoot past the end of the loop, so the speed stays constant across the seam.
        m_distance = std::fmod(m_distance + SPEED * dt, route.totalLength());
        cachePose(route);
    }

    void Bus::cachePose(const Route& route)
    {
        m_position = route.positionAt(m_distance);

        // Angle between the route's direction and the shown heading, wrapped to [-pi, pi] so +-pi doesn't jump.
        const float target = route.headingAt(m_distance);
        const float difference = std::atan2(std::sin(target - m_heading), std::cos(target - m_heading));
        if (std::abs(difference) > ROTATION_THRESHOLD)
        {
            m_heading = target;
            m_facingLeft = std::cos(m_heading) < 0.0f;
        }
    }

} // namespace nesmart::simulation
