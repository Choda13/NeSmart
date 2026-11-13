#include <GL/glew.h> // must come before glfw3.h
#include <GLFW/glfw3.h>

#include "Window.hpp"
#include <stdexcept>

namespace nesmart::app {

	Window::Window(const char* title) {
		if (!glfwInit())
			throw std::runtime_error("GLFW nije uspeo da se inicijalizuje.");

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		m_window = glfwCreateWindow(mode->width, mode->height, title, monitor, nullptr);
		if (m_window == nullptr) {
			destroy();
			throw std::runtime_error("Prozor nije uspeo da se kreira.");
		}
		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(0); // VSync off, FrameLimiter does the capping

		glewExperimental = GL_TRUE; // needed for core profile on some drivers
		if (glewInit() != GLEW_OK) {
			destroy();
			throw std::runtime_error("GLEW nije uspeo da se inicijalizuje.");
		}

		glfwGetFramebufferSize(m_window, &m_width, &m_height);
		glViewport(0, 0, m_width, m_height);
	}

	Window::~Window() { destroy(); }

	void Window::destroy() {
		if (m_window != nullptr) {
			glfwDestroyWindow(m_window);
			m_window = nullptr;
		}
		glfwTerminate();
	}

	bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }
	void Window::close() { glfwSetWindowShouldClose(m_window, GLFW_TRUE); }
	void Window::swapBuffers() { glfwSwapBuffers(m_window); }
	void Window::pollEvents() { glfwPollEvents(); }

} // namespace nesmart::app