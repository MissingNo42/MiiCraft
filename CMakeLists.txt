cmake_minimum_required(VERSION 3.16)
project(Miicraft)

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
)



ogc_create_dol(Miicraft)
