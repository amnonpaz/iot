find_package(gtest REQUIRED)

function(add_unittest)
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES LIBS)
    cmake_parse_arguments(UNITTEST "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    set(TARGET_NAME ${UNITTEST_NAME}_unittest)

    add_executable(${TARGET_NAME} ${UNITTEST_SOURCES})
    target_link_libraries(${TARGET_NAME} ${gtest_LIBS} ${LIBS})
endfunction()
