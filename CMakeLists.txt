cmake_minimum_required(VERSION 3.16)
project(Miicraft)
project(MiicraftTest)

enable_language(ASM)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(assets)
include_directories(src)
include_directories(include/engine)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wpedantic)
add_compile_options(-Winline)
#add_compile_options(-Ofast)


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
        src/system/Random.cpp
        src/player.cpp
        src/engine/render/cache.cpp
        src/engine/render/render.cpp
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
        src/system/Random.cpp
)



include(ExternalProject)


function(add_binfile_library target_name)
    if (NOT ${ARGC} GREATER 1)
        message(FATAL_ERROR "add_binfile_library : Argument error (no input files)")
    endif()

    get_cmake_property(_enabled_languages ENABLED_LANGUAGES)
    if (NOT _enabled_languages MATCHES ".*ASM.*")
        message(FATAL_ERROR "add_binfile_library : ASM language needs to be enabled")
    endif()

    set(_output_dir ${CMAKE_CURRENT_BINARY_DIR}/binfile_asm)
    set(_output_file ${_output_dir}/${target_name}.s)

    file(MAKE_DIRECTORY ${_output_dir})

    add_custom_command(OUTPUT ${_output_file}
            COMMAND /opt/devkitpro/tools/bin/bin2s ${target_name} > ${_output_file}
            DEPENDS ${ARGN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_library(${target_name} ${_output_file})
endfunction()

#add_binfile_library(assets assets/test.mp3)

#target_link_libraries(Miicraft assets)

#add_compile_options(-lfat)
target_link_libraries(Miicraft fat)

target_link_libraries(Miicraft mad )

target_link_libraries(Miicraft asnd)

ogc_create_dol(Miicraft)
