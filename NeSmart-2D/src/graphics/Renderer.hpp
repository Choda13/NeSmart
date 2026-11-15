#pragma once

#include <array>

#include "graphics/Camera.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/ResourceManager.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/SpriteRenderer.hpp"
#include "graphics/TextRenderer.hpp"
#include "simulation/Route.hpp"
#include "simulation/Simulation.hpp"

namespace nesmart::graphics
{
    // Draws the scene: loads the assets, builds the static sprites once and decides the draw order of each frame.
    // Needs a current GL context, so it must be constructed after the Window.
    // Builds the route's mesh from the given route once; it does not keep a reference to it.
    // Throws std::runtime_error if a shader, font or image cannot be loaded.
    class Renderer
    {
    public:
        Renderer(int screenWidth, int screenHeight, const simulation::Route& route);

        void draw(const simulation::Simulation& simulation);

    private:
        void drawRoute();

        // Declaration order is initialization order; the constructor relies on it.
        // m_resources comes first so it outlives everything that refers to it.
        ResourceManager m_resources;
        Camera m_camera;
        SpriteRenderer m_spritesRenderer;
        TextRenderer m_textRenderer;
        Shader& m_solidShader;
        Mesh m_routeMesh;

        Sprite m_background;
        std::array<Sprite, simulation::Route::STATION_COUNT> m_stations;
        Sprite m_bus;
        Sprite m_doorsClosed; // HUD, screen pixels
        Sprite m_doorsOpened;
    };

} // namespace nesmart::graphics
