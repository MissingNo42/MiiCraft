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
        src/world/blockTemplate.cpp
        src/system/saveManager.cpp
        src/world/BiomeMap.cpp
        src/system/Random.cpp
        src/system/Random.h
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
        src/world/blockTemplate.cpp
        src/world/BiomeMap.cpp
        src/system/Random.cpp
        src/system/Random.h
)

#add_compile_options(-lfat)
target_link_libraries(Miicraft fat)

ogc_create_dol(Miicraft)
