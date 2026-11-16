#pragma once

#include "graphics/Camera.hpp"
#include "graphics/ResourceManager.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/SpriteRenderer.hpp"
#include "graphics/TextRenderer.hpp"
#include "simulation/Simulation.hpp"

namespace nesmart::graphics
{
    class Hud
    {
    public:
        Hud(ResourceManager& resources, const Camera& camera, SpriteRenderer& spriteRenderer,
            TextRenderer& textRenderer, const simulation::Simulation& simulation);

        void draw();

    private:
        const simulation::Simulation& m_simulation;
        const Camera& m_camera;
        SpriteRenderer& m_spriteRenderer;
        TextRenderer& m_textRenderer;

        Sprite m_doorsClosed;
        Sprite m_doorsOpened;
        Sprite m_inspector; 
        Sprite m_finesCard;
        Sprite m_passengersCard;
    };

} // namespace nesmart::graphics
