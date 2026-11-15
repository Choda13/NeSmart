#include "app/Application.hpp"

#include <cstdio>

#include <GLFW/glfw3.h> // after Application.hpp: GLEW's header must come before any other GL header

namespace nesmart::app
{

    Application::Application()
        : m_window("NeSmart"), m_limiter(75.0), m_camera(m_window.width(), m_window.height()),
          m_renderer(m_camera, m_simulation)
    {
        glfwSetWindowUserPointer(m_window.handle(), this);
        glfwSetKeyCallback(m_window.handle(), onKey);
        glfwSetMouseButtonCallback(m_window.handle(), onMouseButton);
    }

    int Application::run()
    {
        constexpr double FPS_LOG_INTERVAL = 1.0; // seconds
        double sinceFpsLog = 0.0;

        while (!m_window.shouldClose())
        {
            const double dt = m_limiter.delta();
            m_simulation.update(static_cast<float>(dt));
            m_renderer.draw();

            m_window.swapBuffers();
            m_window.pollEvents();
            m_limiter.tick();
        }
        return 0;
    }

    void Application::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto* self = static_cast<Application*>(glfwGetWindowUserPointer(window));

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            self->m_window.close();
        }
    }

    // Left click: a passenger boards; right click: a passenger leaves. The simulation ignores both while the bus is
    // driving or when the count is at its limit, and the click position does not matter.
    void Application::onMouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        auto* self = static_cast<Application*>(glfwGetWindowUserPointer(window));

        if (action != GLFW_PRESS)
        {
            return;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            self->m_simulation.addPassenger();
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            self->m_simulation.removePassenger();
        }
    }

} // namespace nesmart::app