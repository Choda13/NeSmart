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
