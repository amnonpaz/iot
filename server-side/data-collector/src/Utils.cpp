#include "Utils.hpp"

#include <thread>

namespace utils {

    void TimedAsyncExecution(uint32_t timeoutMs,
                             std::function<void()> &&func) {
        auto th = std::thread([=](){
            std::this_thread::sleep_for(std::chrono::milliseconds(timeoutMs));
            func();
        });

        th.detach();
    }

} // utils
