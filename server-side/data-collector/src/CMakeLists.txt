if(NOT DEFINED AGENT_TARGET)
    message(FATAL_ERROR "AGENT_TARGET is not set")
endif()

set(SOURCES
    main.cpp
    Utils.cpp
    MqttClient.cpp)

set(INCLUDES
    ${mosquitto_INCLUDES})

# mosquittoop.a depends on mosquitto.a, but comes after
# it in the libraries list, which makes a problem. we
# solve this by revesing the list.
list(APPEND MOSQ_LIBS_LIST ${mosquitto_LIBS})
list(REVERSE MOSQ_LIBS_LIST)

set(LIBS
    ${MOSQ_LIBS_LIST}
    ${OpenSSL_LIBS})

add_executable(${AGENT_TARGET} ${SOURCES})
target_include_directories(${AGENT_TARGET} INTERFACE ${INCLUDES})
target_link_libraries(${AGENT_TARGET} ${LIBS})
