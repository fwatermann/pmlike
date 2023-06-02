cmake_minimum_required(VERSION 3.26)
project(PM_Like)

set(CMAKE_CXX_STANDARD 20)


add_compile_definitions(GLFW_INCLUDE_NONE)

set(LIB_BINARY_DIR lib/bin/win32)
if (WIN32)
    set(LIB_BINARY_DIR lib/bin/win32)
elseif (UNIX)
    set(LIB_BINARY_DIR lib/bin/unix)
endif ()

find_library(LIBGLFW glfw3 PATHS ${LIB_BINARY_DIR} REQUIRED)

add_executable(PM_Like src/launcher.cpp lib/src/glad.c src/Game.cpp src/Game.hpp src/world/World.cpp src/world/World.hpp src/render/camera/Camera.cpp src/render/camera/Camera.hpp src/world/Chunk.cpp src/world/Chunk.hpp src/world/World.cpp src/world/World.hpp src/world/Chunk.cpp src/world/Chunk.hpp src/world/generator/ChunkGenerator.cpp src/world/generator/ChunkGenerator.hpp src/world/generator/TestGenerator.cpp src/world/generator/TestGenerator.hpp src/world/block/Block.cpp src/world/block/Block.hpp src/render/ShaderProgram.cpp src/render/ShaderProgram.hpp src/render/Shader.cpp src/render/Shader.hpp src/util/Log.hpp src/render/Font.cpp src/render/Font.hpp src/render/Texture.cpp src/render/Texture.hpp src/world/generator/NoiseGenerator.cpp src/world/generator/NoiseGenerator.hpp src/render/camera/Frustum.hpp)
target_link_libraries(PM_Like ${LIBGLFW})

target_include_directories(PM_Like SYSTEM PUBLIC lib/include)
target_include_directories(PM_Like PUBLIC src include)
