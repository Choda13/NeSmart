#include "Application.hpp"

#include <GLFW/glfw3.h> // after Application.hpp: GLEW's header must come before any other GL header

namespace nesmart::app {

    Application::Application()
        : m_window("NeSmart"), m_limiter(75.0), m_renderer(m_window.width(), m_window.height()) {
        glfwSetWindowUserPointer(m_window.handle(), this);
        glfwSetKeyCallback(m_window.handle(), onKey);
    }

    int Application::run() {
        while (!m_window.shouldClose()) {
            const double dt = m_limiter.delta();

            // m_simulation.update(dt);
            m_renderer.draw();
            (void)dt;

            m_window.swapBuffers();
            m_window.pollEvents();
            m_limiter.tick();
        }
        return 0;
    }

    void Application::onKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto* self = static_cast<Application*>(glfwGetWindowUserPointer(window));

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            self->m_window.close();
    }

} // namespace nesmart::app