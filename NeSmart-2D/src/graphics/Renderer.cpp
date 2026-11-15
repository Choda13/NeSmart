#include "graphics/Renderer.hpp"

#include <GL/glew.h>

namespace nesmart::graphics
{
    Renderer::Renderer(const Camera& camera, const simulation::Simulation& simulation)
        : m_spriteRenderer(m_resources.shader("sprite")),
          m_textRenderer(m_resources.shader("text"), m_resources.font("roboto")),
          m_scene(m_resources, camera, m_spriteRenderer, m_textRenderer, simulation),
          m_hud(m_resources, camera, m_spriteRenderer, m_textRenderer, simulation)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void Renderer::draw()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        m_scene.draw();
        m_hud.draw();
    }

} // namespace nesmart::graphics
