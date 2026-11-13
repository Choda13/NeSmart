#pragma once
#include <chrono>

namespace nesmart::app {

    class Timer {
    public:
        using Clock = std::chrono::steady_clock;

        Timer() : m_start(Clock::now()) {}

        void reset() { m_start = Clock::now(); }

        double elapsed() const { return std::chrono::duration<double>(Clock::now() - m_start).count(); }

        static double now() { return std::chrono::duration<double>(Clock::now().time_since_epoch()).count(); }

        static void sleep(double seconds);

    private:
        Clock::time_point m_start;
    };

} // namespace nesmart::app