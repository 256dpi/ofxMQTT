# Uncompilable CMake File to enable project editing with CLion IDE

cmake_minimum_required(VERSION 2.8.4)
project(ofxMQTT)

include_directories(
    /Users/256dpi/Development/openframeworks/libs/openFrameworks
    /Users/256dpi/Development/openframeworks/libs/openFrameworks/app
    /Users/256dpi/Development/openframeworks/libs/openFrameworks/events
    /Users/256dpi/Development/openframeworks/libs/openFrameworks/types
    /Users/256dpi/Development/openframeworks/libs/openFrameworks/utils
    libs/mosquitto/src/
    src/
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/ofxMQTT.cpp
        example-ofxMQTT/src/main.cpp
        example-ofxMQTT/src/ofApp.cpp
        example-ofxMQTT/src/ofApp.h)

add_executable(ofxMQTT ${SOURCE_FILES})
