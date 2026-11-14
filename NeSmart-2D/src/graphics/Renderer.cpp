#include "graphics/Renderer.hpp"

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "simulation/World.hpp"

namespace nesmart::graphics
{
    namespace
    {
        constexpr const char* SPRITE_VERT_PATH = "assets/shaders/sprite.vert";
        constexpr const char* SPRITE_FRAG_PATH = "assets/shaders/sprite.frag";
        constexpr const char* SOLID_VERT_PATH = "assets/shaders/solid.vert";
        constexpr const char* SOLID_FRAG_PATH = "assets/shaders/solid.frag";
        constexpr const char* BACKGROUND_PATH = "assets/images/background.png";
        constexpr const char* CIRCLE_PATH = "assets/images/circle.png";

        // Thickness of the drawn route line, in world units.
        constexpr float ROUTE_WIDTH = 15.0f;
        const glm::vec3 ROUTE_COLOR{0.878f, 0.192f, 0.192f};

        // Square from (0, 0) to (1, 1) in y-down space, drawn as a triangle strip.
        // UVs are flipped vertically because Texture flips images on load (UV (0, 0) is the image's bottom-left).
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

        // Turns the route's centre line into a ribbon of the given width, built in world space.
        // Each point becomes two vertices, one on each side of the line, so the strip forms a continuous band.
        Mesh makeRouteMesh(const simulation::Route& route, float width)
        {
            const std::vector<glm::vec2>& points = route.points();
            const std::size_t count = points.size();

            std::vector<float> vertices;
            vertices.reserve((count + 1) * 4);

            // Goes one past the end so the first pair repeats and the ribbon closes the loop.
            for (std::size_t i = 0; i <= count; ++i)
            {
                const glm::vec2& previous = points[(i + count - 1) % count];
                const glm::vec2& current = points[i % count];
                const glm::vec2& next = points[(i + 1) % count];

                // Direction from the previous to the next point smooths the ribbon around bends.
                const glm::vec2 direction = glm::normalize(next - previous);
                const glm::vec2 offset = glm::vec2(-direction.y, direction.x) * (width * 0.5f);

                const glm::vec2 left = current + offset;
                const glm::vec2 right = current - offset;
                vertices.insert(vertices.end(), {left.x, left.y, right.x, right.y});
            }
            return Mesh(vertices, {2}, GL_TRIANGLE_STRIP);
        }
    } // namespace

    Renderer::Renderer(int screenWidth, int screenHeight, const simulation::Route& route)
        : m_spriteShader(SPRITE_VERT_PATH, SPRITE_FRAG_PATH)
        , m_solidShader(SOLID_VERT_PATH, SOLID_FRAG_PATH)
        , m_camera(screenWidth, screenHeight)
        , m_quad(makeUnitQuad())
        , m_routeMesh(makeRouteMesh(route, ROUTE_WIDTH))
        , m_background(BACKGROUND_PATH)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void Renderer::draw()
    {
        glClear(GL_COLOR_BUFFER_BIT);

        drawBackground();
        drawRoute();
    }

    void Renderer::drawBackground()
    {
        m_spriteShader.use();
        m_spriteShader.setMat4("uProjection", m_camera.projection());
        m_spriteShader.setInt("uTexture", 0);

        // The unit quad scaled to the whole world covers the background image exactly.
        const glm::mat4 model =
            glm::scale(glm::mat4(1.0f), glm::vec3(simulation::WORLD_WIDTH, simulation::WORLD_HEIGHT, 1.0f));
        m_spriteShader.setMat4("uModel", model);

        m_background.bind(0);
        m_quad.draw();
    }

    void Renderer::drawRoute()
    {
        m_solidShader.use();
        m_solidShader.setMat4("uProjection", m_camera.projection());
        // The route mesh is already in world space, so no model transform is needed.
        m_solidShader.setMat4("uModel", glm::mat4(1.0f));
        m_solidShader.setVec3("uColor", ROUTE_COLOR);

        m_routeMesh.draw();
    }

} // namespace nesmart::graphics
