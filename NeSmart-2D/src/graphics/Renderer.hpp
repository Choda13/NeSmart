#pragma once

#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"

namespace nesmart::graphics
{
    // Draws the scene. Owns every GL object it needs and decides the draw order of each frame.
    // Needs a current GL context, so it must be constructed after the Window.
    // Throws std::runtime_error if a shader or image cannot be loaded.
    class Renderer
    {
    public:
        Renderer(int screenWidth, int screenHeight);

        void draw();

    private:
        void drawBackground();

        // Declaration order is initialization order; the constructor relies on it.
        Shader m_shader;
        Camera m_camera;
        Mesh m_quad;
        Texture m_background;
    };

} // namespace nesmart::graphics
