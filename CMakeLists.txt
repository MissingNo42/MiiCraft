cmake_minimum_required(VERSION 3.26)
project(Miicraft)

# set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(Miicraft main.cpp
        src/render.cpp
        src/wiimote.cpp
        include/wiimote.h)



ogc_create_dol(Miicraft)
