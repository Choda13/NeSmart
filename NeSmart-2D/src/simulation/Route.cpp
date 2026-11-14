#include "simulation/Route.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include <glm/glm.hpp>

namespace
{
    const std::vector<glm::vec2> WAYPOINTS = {
        {1342.0f, 2002.0f}, {1398.0f, 2003.0f}, {1531.0f, 1991.0f}, {1711.0f, 1983.0f}, {1857.0f, 2098.0f},
        {1961.0f, 2141.0f}, {2046.0f, 2113.0f}, {2165.0f, 2044.0f}, {2223.0f, 2010.0f}, {2281.0f, 1987.0f},
        {2315.0f, 1949.0f}, {2352.0f, 1887.0f}, {2379.0f, 1840.0f}, {2398.0f, 1805.0f}, {2441.0f, 1684.0f},
        {2473.0f, 1582.0f}, {2490.0f, 1495.0f}, {2485.0f, 1424.0f}, {2472.0f, 1357.0f}, {2427.0f, 1257.0f},
        {2333.0f, 1112.0f}, {2273.0f, 1060.0f}, {2188.0f, 1011.0f}, {2142.0f, 991.0f},  {2087.0f, 949.0f},
        {2042.0f, 911.0f},  {1958.0f, 854.0f},  {1829.0f, 772.0f},  {1727.0f, 759.0f},  {1640.0f, 785.0f},
        {1578.0f, 833.0f},  {1501.0f, 963.0f},  {1419.0f, 1075.0f}, {1371.0f, 1149.0f}, {1330.0f, 1194.0f},
        {1290.0f, 1226.0f}, {1215.0f, 1233.0f}, {1101.0f, 1233.0f}, {976.0f, 1211.0f},  {786.0f, 1216.0f},
        {724.0f, 1249.0f},  {694.0f, 1302.0f},  {687.0f, 1359.0f},  {688.0f, 1403.0f},  {668.0f, 1489.0f},
        {609.0f, 1559.0f},  {537.0f, 1612.0f},  {483.0f, 1671.0f},  {450.0f, 1731.0f},  {461.0f, 1746.0f},
        {475.0f, 1760.0f},  {513.0f, 1792.0f},  {555.0f, 1842.0f},  {582.0f, 1912.0f},  {594.0f, 1993.0f},
        {627.0f, 2056.0f},  {670.0f, 2094.0f},  {706.0f, 2129.0f},  {764.0f, 2158.0f},  {855.0f, 2149.0f},
        {913.0f, 2145.0f},  {969.0f, 2105.0f},  {1019.0f, 2052.0f}, {1086.0f, 2005.0f}, {1205.0f, 1995.0f},
        {1297.0f, 1993.0f}, {1323.0f, 1994.0f},
    };

    const std::array<std::size_t, 10> STATION_WAYPOINTS = {5, 13, 15, 19, 22, 29, 34, 39, 51, 66};

    // Points sampled on each span between two neighbouring waypoints.
    constexpr std::size_t SAMPLES_PER_SPAN = 16;

    // Uniform Catmull-Rom: the curve passes through p1 (t = 0) and p2 (t = 1).
    glm::vec2 catmullRom(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, float t)
    {
        const float t2 = t * t;
        const float t3 = t2 * t;
        return 0.5f * ((2.0f * p1) + (p2 - p0) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                       (3.0f * p1 - p0 - 3.0f * p2 + p3) * t3);
    }
} // namespace

namespace nesmart::simulation
{
    static_assert(STATION_WAYPOINTS.size() == Route::STATION_COUNT, "STATION_WAYPOINTS must hold one entry per station");

    Route::Route()
    {
        const std::size_t waypointCount = WAYPOINTS.size();

        // Validate path data
        if (waypointCount < 4)
        {
            throw std::runtime_error("Route: at least 4 waypoints are required");
        }
        for (std::size_t i = 0; i < STATION_COUNT; ++i)
        {
            const bool outOfRange = STATION_WAYPOINTS[i] >= waypointCount;
            const bool unordered = i > 0 && STATION_WAYPOINTS[i] <= STATION_WAYPOINTS[i - 1];
            if (outOfRange || unordered)
            {
                throw std::runtime_error("Route: station waypoints must be valid and in increasing order");
            }
        }

        sampleCurve();
        buildCumulativeDistances();
        placeStations();
    }

    void Route::sampleCurve()
    {
        const std::size_t waypointCount = WAYPOINTS.size();

        m_points.reserve(waypointCount * SAMPLES_PER_SPAN);
        for (std::size_t i = 0; i < waypointCount; ++i)
        {
            const glm::vec2& p0 = WAYPOINTS[(i + waypointCount - 1) % waypointCount];
            const glm::vec2& p1 = WAYPOINTS[i];
            const glm::vec2& p2 = WAYPOINTS[(i + 1) % waypointCount];
            const glm::vec2& p3 = WAYPOINTS[(i + 2) % waypointCount];
            for (std::size_t s = 0; s < SAMPLES_PER_SPAN; ++s)
            {
                const float t = static_cast<float>(s) / static_cast<float>(SAMPLES_PER_SPAN);
                m_points.push_back(catmullRom(p0, p1, p2, p3, t));
            }
        }
    }

    void Route::buildCumulativeDistances()
    {
        const std::size_t pointCount = m_points.size();

        m_cumulative.assign(pointCount + 1, 0.0f);
        for (std::size_t i = 0; i < pointCount; ++i)
        {
            m_cumulative[i + 1] = m_cumulative[i] + glm::distance(m_points[i], m_points[(i + 1) % pointCount]);
        }
    }

    void Route::placeStations()
    {
        for (std::size_t i = 0; i < STATION_COUNT; ++i)
        {
            const std::size_t sample = STATION_WAYPOINTS[i] * SAMPLES_PER_SPAN;
            m_stationPositions[i] = m_points[sample];
            m_stationDistances[i] = m_cumulative[sample];
        }
    }

    glm::vec2 Route::positionAt(float distance) const
    {
        const Location location = locate(distance);
        const glm::vec2& from = m_points[location.index];
        const glm::vec2& to = m_points[(location.index + 1) % m_points.size()];
        return glm::mix(from, to, location.t);
    }

    float Route::headingAt(float distance) const
    {
        const Location location = locate(distance);
        const glm::vec2 direction = m_points[(location.index + 1) % m_points.size()] - m_points[location.index];
        return std::atan2(direction.y, direction.x);
    }

    Route::Location Route::locate(float distance) const
    {
        const float total = totalLength();
        float wrapped = std::fmod(distance, total);
        if (wrapped < 0.0f)
        {
            wrapped += total;
        }

        // The first cumulative entry greater than the distance, minus one, is the segment that contains it.
        const auto next = std::upper_bound(m_cumulative.begin(), m_cumulative.end(), wrapped);
        const std::size_t index =
            std::min(static_cast<std::size_t>(next - m_cumulative.begin()) - 1, m_points.size() - 1);

        const float segmentLength = m_cumulative[index + 1] - m_cumulative[index];
        const float t = segmentLength > 0.0f ? (wrapped - m_cumulative[index]) / segmentLength : 0.0f;
        return {index, t};
    }
} // namespace nesmart::simulation