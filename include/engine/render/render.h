//
// Created by Romain on 15/02/2024.
//

#ifndef MIICRAFTTEST_RENDER_H
#define MIICRAFTTEST_RENDER_H

// Render inlined macros
#include "render/bloc.h"
#include "world/verticalChunk.h"
#include "renderer.h"

#define RENDER_BOTTOM_FACE(x, y, z, mx, my, mz, tx, ty, color) \
        GX_Position3f32(mx, my, mz); \
        GX_Normal1x8(1); \
        GX_Color1u32(color); \
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right

//inline void render_block

void renderChunk(VerticalChunk& c, Renderer& renderer, t_pos2D pos);

#endif //MIICRAFTTEST_RENDER_H