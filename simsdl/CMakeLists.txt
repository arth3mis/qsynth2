cmake_minimum_required(VERSION 3.22)

project(SimSDL)

# Adds SDL2.
#CPMAddPackage(
#        NAME SDL2
#        VERSION 2.0.20
#        URL https://libsdl.org/release/SDL2-2.0.20.zip
#        SOURCE_DIR ${LIB_DIR}/sdl2
#)
#include_directories(${LIB_DIR}/sdl2/include)

# use system-installed SDL instead of CPM:
find_package(SDL2 REQUIRED SDL2)

# Creates the test console application.
add_executable(${PROJECT_NAME}
        src/main.cpp
)

# Sets the necessary include directories.
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../plugin/include
        ${JUCE_SOURCE_DIR}/modules)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        AudioPlugin
        SDL2)
