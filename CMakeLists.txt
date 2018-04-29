cmake_minimum_required(VERSION 2.8.4)
project(breakout)

add_executable(launcher
    src/memory/Allocator.hpp
    src/memory/Provider.cpp
    src/memory/Provider.hpp
    src/memory/Pool.cpp
    src/memory/Pool.hpp
)
target_link_libraries(launcher SDL2 SDL2_image GLESv2)