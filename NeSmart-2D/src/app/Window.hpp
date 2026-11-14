#pragma once

struct GLFWwindow;

namespace nesmart::app
{

    // Owns GLFW init/shutdown, the fullscreen window, the GL context and GLEW.
    // Throws std::runtime_error if anything fails to initialize.
    class Window
    {
    public:
        explicit Window(const char* title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        GLFWwindow* handle() const { return m_window; }
        int width() const { return m_width; }
        int height() const { return m_height; }

        bool shouldClose() const;
        void close();
        void swapBuffers();
        void pollEvents();

    private:
        void destroy();

        GLFWwindow* m_window = nullptr;
        int m_width = 0;
        int m_height = 0;
    };

} // namespace nesmart::app