#pragma once

#include "simulation/Bus.hpp"
#include "simulation/Route.hpp"

namespace nesmart::simulation
{
    // Owns all simulation state and advances it. Pure logic: no OpenGL or GLFW.
    // Throws std::runtime_error if the route cannot be built.
    class Simulation
    {
    public:
        Simulation();

        void update(float dt);

        const Route& route() const { return m_route; }
        const Bus& bus() const { return m_bus; }

    
        bool addPassenger();
        bool removePassenger();

    private:
        Route m_route; // declared before m_bus: the bus reads it in its constructor
        Bus m_bus;
    };

} // namespace nesmart::simulation
