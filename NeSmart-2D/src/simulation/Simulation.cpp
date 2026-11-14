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

} // namespace nesmart::simulation
