#pragma once
#include "FrameLimiter.hpp"
#include "Window.hpp"
#include "graphics/Camera.hpp"
#include "graphics/Renderer.hpp"
#include "simulation/Simulation.hpp"

namespace nesmart::app
{

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
        simulation::Simulation m_simulation;
        graphics::Camera m_camera;      
        graphics::Renderer m_renderer;
    };

} // namespace nesmart::app