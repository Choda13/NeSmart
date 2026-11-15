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
    // Owns the resources, sets up the GL state and draws one frame: the world (Scene), then the
    // overlay (Hud). Needs a current GL context, so it must be constructed after the Window.
    // Throws std::runtime_error if a shader, font or image cannot be loaded.
    class Renderer
    {
    public:
        // The camera and the simulation must outlive the renderer.
        Renderer(const Camera& camera, const simulation::Simulation& simulation);

        void draw();

    private:
        // Declaration order is initialization order; the constructor relies on it.
        // m_resources comes first so it outlives everything that refers to it.
        ResourceManager m_resources;
        SpriteRenderer m_spriteRenderer;
        TextRenderer m_textRenderer;     // regular font, for the world (station labels)
        TextRenderer m_boldTextRenderer; // bold font, for the HUD
        Scene m_scene;
        Hud m_hud;
    };

} // namespace nesmart::graphics
