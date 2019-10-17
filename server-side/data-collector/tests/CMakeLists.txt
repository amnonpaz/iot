find_package(gtest REQUIRED)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${SRC_DIR})

add_executable(data_handler_unittest
               DataHandlerTest.cpp
               ${SRC_DIR}/DataHandler.cpp)
target_link_libraries(data_handler_unittest ${gtest_LIBS})
