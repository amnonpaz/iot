#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <sstream>
#include <functional>

#define UNUSED(x) (void)(x)

namespace utils {

    template <typename T>
    T fromString(std::string str) {
        std::stringstream ss{str};
        T t;
        ss >> t;
        return t;
    }

    void TimedAsyncExecution(uint32_t timeoutMs,
                             std::function<void()>&& f);
}

#endif // UTILS_HP_
