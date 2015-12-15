# Uncompilable CMake File to enable project editing with CLion IDE

cmake_minimum_required(VERSION 2.8.4)
project(ofxMQTT)

include_directories(
    /Users/256dpi/Development/openframeworks/libs/openFrameworks
    /Users/256dpi/Development/openframeworks/libs/openFrameworks/app
    /Users/256dpi/Development/openframeworks/libs/openFrameworks/events
    /Users/256dpi/Development/openframeworks/libs/openFrameworks/types
    /Users/256dpi/Development/openframeworks/libs/openFrameworks/utils
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/ofxMQTT.cpp
    libs/mosquitto/src/config.h
    libs/mosquitto/src/dummypthread.h
    libs/mosquitto/src/logging_mosq.c
    libs/mosquitto/src/memory_mosq.c
    libs/mosquitto/src/messages_mosq.c
    libs/mosquitto/src/mosquitto.c
    libs/mosquitto/src/mosquitto_internal.h
    libs/mosquitto/src/mosquittopp.cpp
    libs/mosquitto/src/mqtt3_protocol.h
    libs/mosquitto/src/net_mosq.c
    libs/mosquitto/src/read_handle.c
    libs/mosquitto/src/read_handle_client.c
    libs/mosquitto/src/read_handle_shared.c
    libs/mosquitto/src/send_client_mosq.c
    libs/mosquitto/src/send_mosq.c
    libs/mosquitto/src/socks_mosq.c
    libs/mosquitto/src/srv_mosq.c
    libs/mosquitto/src/thread_mosq.c
    libs/mosquitto/src/time_mosq.c
    libs/mosquitto/src/tls_mosq.c
    libs/mosquitto/src/util_mosq.c
    libs/mosquitto/src/will_mosq.c
)

add_executable(ofxMQTT ${SOURCE_FILES})
