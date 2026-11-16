#include "graphics/Scene.hpp"

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "simulation/World.hpp"

namespace nesmart::graphics
{
    namespace
    {
        constexpr float ROUTE_WIDTH = 15.0f;
        const glm::vec3 ROUTE_COLOR{0.878f, 0.192f, 0.192f};

        constexpr float STATION_DIAMETER = 100.0f;
        const glm::vec3 STATION_TEXT_COLOR{1.0f, 1.0f, 1.0f};

        constexpr float BUS_WIDTH = 110.0f;

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

    Scene::Scene(ResourceManager& resources, const Camera& camera, SpriteRenderer& spriteRenderer,
                 TextRenderer& textRenderer, const simulation::Simulation& simulation)
        : m_simulation(simulation), m_camera(camera), m_spriteRenderer(spriteRenderer), m_textRenderer(textRenderer),
          m_solidShader(resources.shader("solid")), m_routeMesh(makeRouteMesh(simulation.route(), ROUTE_WIDTH))
    {
        const simulation::Route& route = simulation.route();

        m_background.texture = &resources.texture("background");
        m_background.size = {simulation::WORLD_WIDTH, simulation::WORLD_HEIGHT};

        const Texture& circle = resources.texture("circle");
        for (std::size_t i = 0; i < m_stations.size(); ++i)
        {
            m_stations[i] = makeSprite(circle, STATION_DIAMETER, route.stationPosition(i), {0.5f, 0.5f});
        }

        m_bus = makeSprite(resources.texture("bus"), BUS_WIDTH, {0.0f, 0.0f}, {0.5f, 0.5f});
    }

    void Scene::draw()
    {
        const glm::mat4& projection = m_camera.projection();

        m_spriteRenderer.begin(projection);
        m_spriteRenderer.draw(m_background);

        drawRoute();

        m_spriteRenderer.begin(projection);
        for (const Sprite& station : m_stations)
        {
            m_spriteRenderer.draw(station);
        }

        m_textRenderer.begin(projection);
        for (std::size_t i = 0; i < m_stations.size(); ++i)
        {
            m_textRenderer.drawCentered(std::to_string(i), m_stations[i].position, STATION_TEXT_COLOR);
        }

        const simulation::Bus& bus = m_simulation.bus();
        m_bus.position = bus.position();
        m_bus.rotation = bus.heading();
        m_bus.flipX = true;
        m_bus.flipY = bus.isFacingLeft();

        m_spriteRenderer.begin(projection);
        m_spriteRenderer.draw(m_bus);
    }

    void Scene::drawRoute()
    {
        m_solidShader.use();
        m_solidShader.setMat4("uProjection", m_camera.projection());
        m_solidShader.setMat4("uModel", glm::mat4(1.0f));
        m_solidShader.setVec3("uColor", ROUTE_COLOR);

        m_routeMesh.draw();
    }

} // namespace nesmart::graphics
