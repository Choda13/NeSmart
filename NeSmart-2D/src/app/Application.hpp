#pragma once
#include "FrameLimiter.hpp"
#include "Window.hpp"

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
        // rendering::Renderer m_renderer;
    };

} // namespace nesmart::app