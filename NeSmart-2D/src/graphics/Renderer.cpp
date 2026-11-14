#include "graphics/Renderer.hpp"

#include <vector>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "simulation/World.hpp"

namespace nesmart::graphics
{
    namespace
    {
        constexpr const char* SPRITE_VERT_PATH = "assets/shaders/sprite.vert";
        constexpr const char* SPRITE_FRAG_PATH = "assets/shaders/sprite.frag";
        constexpr const char* BACKGROUND_PATH = "assets/images/background.png";

        Mesh makeUnitQuad()
        {
            const std::vector<float> vertices = {
                //  x     y     s     t
                0.0f, 0.0f, 0.0f, 1.0f, // top-left
                1.0f, 0.0f, 1.0f, 1.0f, // top-right
                0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
                1.0f, 1.0f, 1.0f, 0.0f, // bottom-right
            };
            return Mesh(vertices, {2, 2}, GL_TRIANGLE_STRIP);
        }
    } // namespace

    Renderer::Renderer(int screenWidth, int screenHeight)
        : m_shader(SPRITE_VERT_PATH, SPRITE_FRAG_PATH)
        , m_camera(screenWidth, screenHeight)
        , m_quad(makeUnitQuad())
        , m_background(BACKGROUND_PATH)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void Renderer::draw()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        m_shader.use();
        m_shader.setMat4("uProjection", m_camera.projection());
        m_shader.setInt("uTexture", 0);

        drawBackground();
    }

    void Renderer::drawBackground()
    {
        // The unit quad scaled to the whole world covers the background image exactly.
        const glm::mat4 model =
            glm::scale(glm::mat4(1.0f), glm::vec3(simulation::WORLD_WIDTH, simulation::WORLD_HEIGHT, 1.0f));
        m_shader.setMat4("uModel", model);

        m_background.bind(0);
        m_quad.draw();
    }

} // namespace nesmart::graphics
