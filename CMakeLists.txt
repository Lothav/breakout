cmake_minimum_required(VERSION 2.8.4)
project(breakout)

add_executable(
        launcher

        src/main.cpp
        src/memory/Allocator.hpp
        src/memory/Provider.cpp
        src/memory/Provider.hpp
        src/memory/Pool.cpp
        src/memory/Pool.hpp
        src/renderer/Meshes.hpp
        src/renderer/Player.hpp
        src/renderer/Shader.cpp
        src/renderer/Shader.hpp
        src/renderer/Uniform.hpp
        src/renderer/Vertex.hpp
        src/renderer/Window.hpp

        src/renderer/Block.cpp src/renderer/Block.hpp)
target_link_libraries(launcher SDL2 SDL2_image GLESv2)