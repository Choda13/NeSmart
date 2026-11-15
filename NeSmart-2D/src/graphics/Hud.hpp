#pragma once

#include "graphics/Camera.hpp"
#include "graphics/ResourceManager.hpp"
#include "graphics/Sprite.hpp"
#include "graphics/SpriteRenderer.hpp"
#include "graphics/TextRenderer.hpp"
#include "simulation/Simulation.hpp"

namespace nesmart::graphics
{
    // Draws the screen-space overlay (units are screen pixels). The doors icon (top-left), the passengers card (top-right) and the inspector icon (bottom-left, only while an inspector is aboard).
    // The simulation, resources, camera and renderers must outlive it. Needs a current GL context.
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
        TextRenderer& m_textRenderer; // expected to use the bold font

        Sprite m_doorsClosed;
        Sprite m_doorsOpened;
        Sprite m_inspector; // bottom-left, drawn only while an inspector is on the bus
        Sprite m_finesCard; // right under the passengers card
        Sprite m_passengersCard; // people icon baked into the image; the count is drawn on top as text
    };

} // namespace nesmart::graphics
