#pragma once
#include "Timer.hpp"

namespace nesmart::app
{

    class FrameLimiter
    {
    public:
        using Clock = Timer::Clock;

        // targetFps <= 0 means unlimited
        explicit FrameLimiter(double targetFps = 60.0, double maxDelta = 0.25);
        ~FrameLimiter(); // restores the system timer resolution

        FrameLimiter(const FrameLimiter&) = delete;
        FrameLimiter& operator=(const FrameLimiter&) = delete;

        void setTargetFps(double fps) { m_frameTime = fps > 0.0 ? 1.0 / fps : 0.0; }

        // Call once per frame, at the end of the loop (after swapBuffers).
        // Waits until the frame budget is used up, then returns the delta time in seconds.
        double tick();

        double delta() const { return m_delta; }
        double fps() const { return m_fps; }

    private:
        static void waitUntil(Clock::time_point deadline);

        double m_frameTime = 0.0;
        double m_maxDelta;
        double m_delta = 0.0;
        double m_fps = 0.0;
        int m_frameCount = 0;
        Clock::time_point m_lastTick;
        Clock::time_point m_fpsWindowStart;
    };

} // namespace nesmart::app