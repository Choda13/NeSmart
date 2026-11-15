#include "graphics/Renderer.hpp"

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "simulation/World.hpp"

namespace nesmart::graphics
{
    namespace
    {
        constexpr const char* FONT_PATH = "assets/fonts/roboto/Roboto-Regular.ttf";

        constexpr const char* SPRITE_VERT_PATH = "assets/shaders/sprite.vert";
        constexpr const char* SPRITE_FRAG_PATH = "assets/shaders/sprite.frag";
        constexpr const char* SOLID_VERT_PATH = "assets/shaders/solid.vert";
        constexpr const char* SOLID_FRAG_PATH = "assets/shaders/solid.frag";
        constexpr const char* TEXT_VERT_PATH = "assets/shaders/text.vert";
        constexpr const char* TEXT_FRAG_PATH = "assets/shaders/text.frag";

        constexpr const char* BACKGROUND_PATH = "assets/images/background.png";
        constexpr const char* CIRCLE_PATH = "assets/images/circle.png";
        constexpr const char* BUS_PATH = "assets/images/bus.png";
        constexpr const char* DOORS_CLOSED_PATH = "assets/images/doors_closed.png";
        constexpr const char* DOORS_OPENED_PATH = "assets/images/doors_opened.png";

        constexpr const char* FONT_NAME = "roboto";
        constexpr const char* SPRITE_SHADER_NAME = "sprite";
        constexpr const char* SOLID_SHADER_NAME = "solid";
        constexpr const char* TEXT_SHADER_NAME = "text";
        constexpr const char* BACKGROUND_NAME = "background";
        constexpr const char* CIRCLE_NAME = "circle";
        constexpr const char* BUS_NAME = "bus";
        constexpr const char* DOORS_CLOSED_NAME = "doors_closed";
        constexpr const char* DOORS_OPENED_NAME = "doors_opened";

        constexpr float ROUTE_WIDTH = 15.0f;
        const glm::vec3 ROUTE_COLOR{0.878f, 0.192f, 0.192f};

        constexpr float STATION_DIAMETER = 100.0f;
        constexpr unsigned int FONT_PIXEL_HEIGHT = 56;
        const glm::vec3 STATION_TEXT_COLOR{1.0f, 1.0f, 1.0f};

        constexpr float BUS_WIDTH = 110.0f;

        constexpr float HUD_MARGIN = 16.0f;       // screen pixels
        constexpr float HUD_DOORS_WIDTH = 128.0f; // screen pixels

        ResourceManager makeResources()
        {
            ResourceManager resources;
            resources.loadShader(SPRITE_SHADER_NAME, SPRITE_VERT_PATH, SPRITE_FRAG_PATH);
            resources.loadShader(SOLID_SHADER_NAME, SOLID_VERT_PATH, SOLID_FRAG_PATH);
            resources.loadShader(TEXT_SHADER_NAME, TEXT_VERT_PATH, TEXT_FRAG_PATH);
            resources.loadTexture(BACKGROUND_NAME, BACKGROUND_PATH);
            resources.loadTexture(CIRCLE_NAME, CIRCLE_PATH);
            resources.loadTexture(BUS_NAME, BUS_PATH);
            resources.loadTexture(DOORS_CLOSED_NAME, DOORS_CLOSED_PATH);
            resources.loadTexture(DOORS_OPENED_NAME, DOORS_OPENED_PATH);
            resources.loadFont(FONT_NAME, FONT_PATH, FONT_PIXEL_HEIGHT);
            return resources;
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
        : m_resources(makeResources()), m_camera(screenWidth, screenHeight),
          m_spritesRenderer(m_resources.shader(SPRITE_SHADER_NAME)),
          m_textRenderer(m_resources.shader(TEXT_SHADER_NAME), m_resources.font(FONT_NAME)),
          m_solidShader(m_resources.shader(SOLID_SHADER_NAME)), m_routeMesh(makeRouteMesh(route, ROUTE_WIDTH))
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        m_background.texture = &m_resources.texture(BACKGROUND_NAME);
        m_background.size = {simulation::WORLD_WIDTH, simulation::WORLD_HEIGHT};

        // The origin is the sprite's centre, so the circle is centred on the station.
        const Texture& circle = m_resources.texture(CIRCLE_NAME);
        for (std::size_t i = 0; i < m_stations.size(); ++i)
        {
            m_stations[i].texture = &circle;
            m_stations[i].position = route.stationPosition(i);
            m_stations[i].size = {STATION_DIAMETER, STATION_DIAMETER};
            m_stations[i].origin = {0.5f, 0.5f};
        }

        // Position, rotation and flips are set every frame in draw(). The origin is the centre, so the bus pivots there.
        const Texture& bus = m_resources.texture(BUS_NAME);
        m_bus.texture = &bus;
        m_bus.size = {BUS_WIDTH, BUS_WIDTH / bus.aspectRatio()};
        m_bus.origin = {0.5f, 0.5f};

        // Both HUD sprites share the top-left corner; draw() picks one by the bus state.
        const Texture& doorsClosed = m_resources.texture(DOORS_CLOSED_NAME);
        m_doorsClosed.texture = &doorsClosed;
        m_doorsClosed.position = {HUD_MARGIN, HUD_MARGIN};
        m_doorsClosed.size = {HUD_DOORS_WIDTH, HUD_DOORS_WIDTH / doorsClosed.aspectRatio()};

        const Texture& doorsOpened = m_resources.texture(DOORS_OPENED_NAME);
        m_doorsOpened.texture = &doorsOpened;
        m_doorsOpened.position = {HUD_MARGIN, HUD_MARGIN};
        m_doorsOpened.size = {HUD_DOORS_WIDTH, HUD_DOORS_WIDTH / doorsOpened.aspectRatio()};
    }

    void Renderer::draw(const simulation::Simulation& simulation)
    {
        const glm::mat4& projection = m_camera.projection();
    
        glClear(GL_COLOR_BUFFER_BIT);
        m_spritesRenderer.begin(projection);
        m_spritesRenderer.draw(m_background);

        drawRoute();

        m_spritesRenderer.begin(projection);
        for (const Sprite& station : m_stations)
        {
            m_spritesRenderer.draw(station);
        }

        m_textRenderer.begin(projection);
        for (std::size_t i = 0; i < m_stations.size(); ++i)
        {
            m_textRenderer.drawCentered(std::to_string(i), m_stations[i].position, STATION_TEXT_COLOR);
        }

        const simulation::Bus& bus = simulation.bus();
        m_bus.position = bus.position();
        m_bus.rotation = bus.heading();
        m_bus.flipX = true;
        m_bus.flipY = bus.isFacingLeft();

        m_spritesRenderer.begin(projection);
        m_spritesRenderer.draw(m_bus);

        m_spritesRenderer.begin(m_camera.screenProjection());
        if (bus.isMoving())
        {
            m_spritesRenderer.draw(m_doorsClosed);
        }
        else
        {
            m_spritesRenderer.draw(m_doorsOpened);
        }
    }

    void Renderer::drawRoute()
    {
        m_solidShader.use();
        m_solidShader.setMat4("uProjection", m_camera.projection());
        m_solidShader.setMat4("uModel", glm::mat4(1.0f));
        m_solidShader.setVec3("uColor", ROUTE_COLOR);

        m_routeMesh.draw();
    }

} // namespace nesmart::graphics
