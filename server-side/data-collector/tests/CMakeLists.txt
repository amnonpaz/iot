include(tests.cmake)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(${SRC_DIR})

add_unittest(NAME data_handler
             SOURCES
             DataHandlerTest.cpp
             ${SRC_DIR}/DataHandler.cpp)
