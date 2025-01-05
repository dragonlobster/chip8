cmake_minimum_required(VERSION 3.28)
project(Chip8 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(SOURCES src/main.cpp src/Chip8.cpp src/DrawUtils.cpp src/tinyfiledialogs.cpp src/Chip8.h src/DrawUtils.H src/HandleInput.h src/tinyfiledialogs.h)
add_executable(Chip8 ${SOURCES})

target_link_libraries(Chip8 PRIVATE SFML::Graphics PRIVATE SFML::Audio)
