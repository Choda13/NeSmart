#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include <glm/vec2.hpp>

namespace nesmart::simulation
{
    class Route
    {
    public:
        static constexpr std::size_t STATION_COUNT = 10;

        Route();

        const std::vector<glm::vec2>& points() const { return m_points; }

        float totalLength() const { return m_cumulative.back(); }

        glm::vec2 stationPosition(std::size_t station) const { return m_stationPositions[station]; }

        float stationDistance(std::size_t station) const { return m_stationDistances[station]; }

        glm::vec2 positionAt(float distance) const;
        float headingAt(float distance) const;

    private:
        struct Location
        {
            std::size_t index;
            float t;
        };

        void sampleCurve();
        void buildCumulativeDistances();
        void placeStations();

        Location locate(float distance) const;

        std::vector<glm::vec2> m_points;
        std::vector<float> m_cumulative;
        std::array<glm::vec2, STATION_COUNT> m_stationPositions{};
        std::array<float, STATION_COUNT> m_stationDistances{};
    };

} // namespace nesmart::simulation
