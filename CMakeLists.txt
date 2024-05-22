cmake_minimum_required(VERSION 3.22)
project(QSynthi2)

set(CMAKE_CXX_STANDARD 20)

# setup guide:
# https://www.youtube.com/watch?v=Uq7Hwt18s3s
# https://github.com/JanWilczek/audio-plugin-template

# I like to download the dependencies to the same folder as the project.
# If you want to install them system wide, set CPM_SOURCE_CACHE with the path to the dependencies
# either as an environment variable or pass it to the cmake script with -DCPM_SOURCE_CACHE=<path>.

IF (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    # save release lib builds in cmake-build-release[...] folder
    set(LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs)
ELSE()
    set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
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

# This command allows running tests from the "build" folder (the one where CMake generates the project to).
enable_testing()


if(MSVC)
# Enables all warnings and treats warnings as errors.
# This needs to be added AFTER all the third-party dependencies.
#    add_compile_options(/Wall /WX)

    # optimizations
    IF (${CMAKE_BUILD_TYPE} STREQUAL "Release")
#        add_compile_options(/Ox)
    ENDIF()
else()
#    add_compile_options(-Wall -Wextra -Wpedantic)

    # optimizations TODO

    add_compile_options(
            -O0
            -march=native       # vectorization
    )

    IF (${CMAKE_BUILD_TYPE} STREQUAL "Release")
#        add_compile_options(-O3)
    ELSE ()
#        add_compile_options(-O0)
    ENDIF()
endif()


# Adds all the targets configured in the "plugin" folder.
add_subdirectory(plugin)

# Adds all the targets configured in the "test" folder.
add_subdirectory(test)
