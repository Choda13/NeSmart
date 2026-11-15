#pragma once
#include "FrameLimiter.hpp"
#include "Window.hpp"
#include "graphics/Renderer.hpp"
#include "simulation/Simulation.hpp"

namespace nesmart::app
{

    // Owns the window, the frame limiter, the simulation and the renderer, and runs the main loop.
    // Will also wire input to the simulation.
    class Application
    {
    public:
        Application();

        int run();

    private:
        static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

        Window m_window;
        FrameLimiter m_limiter;
        simulation::Simulation m_simulation; // must come before m_renderer: the renderer builds its mesh from the route
        graphics::Renderer m_renderer;       // must come after m_window: needs its GL context
    };

} // namespace nesmart::app