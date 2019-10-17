cmake_minimum_required(VERSION 3.10)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(mosquitto REQUIRED)
find_package(OpenSSL REQUIRED)

add_compile_options("-Wall" "-Wextra" "-Werror")

add_subdirectory(src)

if(DEFINED TESTING)
    add_subdirectory(tests)
endif()
