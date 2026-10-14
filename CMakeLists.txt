cmake_minimum_required(VERSION 3.21)

project(chip8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chip8
    src/main.cpp
    src/Chip8.cpp
    src/SDLApp.cpp
)

find_package(SDL3 REQUIRED)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)

if(MSVC)
    target_compile_options(chip8 PRIVATE /W4)
else()
    target_compile_options(chip8 PRIVATE -Wall -Wextra -Wpedantic)
endif()
