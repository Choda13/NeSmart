#pragma once

#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "simulation/Route.hpp"

namespace nesmart::graphics
{
    // Draws the scene. Owns every GL object it needs and decides the draw order of each frame.
    // Needs a current GL context, so it must be constructed after the Window.
    // Builds the route's mesh from the given route once; it does not keep a reference to it.
    // Throws std::runtime_error if a shader or image cannot be loaded.
    class Renderer
    {
    public:
        Renderer(int screenWidth, int screenHeight, const simulation::Route& route);

        void draw();

    private:
        void drawBackground();
        void drawRoute();

        // Declaration order is initialization order; the constructor relies on it.
        Shader m_spriteShader;      // textured sprites
        Shader m_solidShader; // flat-coloured shapes
        Camera m_camera;
        Mesh m_quad;
        Mesh m_routeMesh;
        Texture m_background;
    };

} // namespace nesmart::graphics
