cmake_minimum_required(VERSION 3.22)
project(QSynthi2)

set(CMAKE_CXX_STANDARD 20)

# setup guide:
# https://www.youtube.com/watch?v=Uq7Hwt18s3s
# https://github.com/JanWilczek/audio-plugin-template

# I like to download the dependencies to the same folder as the project.
# If you want to install them system wide, set CPM_SOURCE_CACHE with the path to the dependencies
# either as an environment variable or pass it to the cmake script with -DCPM_SOURCE_CACHE=<path>.

IF (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs-debug)
ELSE()
    set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs-release)
ENDIF()

# Downloads CPM if not already downloaded. CPM is an easy-to-use package manager nicely integrated with CMake.
include(cmake/get_cpm.cmake)

# This commands downloads AND configures JUCE. It sets up some variables, like JUCE_SOURCE_DIR.
CPMAddPackage(
        NAME JUCE
        GIT_TAG 7.0.12
        VERSION 7.0.12
        GITHUB_REPOSITORY juce-framework/JUCE
        SOURCE_DIR ${LIB_DIR}/juce
)

# Adds googletest.
CPMAddPackage(
        NAME GOOGLETEST
        GITHUB_REPOSITORY google/googletest
        GIT_TAG v1.13.0
        VERSION 1.13.0
        SOURCE_DIR ${LIB_DIR}/googletest
        OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
)

# Adds pocketfft
CPMAddPackage(
        NAME PocketFFT
        GIT_TAG b557a35
        GITHUB_REPOSITORY mreineck/pocketfft
        SOURCE_DIR ${LIB_DIR}/pocketfft
)
include_directories(${LIB_DIR}/pocketfft)

# Adds Eigen
CPMAddPackage(
        NAME Eigen
        GIT_TAG 3.4.0
        VERSION 3.4.0
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen
        DOWNLOAD_ONLY true
        SOURCE_DIR ${LIB_DIR}/eigen
)
include_directories(${LIB_DIR}/eigen)

# Adds OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# (not working)
#CPMAddPackage(
#        NAME OpenCV
#        GIT_TAG 4.10.0
#        VERSION 4.10.0
#        GITHUB_REPOSITORY opencv/opencv
#        SOURCE_DIR ${LIB_DIR}/opencv
#)
#include_directories(${LIB_DIR}/opencv)

# This command allows running tests from the "build" folder (the one where CMake generates the project to).
enable_testing()


IF(MSVC)
    #
ELSE()
    IF (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_compile_options(
                -O0
                -march=native       # vectorization
        )
    ENDIF()
ENDIF()


# Adds all the targets configured in the "plugin" folder.
add_subdirectory(plugin)

# Adds all the targets configured in the "test" folder.
add_subdirectory(test)
