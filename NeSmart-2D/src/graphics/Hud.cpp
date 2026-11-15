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

        constexpr float HUD_INSPECTOR_WIDTH = 128.0f; // screen pixels

        // The passengers card sits in the top-right corner and the fines card right under it. Both images have the
        // same size and layout: an icon on the left, and the text starts right of it. The offsets are fractions of the
        // card size so they follow the card width.
        constexpr float HUD_CARD_WIDTH = 320.0f; // screen pixels
        constexpr float HUD_CARD_GAP = 8.0f;     // screen pixels between the two cards
        constexpr float CARD_TEXT_X = 0.40f;
        constexpr float CARD_TEXT_Y = 0.35f;
        constexpr float CARD_TEXT_SCALE = 0.75f;
        const glm::vec3 HUD_TEXT_COLOR{0.17f, 0.21f, 0.33f};

        // Top-left position of a card's text.
        glm::vec2 cardTextPosition(const Sprite& card)
        {
            return card.position + card.size * glm::vec2(CARD_TEXT_X, CARD_TEXT_Y);
        }
    } // namespace

    Hud::Hud(ResourceManager& resources, const Camera& camera, SpriteRenderer& spriteRenderer,
             TextRenderer& textRenderer, const simulation::Simulation& simulation)
        : m_simulation(simulation), m_camera(camera), m_spriteRenderer(spriteRenderer), m_textRenderer(textRenderer)
    {
        // Both door sprites share the top-left corner; draw() picks one by the bus state.
        const glm::vec2 position{HUD_MARGIN, HUD_MARGIN};
        m_doorsClosed = makeSprite(resources.texture("doors_closed"), HUD_DOORS_WIDTH, position, {0.0f, 0.0f});
        m_doorsOpened = makeSprite(resources.texture("doors_opened"), HUD_DOORS_WIDTH, position, {0.0f, 0.0f});

        // Bottom-left corner: the sprite origin is its top-left, so the height is needed for the y position.
        const Texture& inspector = resources.texture("inspection");
        const float inspectorHeight = HUD_INSPECTOR_WIDTH / inspector.aspectRatio();
        m_inspector = makeSprite(inspector, HUD_INSPECTOR_WIDTH,
                                 {HUD_MARGIN, m_camera.screenSize().y - HUD_MARGIN - inspectorHeight}, {0.0f, 0.0f});

        const glm::vec2 passengersPosition{m_camera.screenSize().x - HUD_MARGIN - HUD_CARD_WIDTH, HUD_MARGIN};
        m_passengersCard =
            makeSprite(resources.texture("passengers_hud"), HUD_CARD_WIDTH, passengersPosition, {0.0f, 0.0f});

        const glm::vec2 finesPosition = passengersPosition + glm::vec2(0.0f, m_passengersCard.size.y + HUD_CARD_GAP);
        m_finesCard = makeSprite(resources.texture("fines_hud"), HUD_CARD_WIDTH, finesPosition, {0.0f, 0.0f});
    }

    void Hud::draw()
    {
        const simulation::Bus& bus = m_simulation.bus();

        m_spriteRenderer.begin(m_camera.screenProjection());
        m_spriteRenderer.draw(bus.isMoving() ? m_doorsClosed : m_doorsOpened);
        m_spriteRenderer.draw(m_passengersCard);
        m_spriteRenderer.draw(m_finesCard);
        if (bus.hasInspector())
        {
            m_spriteRenderer.draw(m_inspector);
        }

        // Text goes after the sprites: it uses a different shader, so begin() is called again.
        const std::string passengers =
            std::to_string(bus.passengerCount()) + " / " + std::to_string(simulation::Bus::BUS_CAPACITY);
        const std::string fines = std::to_string(bus.finesTotal()); // the card's icon already says "FINE"

        m_textRenderer.begin(m_camera.screenProjection());
        m_textRenderer.drawAt(passengers, cardTextPosition(m_passengersCard), HUD_TEXT_COLOR, CARD_TEXT_SCALE);
        m_textRenderer.drawAt(fines, cardTextPosition(m_finesCard), HUD_TEXT_COLOR, CARD_TEXT_SCALE);
    }

} // namespace nesmart::graphics
