#include <GLFW/glfw3.h>

#include "Application.hpp"

namespace nesmart::app {

    Application::Application() : m_window("NeSmart"), m_limiter(75.0) {
        glfwSetWindowUserPointer(m_window.handle(), this);
        glfwSetKeyCallback(m_window.handle(), onKey);
        // glfwSetMouseButtonCallback(...), glfwSetCursorPosCallback(...) go here later
    }

    int Application::run() {
        while (!m_window.shouldClose()) {
            const double dt = m_limiter.delta();

            // m_simulation.update(dt);
            // m_renderer.draw(m_simulation);
            (void)dt;

            m_window.swapBuffers();
            m_window.pollEvents();
            m_limiter.tick();
        }
        return 0;
    }

    void Application::onKey(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
        auto* self = static_cast<Application*>(glfwGetWindowUserPointer(window));

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            self->m_window.close();
    }

} // namespace nesmart::app