#include "simulation/Bus.hpp"

#include <cmath>

namespace nesmart::simulation
{
    Bus::Bus(const Route& route)
        : m_distance(route.stationDistance(0))
    {
        cachePose(route);
        m_heading = route.headingAt(m_distance); // no threshold on the first frame
        m_facingLeft = std::cos(m_heading) < 0.0f;
    }

    void Bus::update(float dt, const Route& route)
    {
        if (m_state == State::StoppedAtStation)
        {
            m_stopTimer -= dt;
            if (m_stopTimer <= 0.0f)
            {
                m_state = State::Driving; // starts moving next frame
                m_nextStation = (m_nextStation + 1) % Route::STATION_COUNT;
            }
            return;
        }

        const float length = route.totalLength();
        const float step = SPEED * dt;
        const float target = route.stationDistance(m_nextStation);

        // Distance from here to the next station, going forward around the loop.
        float ahead = target - m_distance;
        if (ahead <= 0.0f)
        {
            ahead += length;
        }

        if (ahead <= step)
        {
            // Arrived: snap onto the station and wait.
            m_distance = target;
            m_state = State::StoppedAtStation;
            m_stopTimer = STOP_DURATION;

            // The inspector gets off at the first station after boarding, so another one can board right here.
            if (m_hasInspector)
            {
                m_hasInspector = false;
                m_passengerCount--;
            }
        }
        else
        {
            // fmod keeps the overshoot past the end of the loop, so the speed stays constant across the seam.
            m_distance = std::fmod(m_distance + step, length);
        }
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

    bool Bus::addPassenger()
    {
        if (m_state == State::StoppedAtStation && m_passengerCount < BUS_CAPACITY)
        {
            m_passengerCount++;
            return true;
        }
        return false;
    }
    bool Bus::removePassenger() {
        // The inspector is counted as a passenger but leaves only on its own, so it is never removed here.
        const unsigned int inspectorCount = m_hasInspector ? 1 : 0;
        if (m_passengerCount > inspectorCount && m_state == State::StoppedAtStation)
        {
            m_passengerCount--;
            return true;
        }
        return false;
    }

    bool Bus::addInspector()
    {
        if (m_state == State::StoppedAtStation && !m_hasInspector)
        {
            m_hasInspector = true;
            m_passengerCount++; // no capacity check: the inspector boards even when the bus is full
            return true;
        }
        return false;
    }

} // namespace nesmart::simulation
