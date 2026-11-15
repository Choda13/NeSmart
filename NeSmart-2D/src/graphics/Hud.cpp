#include "graphics/Hud.hpp"

#include <string>

#include <glm/glm.hpp>

#include "simulation/Bus.hpp"

namespace nesmart::graphics
{
    namespace
    {
        constexpr float HUD_MARGIN = 16.0f;       // screen pixels
        constexpr float HUD_DOORS_WIDTH = 128.0f; // screen pixels

        // The passengers card sits in the top-right corner. The text starts right of the icon baked into the image;
        // the offsets are fractions of the card size so they follow the card width.
        constexpr float HUD_PASSENGERS_WIDTH = 320.0f; // screen pixels
        constexpr float PASSENGERS_TEXT_X = 0.40f;
        constexpr float PASSENGERS_TEXT_Y = 0.35f;
        constexpr float PASSENGERS_TEXT_SCALE = 0.75f;
        const glm::vec3 HUD_TEXT_COLOR{0.17f, 0.21f, 0.33f};
    } // namespace

    Hud::Hud(ResourceManager& resources, const Camera& camera, SpriteRenderer& spriteRenderer,
             TextRenderer& textRenderer, const simulation::Simulation& simulation)
        : m_simulation(simulation), m_camera(camera), m_spriteRenderer(spriteRenderer), m_textRenderer(textRenderer)
    {
        // Both door sprites share the top-left corner; draw() picks one by the bus state.
        const glm::vec2 position{HUD_MARGIN, HUD_MARGIN};
        m_doorsClosed = makeSprite(resources.texture("doors_closed"), HUD_DOORS_WIDTH, position, {0.0f, 0.0f});
        m_doorsOpened = makeSprite(resources.texture("doors_opened"), HUD_DOORS_WIDTH, position, {0.0f, 0.0f});

        const glm::vec2 passengersPosition{m_camera.screenSize().x - HUD_MARGIN - HUD_PASSENGERS_WIDTH, HUD_MARGIN};
        m_passengersCard = makeSprite(resources.texture("passengers_hud"), HUD_PASSENGERS_WIDTH, passengersPosition,
                                      {0.0f, 0.0f});
    }

    void Hud::draw()
    {
        const simulation::Bus& bus = m_simulation.bus();

        m_spriteRenderer.begin(m_camera.screenProjection());
        m_spriteRenderer.draw(bus.isMoving() ? m_doorsClosed : m_doorsOpened);
        m_spriteRenderer.draw(m_passengersCard);

        // Text goes after the sprites: it uses a different shader, so begin() is called again.
        const std::string passengers =
            std::to_string(bus.passengerCount()) + " / " + std::to_string(simulation::Bus::BUS_CAPACITY);
        const glm::vec2 textPosition = m_passengersCard.position + m_passengersCard.size * glm::vec2(PASSENGERS_TEXT_X,
                                                                                                    PASSENGERS_TEXT_Y);

        m_textRenderer.begin(m_camera.screenProjection());
        m_textRenderer.drawAt(passengers, textPosition, HUD_TEXT_COLOR, PASSENGERS_TEXT_SCALE);
    }

} // namespace nesmart::graphics
