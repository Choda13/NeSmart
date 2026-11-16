#pragma once

#include "graphics/Camera.hpp"
#include "graphics/Hud.hpp"
#include "graphics/ResourceManager.hpp"
#include "graphics/Scene.hpp"
#include "graphics/SpriteRenderer.hpp"
#include "graphics/TextRenderer.hpp"
#include "simulation/Route.hpp"
#include "simulation/Simulation.hpp"

namespace nesmart::graphics
{
    class Renderer
    {
    public:
        Renderer(const Camera& camera, const simulation::Simulation& simulation);

        void draw();

    private:
        ResourceManager m_resources;
        SpriteRenderer m_spriteRenderer;
        TextRenderer m_textRenderer; 
        TextRenderer m_boldTextRenderer;
        Scene m_scene;
        Hud m_hud;
    };

} // namespace nesmart::graphics
