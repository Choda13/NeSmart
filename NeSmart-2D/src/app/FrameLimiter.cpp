#include "FrameLimiter.hpp"
#include <algorithm>
#include <thread>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <timeapi.h>

#pragma comment(lib, "winmm.lib")

namespace nesmart::app
{

    namespace
    {
        constexpr UINT TIMER_RESOLUTION_MS = 1;
    }

    FrameLimiter::FrameLimiter(double targetFps, double maxDelta)
        : m_maxDelta(maxDelta), m_lastTick(Clock::now()), m_fpsWindowStart(m_lastTick)
    {
        timeBeginPeriod(TIMER_RESOLUTION_MS);
        setTargetFps(targetFps);
    }

    FrameLimiter::~FrameLimiter() { timeEndPeriod(TIMER_RESOLUTION_MS); }

    double FrameLimiter::tick()
    {
        if (m_frameTime > 0.0)
            waitUntil(m_lastTick +
                      std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(m_frameTime)));

        const auto now = Clock::now();
        m_delta = std::min(std::chrono::duration<double>(now - m_lastTick).count(), m_maxDelta);
        m_lastTick = now;

        // FPS counter, refreshed twice a second
        ++m_frameCount;
        const double window = std::chrono::duration<double>(now - m_fpsWindowStart).count();
        if (window >= 0.5)
        {
            m_fps = m_frameCount / window;
            m_frameCount = 0;
            m_fpsWindowStart = now;
        }
        return m_delta;
    }

    void FrameLimiter::waitUntil(Clock::time_point deadline)
    {
        constexpr auto spinMargin = std::chrono::milliseconds(2);
        if (deadline - Clock::now() > spinMargin)
            std::this_thread::sleep_until(deadline - spinMargin);
        while (Clock::now() < deadline)
            std::this_thread::yield();
    }

} // namespace nesmart::app