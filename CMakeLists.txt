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
        src/renderer/Shader.cpp
        src/renderer/Shader.hpp
        src/renderer/Uniform.hpp
        src/renderer/Vertex.hpp
        src/renderer/Window.hpp
        src/renderer/Meshes.cpp
        src/renderer/Uniform.cpp
        src/renderer/Vertex.cpp
        src/renderer/Window.cpp

        src/entity/Player.hpp
        src/entity/Player.cpp
        src/entity/Block.cpp
        src/entity/Block.hpp
        src/entity/Ball.cpp
        src/entity/Ball.hpp
)

include_directories(/usr/include/freetype2)

target_link_libraries(launcher SDL2 SDL2_image GL GLEW freetype)