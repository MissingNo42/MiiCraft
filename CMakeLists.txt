cmake_minimum_required(VERSION 3.22)
project(Miicraft)

# set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(Miicraft main.cpp
        src/render.cpp
        src/wiimote.cpp
        include/wiimote.h
        src/world/block.cpp
        src/world/block.h
        src/world/verticalChunk.cpp
        src/world/verticalChunk.h
        src/world/horizontalChunck.cpp
        src/world/horizontalChunck.h
        src/world/world.cpp
        src/world/world.h
)



ogc_create_dol(Miicraft)
