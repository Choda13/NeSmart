#include "app/Application.hpp"

#include <cstdio>

#include <GLFW/glfw3.h> // after Application.hpp: GLEW's header must come before any other GL header

namespace nesmart::app
{

    Application::Application()
        : m_window("NeSmart"), m_limiter(75.0), m_renderer(m_window.width(), m_window.height(), m_simulation.route())
    {
        glfwSetWindowUserPointer(m_window.handle(), this);
        glfwSetKeyCallback(m_window.handle(), onKey);
    }

    int Application::run()
    {
        constexpr double FPS_LOG_INTERVAL = 1.0; // seconds
        double sinceFpsLog = 0.0;

        while (!m_window.shouldClose())
        {
            const double dt = m_limiter.delta();

            sinceFpsLog += dt;
            if (sinceFpsLog >= FPS_LOG_INTERVAL)
            {
                std::printf("FPS: %.1f\n", m_limiter.fps());
                sinceFpsLog = 0.0;
            }

            m_simulation.update(static_cast<float>(dt));
            m_renderer.draw(m_simulation);

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

} // namespace nesmart::app