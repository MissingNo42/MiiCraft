cmake_minimum_required(VERSION 3.22)
project(Miicraft)

# set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(Miicraft main.cpp
        src/render.cpp
        src/wiimote.cpp
        src/engine/render/renderer.cpp
        src/engine/render/light.cpp
        src/matrix/matrix.cpp
)



ogc_create_dol(Miicraft)
