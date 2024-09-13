cmake_minimum_required(VERSION 3.16)
project(Miicraft)
project(MiicraftTest)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(assets)
include_directories(src)
include_directories(include/engine)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wpedantic)
add_compile_options(-Winline)
add_compile_options(-g)
#add_compile_options(-Ofast)


# Compile the texture from PNG to TPL to C raw data
message(STATUS "Compiling TPL texture...")
execute_process(
    COMMAND ${DEVKITPRO}/tools/bin/gxtexconv -i assets/texture.png colfmt=6 mipmap=no
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE result
    ERROR_VARIABLE error
)

if(result)
    message(FATAL_ERROR "Failed to compile texture: ${error}")
endif()

execute_process(
    COMMAND ./gen.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error
)

if(result)
    message(FATAL_ERROR "Failed to export the texture: ${error} (${output})")
endif()

message(STATUS "Building...")
add_executable(Miicraft main.cpp
        src/wiimote.cpp
        src/world/verticalChunk.cpp
        src/world/world.cpp
        src/engine/render/renderer.cpp
        src/engine/render/block.cpp
        src/engine/render/camera.cpp
        src/world/FlatWorldGenerator.cpp
        src/world/PerlinWorldGenerator.cpp
        src/world/coord.cpp
        src/system/saveManager.cpp
        src/system/Random.cpp
        src/player.cpp
        src/engine/render/cacheUnit.cpp
        src/engine/render/render.cpp
        src/engine/render/cache.cpp
        src/inventory.cpp
        src/item.cpp
        src/craft.cpp
        src/engine/render/interface.cpp
)

add_executable(MiicraftTest Debug.cpp
        src/world/verticalChunk.cpp
        src/world/world.cpp
        src/world/FlatWorldGenerator.cpp
        src/world/PerlinWorldGenerator.cpp
        src/world/coord.cpp
        src/system/Random.cpp
)


#add_compile_options(-lfat)
target_link_libraries(Miicraft fat)

ogc_create_dol(Miicraft)
