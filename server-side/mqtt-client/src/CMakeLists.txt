if(NOT DEFINED AGENT_TARGET)
    message(FATAL_ERROR "AGENT_TARGET is not set")
endif()

set(SOURCES
    main.cpp)

# TBD: set(LIBS)

add_executable(${AGENT_TARGET} ${SOURCES})
# TBD: target_link_libraries(${COLLECTOR_TARGET} ${LIBS})
