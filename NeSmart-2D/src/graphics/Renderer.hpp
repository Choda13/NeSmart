#pragma once

#include <array>
#include <string>

#include <glm/glm.hpp>

#include "graphics/Camera.hpp"
#include "graphics/Font.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "simulation/Route.hpp"
#include "simulation/Simulation.hpp"

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

        void draw(const simulation::Simulation& simulation);

    private:
        void drawBackground();
        void drawRoute();
        void drawStations();
        void drawBus(const simulation::Bus& bus);

        // Draws the text centred on the given world position, one quad per glyph.
        void drawTextCentered(const std::string& text, glm::vec2 center, const glm::vec3& color);

        // Declaration order is initialization order; the constructor relies on it.
        Shader m_spriteShader;      
        Shader m_solidShader;
        Shader m_textShader;

        Camera m_camera;

        Mesh m_quadMesh;
        Mesh m_routeMesh;

        Texture m_backgroundTex;
        Texture m_circleTex;
        Texture m_busTex;

        Font m_font;
        std::array<glm::vec2, simulation::Route::STATION_COUNT> m_stationPositions;
    };

} // namespace nesmart::graphics
