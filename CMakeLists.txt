cmake_minimum_required(VERSION 3.16)
project(Miicraft)

# set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(Miicraft main.cpp
        src/wiimote.cpp
        include/wiimote.h
        src/world/block.cpp
        src/world/block.h
        src/world/verticalChunk.cpp
        src/world/verticalChunk.h
        src/world/world.cpp
        src/world/world.h
        src/engine/render/renderer.cpp
        src/engine/render/light.cpp
        src/matrix/matrix.cpp
        src/world/coord.h
        src/world/coord.h
        src/world/game.cpp
        src/world/game.h
        src/world/game.h
        src/world/WorldGenerator.cpp
        src/world/WorldGenerator.h
        src/world/FlatWorldGenerator.cpp
        src/world/FlatWorldGenerator.h
        src/world/FlatWorldGenerator.h
        include/utils/FastNoiseLite.h
        include/utils/FastNoiseLite.h
)



ogc_create_dol(Miicraft)
