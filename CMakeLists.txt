cmake_minimum_required(VERSION 3.26)
project(Miicraft)

# set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(Miicraft main.cpp
        src/wiimote.cpp
        src/engine/render/renderer.cpp
        src/engine/render/light.cpp
        src/matrix/matrix.cpp
)



ogc_create_dol(Miicraft)
