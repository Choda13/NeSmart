#pragma once

#include "simulation/Bus.hpp"
#include "simulation/Route.hpp"

namespace nesmart::simulation
{
    class Simulation
    {
    public:
        Simulation();

        void update(float dt);

        const Route& route() const { return m_route; }
        const Bus& bus() const { return m_bus; }

    
        bool addPassenger();
        bool removePassenger();
        bool addInspector();

    private:
        Route m_route;
        Bus m_bus;
    };

} // namespace nesmart::simulation
