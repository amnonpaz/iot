#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <sstream>

#define UNUSED(x) (void)(x)

namespace utils {

    template <typename T>
    T fromString(std::string str) {
        std::stringstream ss{str};
        T t;
        ss >> t;
        return t;
    }

}

#endif // UTILS_HP_
