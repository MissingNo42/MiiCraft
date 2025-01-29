cmake_minimum_required(VERSION 3.22)

project(Miicraft)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Winline)
add_compile_options(-Wno-unknown-pragmas)
add_compile_options(-g)
add_compile_options(-fms-extensions)
add_compile_options(-Ofast)


# Assets generation

set(ASSETS_DIR   ${CMAKE_SOURCE_DIR}/assets)
set(GENERATORS_DIR ${ASSETS_DIR}/generators)
set(TEXTURES_DIR ${ASSETS_DIR}/textures)
set(SKYBOX_DIR   ${TEXTURES_DIR}/skybox)


# Assets Skybox Cubemaps generation

set(SKYBOX_GENERATOR      ${GENERATORS_DIR}/textures.skybox.py)
set(SKYBOX_NIGHT          ${SKYBOX_DIR}/night.png) #  /night.debug.png)
set(SKYBOX_DAY            ${SKYBOX_DIR}/day.png)
set(SKYBOX_SUNRISE        ${SKYBOX_DIR}/sunrise.png)
set(SKYBOX_SUNSET         ${SKYBOX_DIR}/sunset.png)
set(SKYBOX_SUNRISE_EFFECT ${SKYBOX_DIR}/sunrise.effect.png)
set(SKYBOX_SUNSET_EFFECT  ${SKYBOX_DIR}/sunset.effect.png)
set(SKYBOX_MAIN_CUBEMAP   ${SKYBOX_DIR}/generated.main.png)
set(SKYBOX_NIGHT_CUBEMAP  ${SKYBOX_DIR}/generated.night.png)
set(SKYBOX_HEADER_CUBEMAP ${SKYBOX_DIR}/generated.cubemap.h)

add_custom_command(
		OUTPUT ${SKYBOX_MAIN_CUBEMAP} ${SKYBOX_NIGHT_CUBEMAP} ${SKYBOX_HEADER_CUBEMAP}
		COMMAND ${SKYBOX_GENERATOR} ${SKYBOX_NIGHT} ${SKYBOX_DAY} ${SKYBOX_SUNRISE} ${SKYBOX_SUNSET} ${SKYBOX_SUNRISE_EFFECT} ${SKYBOX_SUNSET_EFFECT} ${SKYBOX_MAIN_CUBEMAP} ${SKYBOX_NIGHT_CUBEMAP} ${SKYBOX_HEADER_CUBEMAP}
		DEPENDS ${SKYBOX_GENERATOR} ${SKYBOX_NIGHT} ${SKYBOX_DAY} ${SKYBOX_SUNRISE} ${SKYBOX_SUNSET} ${SKYBOX_SUNRISE_EFFECT} ${SKYBOX_SUNSET_EFFECT}
		COMMENT "Generating the cubemaps"
)


# Assets Blending Textures generation

set(BLEND_GENERATOR    ${GENERATORS_DIR}/textures.blend.py)
set(MAIN_TEXTURE       ${TEXTURES_DIR}/main.png)
set(BLEND_MAIN_TEXTURE ${TEXTURES_DIR}/generated.main.png)

add_custom_command(
		OUTPUT ${BLEND_MAIN_TEXTURE}
		COMMAND ${BLEND_GENERATOR} ${MAIN_TEXTURE} ${SKYBOX_MAIN_CUBEMAP} ${BLEND_MAIN_TEXTURE}
		DEPENDS ${BLEND_GENERATOR} ${MAIN_TEXTURE} ${SKYBOX_MAIN_CUBEMAP}
		COMMENT "Blending the cubemaps"
)


# Assets Converting Textures generation

set(ENCODED_MAIN_TEXTURE  ${TEXTURES_DIR}/generated.main.tpl)
set(ENCODED_NIGHT_TEXTURE ${TEXTURES_DIR}/generated.night.tpl)

add_custom_command(
		OUTPUT ${ENCODED_MAIN_TEXTURE}
		COMMAND ${DEVKITPRO}/tools/bin/gxtexconv -i ${BLEND_MAIN_TEXTURE} -o ${ENCODED_MAIN_TEXTURE} colfmt=6 mipmap=no
		DEPENDS ${BLEND_MAIN_TEXTURE}
		COMMENT "Converting the main texture"
)

add_custom_command(
		OUTPUT ${ENCODED_NIGHT_TEXTURE}
		COMMAND ${DEVKITPRO}/tools/bin/gxtexconv -i ${SKYBOX_NIGHT_CUBEMAP} -o ${ENCODED_NIGHT_TEXTURE} colfmt=14 mipmap=no
		DEPENDS ${SKYBOX_NIGHT_CUBEMAP}
		COMMENT "Converting the night texture"
)


# Assets Embedding Textures generation

set(EMBED_GENERATOR     ${GENERATORS_DIR}/textures.embed.py)
set(EMBEDDED_ASSETS_SRC ${CMAKE_SOURCE_DIR}/src/engine/render/assets.cpp)
set(EMBEDDED_ASSETS_INC ${CMAKE_SOURCE_DIR}/include/engine/render/assets.h)
set(PALETTE_DAY         ${TEXTURES_DIR}/palette.day.png)
set(PALETTE_NIGHT       ${TEXTURES_DIR}/palette.night.png)

add_custom_command(
        OUTPUT ${EMBEDDED_ASSETS_SRC} ${EMBEDDED_ASSETS_INC}
        COMMAND ${EMBED_GENERATOR} ${ENCODED_MAIN_TEXTURE} ${ENCODED_NIGHT_TEXTURE} ${PALETTE_DAY} ${PALETTE_NIGHT} ${SKYBOX_HEADER_CUBEMAP} ${EMBEDDED_ASSETS_SRC} ${EMBEDDED_ASSETS_INC}
        DEPENDS ${EMBED_GENERATOR} ${ENCODED_MAIN_TEXTURE} ${ENCODED_NIGHT_TEXTURE} ${PALETTE_DAY} ${PALETTE_NIGHT} ${SKYBOX_HEADER_CUBEMAP}
        COMMENT "Embedding the assets"
)

add_custom_target(assets_textures_embed ALL DEPENDS ${EMBEDDED_ASSETS_SRC} ${EMBEDDED_ASSETS_INC})


# Set the directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES  "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE INCLUDES "${INC_DIR}/*.h")

list(REMOVE_ITEM SOURCES  ${COMPILED_ASSETS_SRC})
list(REMOVE_ITEM INCLUDES ${COMPILED_ASSETS_INC})

message(STATUS "Sources:")
foreach (SOURCE ${SOURCES})
    message(STATUS "\t: ${SOURCE}")
endforeach ()

message(STATUS "Headers:")
foreach (INCLUDE ${INCLUDES})
    message(STATUS "\t: ${INCLUDE}")
endforeach ()

add_executable(Miicraft)
add_dependencies(Miicraft assets_textures_embed)

target_sources(Miicraft
        PRIVATE
        main.cpp
        ${SOURCES}
        ${INCLUDES}
		${COMPILED_ASSETS_INC}
		${COMPILED_ASSETS_SRC}
)

target_link_libraries(Miicraft fat)

ogc_create_dol(Miicraft)
