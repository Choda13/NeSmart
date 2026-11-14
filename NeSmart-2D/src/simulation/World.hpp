#pragma once

namespace nesmart::simulation
{
    // World space: units are background image pixels, origin at the top-left, x to the right, y down.
    // Everything (background, route, stations, bus) is positioned in these coordinates.
    //
    // The background covers the whole world. The safe area is the part that every screen must show;
    // screens with a different aspect ratio reveal extra background around it. Keep all gameplay
    // (route, stations) inside the safe area.
    constexpr float WORLD_WIDTH = 3000.0f;
    constexpr float WORLD_HEIGHT = 3000.0f;

    constexpr float SAFE_WIDTH = 2300.0f;
    constexpr float SAFE_HEIGHT = 1680.0f;

    // Center of the safe area, the point the camera looks at.
    constexpr float SAFE_CENTER_X = 1500.0f;
    constexpr float SAFE_CENTER_Y = 1450.0f;

} // namespace nesmart::simulation
