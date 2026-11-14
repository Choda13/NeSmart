#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include <glm/vec2.hpp>

namespace nesmart::simulation
{
    // The closed bus route in world space (see World.hpp). The curve is built once in the constructor from
    // hardcoded waypoints and stored as a polyline; nothing is evaluated per frame. Positions are looked up by
    // distance travelled along the loop, which wraps around at the end.
    // Throws std::runtime_error if the hardcoded waypoint data is invalid.
    class Route
    {
    public:
        static constexpr std::size_t STATION_COUNT = 10;

        Route();

        // The sampled closed polyline (the last point connects back to the first).
        const std::vector<glm::vec2>& points() const { return m_points; }

        float totalLength() const { return m_cumulative.back(); }

        glm::vec2 stationPosition(std::size_t station) const { return m_stationPositions[station]; }

        // Distance along the loop, from the polyline start, at which the given station lies.
        float stationDistance(std::size_t station) const { return m_stationDistances[station]; }

        // Both wrap: a distance past totalLength() continues from the start of the loop.
        glm::vec2 positionAt(float distance) const;
        float headingAt(float distance) const; // radians, atan2 of the direction of travel (y points down)

    private:
        struct Location
        {
            std::size_t index; // polyline segment, from points()[index] to the next point
            float t;           // 0..1 along that segment
        };

        // Constructor steps, called in this order: each one relies on the data built by the previous ones.
        void sampleCurve();
        void buildCumulativeDistances();
        void placeStations();

        Location locate(float distance) const;

        std::vector<glm::vec2> m_points;
        std::vector<float> m_cumulative; // m_cumulative[i] is the distance at m_points[i]; the last is the total
        std::array<glm::vec2, STATION_COUNT> m_stationPositions{};
        std::array<float, STATION_COUNT> m_stationDistances{};
    };

} // namespace nesmart::simulation
