cmake_minimum_required(VERSION 3.16)
project(Miicraft)
project(MiicraftTest)

# set(CMAKE_CXX_STANDARD 23)

include_directories(include)
include_directories(assets)

add_executable(Miicraft main.cpp
        src/wiimote.cpp
        src/world/block.cpp
        src/world/verticalChunk.cpp
        src/world/horizontalChunck.cpp
        src/world/world.cpp
        src/engine/render/renderer.cpp
        src/engine/render/light.cpp
        src/matrix/matrix.cpp
        src/engine/render/bloc.cpp
        src/engine/render/camera.cpp
        src/world/game.cpp
        src/world/WorldGenerator.cpp
        src/world/FlatWorldGenerator.cpp
        src/world/PerlinWorldGenerator.cpp
        src/world/coord.cpp
)

add_executable(MiicraftTest Debug.cpp
        src/world/block.cpp
        src/world/verticalChunk.cpp
        src/world/world.cpp
        src/world/game.cpp
        src/world/WorldGenerator.cpp
        src/world/FlatWorldGenerator.cpp
        src/world/PerlinWorldGenerator.cpp
        src/world/coord.cpp
)



ogc_create_dol(Miicraft)
