#pragma once
#include "FrameLimiter.hpp"
#include "Window.hpp"
#include "graphics/Renderer.hpp"
#include "simulation/Route.hpp"

namespace nesmart::app {

    // Owns the window and the main loop. Will also own the simulation and the
    // renderer, and wire input to the simulation.
    class Application {
    public:
        Application();

        int run();

    private:
        static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);

        Window m_window;
        FrameLimiter m_limiter;
        // simulation::Simulation m_simulation;
        simulation::Route m_route;     // must come before m_renderer: the renderer builds its mesh from it
        graphics::Renderer m_renderer; // must come after m_window: needs its GL context
    };

} // namespace nesmart::app