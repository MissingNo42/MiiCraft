//
// Created by Romain on 05/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include <cstdio>
#include "engine/render/renderer.h"
#include "engine/render/block.h"

#include "texture.c"
#include "world/block.h"

void Renderer::setupVideo() {
	VIDEO_Init();
	rmode = VIDEO_GetPreferredMode(nullptr);
	frameBuffer = frameBuffers[selectFrameBuffer = 0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffers[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer);
	VIDEO_SetBlack(false);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	// setup the fifo...
	gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);
	
	// ...then init the flipper
	GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
	
	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(background, GX_MAX_Z24);

	// other gx setup
	GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
	f32 yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
	GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
	
	GX_SetPixelFmt(rmode->aa ? GX_PF_RGB565_Z16 : GX_PF_RGB8_Z24, GX_ZC_LINEAR); // TODO DANGER: GX_PF_RGB8_Z24 before
	
	//GX_SetCullMode(GX_CULL_BACK);
	GX_CopyDisp(frameBuffer, GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
	
	//GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetZCompLoc(GX_FALSE);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetColorUpdate(GX_TRUE);
	GX_SetAlphaUpdate(GX_TRUE);
	

    ///FOG

    GXColor greyBackground = {0x80, 0x80, 0x80, 0xff};
    GX_SetFog(GX_FOG_PERSP_LIN, 900, 990, 20, 1200, greyBackground);

    GXFogAdjTbl* fogTable = (GXFogAdjTbl*)memalign(32, 8 * sizeof(GXFogAdjTbl));

    f32 projmtx[4][4] = {
            {2.0f / rmode->fbWidth, 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / rmode->efbHeight, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 0.0f},
            {-1.0f, -1.0f, 0.0f, 1.0f}
    };

    GX_InitFogAdjTable(fogTable, rmode->fbWidth, projmtx);
    GX_SetFogRangeAdj(true, 500, fogTable);

	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
}

void Renderer::setupVtxDesc() {
	// setup the vertex attribute table
	GX_ClearVtxDesc();
	GX_InvVtxCache();
	
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

void Renderer::setupTexture() {
	TPLFile TPLfile;
	
	// set number of rasterized color channels
	GX_SetNumChans(2);
	
	//set number of textures to generate
	GX_SetNumTexGens(1);
	
	GX_InvalidateTexAll();
	//GX_SetMisc(GX_MT_XF_FLUSH, GX_XF_FLUSH_SAFE); // fix drawdone hang
	
	
	TPL_OpenTPLFromMemory(&TPLfile, (void *)texture_data, texture_sz);
	TPL_GetTexture(&TPLfile, GX_TEXMAP0, &texture);
	TPL_CloseTPLFile(&TPLfile);
    GX_InitTexObjLOD(&texture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
	GX_LoadTexObj(&texture, GX_TEXMAP0);
	
	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
	//GX_SetNumTevStages already to 1
	
	//GX_InitTexObjFilterMode(&texture, GX_NEAR, GX_NEAR);
	//GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE); // REPLACE=MODULATE -> alpha = black    |    DECAL -> alpha = white    |    GX_BLEND=PASSCLR -> all = white

	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
	// occlusion-> add a stage using color1/a1 of vertex and set these to vertex according to neighbors faces
}

void Renderer::setupDebugConsole() {
	CON_InitEx(rmode, 10, 10, 100, 100);
}

void Renderer::endFrame() {
	
	// end frame OP order form the GX Programming Manual
	GX_CopyDisp(frameBuffer, GX_TRUE);
	GX_DrawDone();
	
	VIDEO_SetNextFramebuffer(frameBuffer);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	
	selectFrameBuffer ^= 1;
	frameBuffer = frameBuffers[selectFrameBuffer];
}

Renderer::~Renderer() {
	free(gp_fifo);
	free(MEM_K1_TO_K0(frameBuffers[0]));
	free(MEM_K1_TO_K0(frameBuffers[1]));
}


void * Renderer::frameBuffer;
void * Renderer::frameBuffers[2];
int Renderer::selectFrameBuffer;
void * Renderer::gp_fifo = nullptr;
GXTexObj Renderer::texture ATTRIBUTE_ALIGN(32);

GXRModeObj * Renderer::rmode;
GXColor Renderer::background = {0, 0, 0, 0xff}; //{0x83, 0xDC, 0xE5, 0xff};


void Renderer::renderBloc(const guVector &coord, u32 code,
						  bool top, bool bottom, bool left, bool right, bool front, bool back) {
	Mtx model, modelview; // Various matrices
	
	guMtxIdentity(model);
	
	guMtxTransApply(model, model, coord.x, coord.y, coord.z);
	
	guMtxConcat(camera.view3D, model, modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);
	
	int sz = (top + bottom + left + right + front + back) << 2;
	f32 x, y;
	
	GX_Begin(GX_QUADS, GX_VTXFMT0, sz); // Start drawing

	// Bottom face
	if (bottom) {
		x = blocData[code].x[BLOCK_FACE_BOTTOM];
		y = blocData[code].y[BLOCK_FACE_BOTTOM];
		
		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, -1, 0);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right
		
		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, -1, 0);
		GX_TexCoord2f32(x, y + OFFSET); // Bottom left
		
		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, -1, 0);
		GX_TexCoord2f32(x, y); // Top left
		
		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, -1, 0);
		GX_TexCoord2f32(x + OFFSET, y); // Top right
	}

	// Front face
	if (front) {
		x = blocData[code].x[BLOCK_FACE_FRONT];
		y = blocData[code].y[BLOCK_FACE_FRONT];
		
		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(x, y); // Top left
		
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(x + OFFSET, y); // Top right
		
		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right
		
		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
		GX_TexCoord2f32(x, y + OFFSET); // Bottom left
	}

	// Back face
	if (back) {
		x = blocData[code].x[BLOCK_FACE_BACK];
		y = blocData[code].y[BLOCK_FACE_BACK];
		
		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, -1);
		GX_TexCoord2f32(x, y + OFFSET); // Bottom left
		
		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, -1);
		GX_TexCoord2f32(x, y); // Top left
		
		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, -1);
		GX_TexCoord2f32(x + OFFSET, y); // Top right
		
		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, -1);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right
		
	}

	// Right face
	if (right) {
		x = blocData[code].x[BLOCK_FACE_RIGHT];
		y = blocData[code].y[BLOCK_FACE_RIGHT];
		
		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(1, 0, 0);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom left
		
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(1, 0, 0);
		GX_TexCoord2f32(x + OFFSET, y); // Top left
		
		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(1, 0, 0);
		GX_TexCoord2f32(x, y); // Top right
		
		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(1, 0, 0);
		GX_TexCoord2f32(x, y + OFFSET); // Bottom right
	}

	// Left face
	if (left) {
		x = blocData[code].x[BLOCK_FACE_LEFT];
		y = blocData[code].y[BLOCK_FACE_LEFT];
		
		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(-1, 0, 0);
		GX_TexCoord2f32(x + OFFSET, y); // Bottom left
		
		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(-1, 0, 0);
		GX_TexCoord2f32(x, y); // Top left
		
		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(-1, 0, 0);
		GX_TexCoord2f32(x, y + OFFSET); // Top right
		
		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(-1, 0, 0);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right
	}

	// Top face
	if (top) {
		x = blocData[code].x[BLOCK_FACE_TOP];
		y = blocData[code].y[BLOCK_FACE_TOP];
		
		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 1, 0);
		GX_TexCoord2f32(x + OFFSET, y); // Top right
		
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 1, 0);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right
		
		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 1, 0);
		GX_TexCoord2f32(x, y + OFFSET); // Bottom left
		
		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 1, 0);
		GX_TexCoord2f32(x, y); // Top left
	}

	GX_End();         // Done drawing quads
}


void Renderer::drawFocus(Block block, f32 x, f32 y, f32 z) {
    Mtx model, modelview; // Various matrices

    guMtxIdentity(model);

    guMtxTransApply(model, model, x, y, z);

    guMtxConcat(camera.view3D, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_SetLineWidth(20.0f, GX_VTXFMT0);

    GX_Begin(GX_LINESTRIP, GX_VTXFMT0, 26);

        GX_Position3f32(-1.0f, -1.0f, -1.0f);
        GX_Normal3f32(0, -1, 0);
        GX_TexCoord2f32(0, 3); // Top right

        GX_Position3f32(0.0f, -1.0f, -1.0f);
        GX_Normal3f32(0, -1, 0);
        GX_TexCoord2f32(0, 3); // Top left

        GX_Position3f32(0.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, -1, 0);
        GX_TexCoord2f32(0, 3); // Bottom left

        GX_Position3f32(-1.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, -1, 0);
        GX_TexCoord2f32(0, 3); // Bottom right

        GX_Position3f32(-1.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(0, 3); // Bottom left

        GX_Position3f32(0.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(0, 3); // Bottom right

        GX_Position3f32(0.0f, 0.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(0, 3); // Top right

        GX_Position3f32(-1.0f, 0.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(0, 3); // Top left

        GX_Position3f32(-1.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(0, 3); // Top left

        GX_Position3f32(-1.0f, -1.0f, -1.0f);
        GX_Normal3f32(0, 0, -1);
        GX_TexCoord2f32(0, 3); // Bottom right

        GX_Position3f32(-1.0f, 0.0f, -1.0f);
        GX_Normal3f32(0, 0, -1);
        GX_TexCoord2f32(0, 3); // Top right

        GX_Position3f32(0.0f, 0.0f, -1.0f);
        GX_Normal3f32(0, 0, -1);
        GX_TexCoord2f32(0, 3); // Top left

        GX_Position3f32(0.0f, -1.0f, -1.0f);
        GX_Normal3f32(0, 0, -1);
        GX_TexCoord2f32(0, 3); // Bottom left

        GX_Position3f32(0.0f, -1.0f, -1.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(0, 3); // Bottom right

        GX_Position3f32(0.0f, 0.0f, -1.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(0, 3); // Top right

        GX_Position3f32(0.0f, 0.0f, 0.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(0, 3); // Top left

        GX_Position3f32(0.0f, -1.0f, 0.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(0, 3); // Bottom left

        GX_Position3f32(-1.0f, -1.0f, 0.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(0, 3); // Bottom left

        GX_Position3f32(-1.0f, -1.0f, -1.0f);
        GX_Normal3f32(-1, 0, 0);
        GX_TexCoord2f32(0, 3); // Bottom right

        GX_Position3f32(-1.0f, -1.0f, 0.0f);
        GX_Normal3f32(-1, 0, 0);
        GX_TexCoord2f32(0, 3); // Top right

        GX_Position3f32(-1.0f, 0.0f, 0.0f);
        GX_Normal3f32(-1, 0, 0);
        GX_TexCoord2f32(0, 3); // Top left

        GX_Position3f32(-1.0f, 0.0f, -1.0f);
        GX_Normal3f32(-1, 0, 0);
        GX_TexCoord2f32(0, 3); // Bottom left

        GX_Position3f32(-1.0f, 0.0f, -1.0f);
        GX_Normal3f32(0, 1, 0);
        GX_TexCoord2f32(0, 3); // Top left

        GX_Position3f32(-1.0f, 0.0f, 0.0f);
        GX_Normal3f32(0, 1, 0);
        GX_TexCoord2f32(0, 3); // Bottom left

        GX_Position3f32(0.0f, 0.0f, 0.0f);
        GX_Normal3f32(0, 1, 0);
        GX_TexCoord2f32(0, 3); // Bottom right

        GX_Position3f32(0.0f, 0.0f, -1.0f);
        GX_Normal3f32(0, 1, 0);
        GX_TexCoord2f32(0, 3); // Top right

    GX_End();
    GX_SetLineWidth(1.0f, GX_VTXFMT0);
}
/*
void Renderer::miningAnimation(int state, f32 x, f32 y, f32 z) {
    Mtx model, modelview; // Various matrices

    guMtxIdentity(model);

    guMtxTransApply(model, model, x, y, z);

    guMtxConcat(camera.viewMatrix, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 24); // Start drawing

    // Bottom face
        GX_Position3f32(-1.0f, -1.0f, -1.0f);
        GX_Normal3f32(0, -1, 0);
        GX_TexCoord2f32(x + (f32) state * OFFSET, y + (f32) state * OFFSET); // Top right

        GX_Position3f32(0.0f, -1.0f, -1.0f);
        GX_Normal3f32(0, -1, 0);
        GX_TexCoord2f32(x, y + (f32) state * OFFSET); // Top left

        GX_Position3f32(0.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, -1, 0);
        GX_TexCoord2f32(x, y); // Bottom left

        GX_Position3f32(-1.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, -1, 0);
        GX_TexCoord2f32(x + (f32) state * OFFSET, y); // Bottom right

        GX_Position3f32(-1.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(x, y); // Bottom left

        GX_Position3f32(0.0f, -1.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(x + OFFSET, y); // Bottom right

        GX_Position3f32(0.0f, 0.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

        GX_Position3f32(-1.0f, 0.0f, 0.0f);
        GX_Normal3f32(0, 0, 1);
        GX_TexCoord2f32(x, y + OFFSET); // Top left

    // Back face

        GX_Position3f32(-1.0f, -1.0f, -1.0f);
        GX_Normal3f32(0, 0, -1);
        GX_TexCoord2f32(x + OFFSET, y); // Bottom right

        GX_Position3f32(-1.0f, 0.0f, -1.0f);
        GX_Normal3f32(0, 0, -1);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

        GX_Position3f32(0.0f, 0.0f, -1.0f);
        GX_Normal3f32(0, 0, -1);
        GX_TexCoord2f32(x, y + OFFSET); // Top left

        GX_Position3f32(0.0f, -1.0f, -1.0f);
        GX_Normal3f32(0, 0, -1);
        GX_TexCoord2f32(x, y); // Bottom left

    // Right face

        GX_Position3f32(0.0f, -1.0f, -1.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(x + OFFSET, y); // Bottom right

        GX_Position3f32(0.0f, 0.0f, -1.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

        GX_Position3f32(0.0f, 0.0f, 0.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(x, y + OFFSET); // Top left

        GX_Position3f32(0.0f, -1.0f, 0.0f);
        GX_Normal3f32(1, 0, 0);
        GX_TexCoord2f32(x, y); // Bottom left

    // Left face

        GX_Position3f32(-1.0f, -1.0f, -1.0f);
        GX_Normal3f32(-1, 0, 0);
        GX_TexCoord2f32(x, y); // Bottom right

        GX_Position3f32(-1.0f, -1.0f, 0.0f);
        GX_Normal3f32(-1, 0, 0);
        GX_TexCoord2f32(x + OFFSET, y); // Top right

        GX_Position3f32(-1.0f, 0.0f, 0.0f);
        GX_Normal3f32(-1, 0, 0);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top left

        GX_Position3f32(-1.0f, 0.0f, -1.0f);
        GX_Normal3f32(-1, 0, 0);
        GX_TexCoord2f32(x, y + OFFSET); // Bottom left

    // Top face
        GX_Position3f32(-1.0f, 0.0f, -1.0f);
        GX_Normal3f32(0, 1, 0);
        GX_TexCoord2f32(x, y + OFFSET); // Top left

        GX_Position3f32(-1.0f, 0.0f, 0.0f);
        GX_Normal3f32(0, 1, 0);
        GX_TexCoord2f32(x, y); // Bottom left

        GX_Position3f32(0.0f, 0.0f, 0.0f);
        GX_Normal3f32(0, 1, 0);
        GX_TexCoord2f32(x + OFFSET, y); // Bottom right

        GX_Position3f32(0.0f, 0.0f, -1.0f);
        GX_Normal3f32(0, 1, 0);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

    GX_End();         // Done drawing quads
}*/


/*
Renderer::CacheChunk(const VerticalChunk& c){
	BlockType bk[16][16][16];
	
	
}*/
