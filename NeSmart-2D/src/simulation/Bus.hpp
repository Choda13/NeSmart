#pragma once

#include <cstddef>
#include <glm/vec2.hpp>
#include "simulation/Route.hpp"

namespace nesmart::simulation
{
    class Bus
    {
    public:
        
        static constexpr unsigned int BUS_CAPACITY = 50;
        static constexpr float SPEED = 200.0f;
        static constexpr float ROTATION_THRESHOLD = 0.05f; // about 3 degrees
        static constexpr float STOP_DURATION = 10.0f;

        explicit Bus(const Route& route);

        void update(float dt, const Route& route);

        bool isMoving() const { return m_state == State::Driving; }
        glm::vec2 position() const { return m_position; }
        float heading() const { return m_heading; } // radians, y points down, same convention as Route::headingAt
        bool isFacingLeft() const { return m_facingLeft; }

        // Includes the inspector while one is on board, so it can briefly be BUS_CAPACITY + 1.
        unsigned int passengerCount() const { return m_passengerCount; }
        bool hasInspector() const { return m_hasInspector; }

        bool addPassenger();
        bool removePassenger(); // never removes the inspector

        // Only while stopped, and only one inspector at a time. Enters even if the bus is full. The inspector counts as
        // a passenger and leaves (count - 1) when the bus arrives at the next station.
        bool addInspector();

    private:
        enum class State
        {
            Driving,
            StoppedAtStation
        };

        void cachePose(const Route& route);

        State m_state = State::Driving;
        float m_stopTimer = 0.0f;
        std::size_t m_nextStation = 1;
        float m_distance = 0.0f;
        glm::vec2 m_position{0.0f};
        float m_heading = 0.0f;
        bool m_facingLeft = false;

        unsigned int m_passengerCount = 0;
        bool m_hasInspector = false;
    };

} // namespace nesmart::simulation
