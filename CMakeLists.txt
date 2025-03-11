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

set(ASSETS_DIR     ${CMAKE_SOURCE_DIR}/assets)
set(GENERATORS_DIR ${ASSETS_DIR}/generators)
set(GENERATED_DIR  ${ASSETS_DIR}/generated)
set(TEXTURES_DIR   ${ASSETS_DIR}/textures)
set(SKYBOX_DIR     ${TEXTURES_DIR}/skybox)
set(ASSETS_INC_DIR ${CMAKE_SOURCE_DIR}/include/assets)
set(ASSETS_SRC_DIR ${CMAKE_SOURCE_DIR}/src/assets)

file(MAKE_DIRECTORY ${GENERATED_DIR})
file(MAKE_DIRECTORY ${ASSETS_INC_DIR})
file(MAKE_DIRECTORY ${ASSETS_SRC_DIR})


# Assets Skybox Cubemaps generation

set(SKYBOX_GENERATOR      ${GENERATORS_DIR}/textures.skybox.py)
set(SKYBOX_NIGHT          ${SKYBOX_DIR}/night.png)  # night.debug.png)
set(SKYBOX_DAY            ${SKYBOX_DIR}/day.png)
set(SKYBOX_SUNRISE        ${SKYBOX_DIR}/sunrise.png)
set(SKYBOX_SUNSET         ${SKYBOX_DIR}/sunset.png)
set(SKYBOX_SUNRISE_EFFECT ${SKYBOX_DIR}/sunrise.effect.png)
set(SKYBOX_SUNSET_EFFECT  ${SKYBOX_DIR}/sunset.effect.png)
set(SKYBOX_MAIN_CUBEMAP   ${GENERATED_DIR}/generated.cubemap.png)
set(SKYBOX_NIGHT_CUBEMAP  ${GENERATED_DIR}/generated.night.png)
set(SKYBOX_HEADER_CUBEMAP ${ASSETS_INC_DIR}/cubemap.h)

add_custom_command(
		OUTPUT ${SKYBOX_MAIN_CUBEMAP} ${SKYBOX_NIGHT_CUBEMAP} ${SKYBOX_HEADER_CUBEMAP}
		COMMAND ${SKYBOX_GENERATOR} ${SKYBOX_NIGHT} ${SKYBOX_DAY} ${SKYBOX_SUNRISE} ${SKYBOX_SUNSET} ${SKYBOX_SUNRISE_EFFECT} ${SKYBOX_SUNSET_EFFECT} ${SKYBOX_MAIN_CUBEMAP} ${SKYBOX_NIGHT_CUBEMAP} ${SKYBOX_HEADER_CUBEMAP}
		DEPENDS ${SKYBOX_GENERATOR} ${SKYBOX_NIGHT} ${SKYBOX_DAY} ${SKYBOX_SUNRISE} ${SKYBOX_SUNSET} ${SKYBOX_SUNRISE_EFFECT} ${SKYBOX_SUNSET_EFFECT}
		COMMENT "Generating the cubemaps"
)


# Assets Cloud Geometry generation

set(CLOUD_GENERATOR ${GENERATORS_DIR}/textures.cloud.py)
set(CLOUD_TEXTURE   ${TEXTURES_DIR}/cloud.png)
set(CLOUD_SOURCE    ${ASSETS_SRC_DIR}/cloud.cpp)
set(CLOUD_HEADER    ${ASSETS_INC_DIR}/cloud.h)

add_custom_command(
		OUTPUT ${CLOUD_SOURCE} ${CLOUD_HEADER}
		COMMAND ${CLOUD_GENERATOR} ${CLOUD_TEXTURE} ${CLOUD_SOURCE} ${CLOUD_HEADER}
		DEPENDS ${CLOUD_GENERATOR} ${CLOUD_TEXTURE}
		COMMENT "Generating cloud geometry"
)


# Assets Blending Textures generation

set(BLEND_GENERATOR    ${GENERATORS_DIR}/textures.blend.py)
set(MAIN_TEXTURE       ${TEXTURES_DIR}/main.png)
set(BLEND_MAIN_TEXTURE ${GENERATED_DIR}/generated.main.png)

add_custom_command(
		OUTPUT ${BLEND_MAIN_TEXTURE}
		COMMAND ${BLEND_GENERATOR} ${MAIN_TEXTURE} ${SKYBOX_MAIN_CUBEMAP} ${BLEND_MAIN_TEXTURE}
		DEPENDS ${BLEND_GENERATOR} ${MAIN_TEXTURE} ${SKYBOX_MAIN_CUBEMAP}
		COMMENT "Blending the cubemaps"
)


# Assets Converting Textures generation

set(ENCODED_MAIN_TEXTURE  ${GENERATED_DIR}/generated.main.tpl)
set(ENCODED_NIGHT_TEXTURE ${GENERATED_DIR}/generated.night.tpl)

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
set(EMBEDDED_ASSETS_SRC ${ASSETS_SRC_DIR}/assets.cpp)
set(EMBEDDED_ASSETS_INC ${ASSETS_INC_DIR}/assets.h)
set(PALETTE_DAY         ${TEXTURES_DIR}/palette.day.png)
set(PALETTE_NIGHT       ${TEXTURES_DIR}/palette.night.png)

add_custom_command(
        OUTPUT ${EMBEDDED_ASSETS_SRC} ${EMBEDDED_ASSETS_INC}
        COMMAND ${EMBED_GENERATOR} ${ENCODED_MAIN_TEXTURE} ${ENCODED_NIGHT_TEXTURE} ${PALETTE_DAY} ${PALETTE_NIGHT} ${EMBEDDED_ASSETS_SRC} ${EMBEDDED_ASSETS_INC}
        DEPENDS ${EMBED_GENERATOR} ${ENCODED_MAIN_TEXTURE} ${ENCODED_NIGHT_TEXTURE} ${PALETTE_DAY} ${PALETTE_NIGHT}
        COMMENT "Embedding the assets"
)


set(ASSETS_SOURCES ${EMBEDDED_ASSETS_SRC} ${CLOUD_SOURCE})
set(ASSETS_HEADERS ${EMBEDDED_ASSETS_INC} ${CLOUD_HEADER} ${SKYBOX_HEADER_CUBEMAP})

add_custom_target(assets_textures_embed ALL DEPENDS ${ASSETS_SOURCES} ${ASSETS_HEADERS})


# Set the directories
set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_SOURCE_DIR}/include")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${INC_DIR}/*.h")

list(REMOVE_ITEM SOURCES ${ASSETS_SOURCES})
list(REMOVE_ITEM HEADERS ${ASSETS_HEADERS})

message(STATUS "Sources:")
foreach (SOURCE ${SOURCES})
    message(STATUS "\t: ${SOURCE}")
endforeach ()

message(STATUS "Headers:")
foreach (HEADER ${HEADERS})
    message(STATUS "\t: ${HEADER}")
endforeach ()

add_executable(Miicraft)
add_dependencies(Miicraft assets_textures_embed)

target_sources(Miicraft
        PRIVATE
        main.cpp
        ${SOURCES}
        ${HEADERS}
		${ASSETS_SOURCES}
		${ASSETS_HEADERS}
)

target_link_libraries(Miicraft fat)

ogc_create_dol(Miicraft)
