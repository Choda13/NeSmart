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
    // Draws the world: background, route, stations with their labels and the bus, in that order.
    // Builds the static sprites and the route's mesh once from the simulation's route, and reads the bus state from
    // it every frame.
    // The simulation, resources, camera and renderers must outlive it. Needs a current GL context.
    class Scene
    {
    public:
        Scene(ResourceManager& resources, const Camera& camera, SpriteRenderer& spriteRenderer,
              TextRenderer& textRenderer, const simulation::Simulation& simulation);

        void draw();

    private:
        void drawRoute();

        const simulation::Simulation& m_simulation;
        const Camera& m_camera;
        SpriteRenderer& m_spriteRenderer;
        TextRenderer& m_textRenderer;
        Shader& m_solidShader;
        Mesh m_routeMesh;

        Sprite m_background;
        std::array<Sprite, simulation::Route::STATION_COUNT> m_stations;
        Sprite m_bus;
    };

} // namespace nesmart::graphics
