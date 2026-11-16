#include "graphics/Hud.hpp"

#include <string>

#include <glm/glm.hpp>

#include "simulation/Bus.hpp"

namespace nesmart::graphics
{
    namespace
    {
        constexpr float HUD_MARGIN = 16.0f; 
        constexpr float HUD_INSPECTOR_WIDTH = 128.0f;

        constexpr float HUD_CARD_WIDTH = 320.0f;
        constexpr float HUD_CARD_GAP = 8.0f;
        const glm::vec2 CARD_TEXT_OFFSET{128.0f, 35.0f};
        constexpr float CARD_TEXT_SCALE = 0.75f;
        const glm::vec3 HUD_TEXT_COLOR{0.17f, 0.21f, 0.33f};

        const glm::vec2 DOORS_LABEL_OFFSET{160.0f, 20.0f};
        constexpr float DOORS_LABEL_SCALE = 0.4f;
        const glm::vec2 DOORS_STATE_OFFSET{160.0f, 45.0f};
        constexpr float DOORS_STATE_SCALE = 0.5f;

        const glm::vec3 DOORS_LABEL_COLOR{0.42f, 0.47f, 0.58f};
        const glm::vec3 DOORS_OPENED_COLOR{0.13f, 0.45f, 0.24f};

        const std::string AUTHOR_TEXT = "Danilo Kovačević RA136/2022";
        constexpr float AUTHOR_SCALE = 0.4f;
    } // namespace

    Hud::Hud(ResourceManager& resources, const Camera& camera, SpriteRenderer& spriteRenderer,
             TextRenderer& textRenderer, const simulation::Simulation& simulation)
        : m_simulation(simulation), m_camera(camera), m_spriteRenderer(spriteRenderer), m_textRenderer(textRenderer)
    {
        const glm::vec2 position{HUD_MARGIN, HUD_MARGIN};
        m_doorsClosed = makeSprite(resources.texture("doors_closed"), HUD_CARD_WIDTH, position, {0.0f, 0.0f});
        m_doorsOpened = makeSprite(resources.texture("doors_opened"), HUD_CARD_WIDTH, position, {0.0f, 0.0f});

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
        const Sprite& doorsCard = bus.isMoving() ? m_doorsClosed : m_doorsOpened;
        m_spriteRenderer.draw(doorsCard);
        m_spriteRenderer.draw(m_passengersCard);
        m_spriteRenderer.draw(m_finesCard);
        if (bus.hasInspector())
        {
            m_spriteRenderer.draw(m_inspector);
        }

        const std::string passengers =
            std::to_string(bus.passengerCount()) + " / " + std::to_string(simulation::Bus::BUS_CAPACITY);
        const std::string fines = std::to_string(bus.finesTotal());

        m_textRenderer.begin(m_camera.screenProjection());
        m_textRenderer.drawAt("Doors", doorsCard.position + DOORS_LABEL_OFFSET, DOORS_LABEL_COLOR, DOORS_LABEL_SCALE);
        m_textRenderer.drawAt(bus.isMoving() ? "Closed" : "Opened", doorsCard.position + DOORS_STATE_OFFSET,
                              bus.isMoving() ? HUD_TEXT_COLOR : DOORS_OPENED_COLOR, DOORS_STATE_SCALE);
        m_textRenderer.drawAt(passengers, m_passengersCard.position + CARD_TEXT_OFFSET, HUD_TEXT_COLOR,
                              CARD_TEXT_SCALE);
        m_textRenderer.drawAt(fines, m_finesCard.position + CARD_TEXT_OFFSET, HUD_TEXT_COLOR, CARD_TEXT_SCALE);

        const glm::vec2 authorPosition{
            m_camera.screenSize().x - HUD_MARGIN - m_textRenderer.width(AUTHOR_TEXT, AUTHOR_SCALE),
            m_camera.screenSize().y - HUD_MARGIN - m_textRenderer.height(AUTHOR_SCALE)};
        m_textRenderer.drawAt(AUTHOR_TEXT, authorPosition, HUD_TEXT_COLOR, AUTHOR_SCALE);
    }

} // namespace nesmart::graphics
