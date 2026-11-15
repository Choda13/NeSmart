#pragma once
#include "FrameLimiter.hpp"
#include "Window.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Renderer.hpp"
#include "simulation/Simulation.hpp"

namespace nesmart::app
{

    // Owns the window, the frame limiter, the simulation, the camera and the renderer, and runs the main loop.
    // Will also wire input to the simulation.
    class Application
    {
    public:
        Application();

        int run();

    private:
        static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void onMouseButton(GLFWwindow* window, int button, int action, int mods);

        Window m_window;
        FrameLimiter m_limiter;
        simulation::Simulation m_simulation; // must come before m_renderer: the renderer builds its mesh from the route
        graphics::Camera m_camera;           // must come after m_window (screen size) and before m_renderer
        graphics::Renderer m_renderer;       // must come after m_window: needs its GL context
    };

} // namespace nesmart::app