#include "graphics/Renderer.hpp"

#include <array>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

        // Thickness of the drawn route line, in world units.
        constexpr float ROUTE_WIDTH = 15.0f;
        const glm::vec3 ROUTE_COLOR{0.878f, 0.192f, 0.192f};

        // Station marker size in world units, and the font's pixel height (also world units, see drawTextCentered).
        constexpr float STATION_DIAMETER = 100.0f;
        constexpr unsigned int FONT_PIXEL_HEIGHT = 56;
        const glm::vec3 STATION_TEXT_COLOR{1.0f, 1.0f, 1.0f};

        std::array<glm::vec2, simulation::Route::STATION_COUNT> copyStationPositions(const simulation::Route& route)
        {
            std::array<glm::vec2, simulation::Route::STATION_COUNT> positions{};
            for (std::size_t i = 0; i < positions.size(); ++i)
            {
                positions[i] = route.stationPosition(i);
            }
            return positions;
        }

        // Square from (0, 0) to (1, 1) in y-down space, drawn as a triangle strip.
        // STs are flipped vertically because Texture flips images on load (ST (0, 0) is the image's bottom-left).
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
        , m_textShader(TEXT_VERT_PATH, TEXT_FRAG_PATH)
        , m_camera(screenWidth, screenHeight)
        , m_quadMesh(makeUnitQuad())
        , m_routeMesh(makeRouteMesh(route, ROUTE_WIDTH))
        , m_backgroundTex(BACKGROUND_PATH)
        , m_circleTex(CIRCLE_PATH)
        , m_font(FONT_PATH, FONT_PIXEL_HEIGHT)
        , m_stationPositions(copyStationPositions(route))
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
        drawStations();
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

        m_backgroundTex.bind(0);
        m_quadMesh.draw();
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

    void Renderer::drawStations()
    {
        // Markers first, then every number on top of them.
        m_spriteShader.use();
        m_spriteShader.setMat4("uProjection", m_camera.projection());
        m_spriteShader.setInt("uTexture", 0);
        m_circleTex.bind(0);

        for (const glm::vec2& position : m_stationPositions)
        {
            // The unit quad's origin is its top-left corner, so shift by half the size to centre it on the station.
            const glm::mat4 model =
                glm::translate(glm::mat4(1.0f), glm::vec3(position - glm::vec2(STATION_DIAMETER * 0.5f), 0.0f)) *
                glm::scale(glm::mat4(1.0f), glm::vec3(STATION_DIAMETER, STATION_DIAMETER, 1.0f));
            m_spriteShader.setMat4("uModel", model);
            m_quadMesh.draw();
        }

        for (std::size_t i = 0; i < m_stationPositions.size(); ++i)
        {
            drawTextCentered(std::to_string(i), m_stationPositions[i], STATION_TEXT_COLOR);
        }
    }

    

    void Renderer::drawTextCentered(const std::string& text, glm::vec2 center, const glm::vec3& color)
    {
        m_textShader.use();
        m_textShader.setMat4("uProjection", m_camera.projection());
        m_textShader.setInt("uTexture", 0);
        m_textShader.setVec3("uColor", color);
        m_font.bind(0);

        // Glyph sizes are in pixels but placed in world units, so one font pixel is one world unit.
        // Vertical centring uses the digit height, which suits numbers (the world is y-down, so the baseline is lower).
        const float baseline = center.y + m_font.glyph('0').bearing.y * 0.5f;
        float penX = center.x - m_font.width(text) * 0.5f;

        for (const char c : text)
        {
            const Font::Glyph& glyph = m_font.glyph(c);

            const glm::vec2 topLeft{penX + glyph.bearing.x, baseline - glyph.bearing.y};
            const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(topLeft, 0.0f)) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(glyph.size, 1.0f));

            m_textShader.setMat4("uModel", model);
            m_textShader.setVec2("uUVOffset", glyph.uvOffset);
            m_textShader.setVec2("uUVSize", glyph.uvSize);
            m_quadMesh.draw();

            penX += glyph.advance;
        }
    }

} // namespace nesmart::graphics
