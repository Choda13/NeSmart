#include "Timer.hpp"
#include <thread>

namespace nesmart::app {

    void Timer::sleep(double seconds) {
        if (seconds > 0)
            std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }

} // namespace nesmart::app