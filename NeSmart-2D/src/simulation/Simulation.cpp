#include "simulation/Simulation.hpp"

namespace nesmart::simulation
{
    Simulation::Simulation()
        : m_bus(m_route)
    {
    }

    void Simulation::update(float dt)
    {
        m_bus.update(dt, m_route);
    }

    bool Simulation::addPassenger() { return m_bus.addPassenger(); }
    bool Simulation::removePassenger() { return m_bus.removePassenger(); }
} // namespace nesmart::simulation
