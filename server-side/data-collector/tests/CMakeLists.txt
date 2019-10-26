include(tests.cmake)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${SRC_DIR})

add_unittest(NAME data_handler
             SOURCES
             MessageHandlerTest.cpp
             ${SRC_DIR}/MessageHandler.cpp)
