//
// Created by Romain on 05/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include "engine/render/renderer.h"
#include "engine/render/bloc.h"

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
	GX_SetCopyClear(background, 0x00ffff22);
	
	// other gx setup
	GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
	f32 yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
	u32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
	GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
	
	GX_SetPixelFmt(rmode->aa ? GX_PF_RGB565_Z16 : GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	
	GX_SetCullMode(GX_CULL_NONE); // TODO: set to GX_CULL_BACK to disable backface culling (INIT as default)
	GX_CopyDisp(frameBuffer, GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
}

void Renderer::setupVtxDesc() {
	// setup the vertex attribute table
	// describes the data
	// args: vat location 0-7, type of data, data format, size, scale
	// so for ex. in the first call we are sending position data with
	// 3 values X,Y,Z of size F32. scale sets the number of fractional
	// bits for non float data.
	GX_ClearVtxDesc();
	GX_InvVtxCache();
	
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	
	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY); // TODO gtttttttt
	
}

void Renderer::setupMisc() {
	
	// set number of rasterized color channels
	GX_SetNumChans(2);
	
	//set number of textures to generate
	GX_SetNumTexGens(1);
	
	GX_InvalidateTexAll();
}

void Renderer::setupDebugConsole() {
	CON_InitEx(rmode, 10, 10, 100, 100);
}

void Renderer::testRender() {
	GXColor background = {0, 255, 0, 0xff};
	GX_SetCopyClear(background, GX_MAX_Z24);
	GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
	GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth, rmode->efbHeight);
	GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
	GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer, GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
}

void Renderer::endFrame() {
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(frameBuffer, GX_TRUE);
	
	GX_DrawDone();
	
	selectFrameBuffer ^= 1;
	frameBuffer = frameBuffers[selectFrameBuffer];
	
	VIDEO_SetNextFramebuffer(frameBuffer);
	VIDEO_Flush();
	VIDEO_WaitVSync();
}

void * Renderer::frameBuffer;
void * Renderer::frameBuffers[2];
int Renderer::selectFrameBuffer;
void * Renderer::gp_fifo = nullptr;

GXRModeObj * Renderer::rmode;
GXColor Renderer::background = {0xff, 0x00, 0xff, 0xff}; // blue = {0x29, 0xae, 0xea, 0xff}; // blue


void Renderer::renderBloc(const guVector &coord, u32 code) {
	Mtx model,modelview; // Various matrices
	
	guMtxIdentity(model);
	
	guMtxTransApply(model, model, coord.x, coord.y, coord.z);
	
	guMtxConcat(camera.viewMatrix, model, modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);
	
	
	GX_Begin(GX_QUADS, GX_VTXFMT0, 24); // Start drawing
	
	// Bottom face
	f32 x = blocData[code].x[BLOC_FACE_BOTTOM];
	f32 y = blocData[code].y[BLOC_FACE_BOTTOM];
	//SYS_Report("jtrdthbfdg %f %f\r", x, y);
	GX_Position3f32(-1.0f, -1.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 255); // Couleur avec transparence
	GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
	
	GX_Position3f32(0.0f, -1.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 255);
	GX_TexCoord2f32(x, y + OFFSET); // Top left
	
	GX_Position3f32(0.0f, -1.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 255);
	GX_TexCoord2f32(x, y); // Bottom left
	
	GX_Position3f32(-1.0f, -1.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 255);
	GX_TexCoord2f32(x + OFFSET, y); // Bottom right
	
	// Front face
	x = blocData[code].x[BLOC_FACE_FRONT];
	y = blocData[code].y[BLOC_FACE_FRONT];
	
	GX_Position3f32(-1.0f, -1.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x, y); // Bottom left
	
	GX_Position3f32(0.0f, -1.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y); // Bottom right
	
	GX_Position3f32(0.0f, 0.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
	
	GX_Position3f32(-1.0f, 0.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x, y + OFFSET); // Top left
	
	// Back face
	x = blocData[code].x[BLOC_FACE_BACK];
	y = blocData[code].y[BLOC_FACE_BACK];
	
	GX_Position3f32(-1.0f, -1.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y); // Bottom right
	
	GX_Position3f32(-1.0f, 0.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
	
	GX_Position3f32(0.0f, 0.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x, y + OFFSET); // Top left
	
	GX_Position3f32(0.0f, -1.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x, y); // Bottom left
	
	// Right face
	x = blocData[code].x[BLOC_FACE_RIGHT];
	y = blocData[code].y[BLOC_FACE_RIGHT];
	
	GX_Position3f32(0.0f, -1.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y); // Bottom right
	
	GX_Position3f32(0.0f, 0.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
	
	GX_Position3f32(0.0f, 0.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x, y + OFFSET); // Top left
	
	GX_Position3f32(0.0f, -1.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x, y); // Bottom left
	
	// Left face
	x = blocData[code].x[BLOC_FACE_LEFT];
	y = blocData[code].y[BLOC_FACE_LEFT];
	
	GX_Position3f32(-1.0f, -1.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x, y); // Bottom right
	
	GX_Position3f32(-1.0f, -1.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 0, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y); // Top right
	
	GX_Position3f32(-1.0f, 0.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 255, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top left
	
	GX_Position3f32(-1.0f, 0.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 255, 255, 0);
	GX_TexCoord2f32(x, y + OFFSET); // Bottom left
	
	// Top face
	x = blocData[code].x[BLOC_FACE_TOP];
	y = blocData[code].y[BLOC_FACE_TOP];
	
	GX_Position3f32(-1.0f, 0.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 255, 255, 0);
	GX_TexCoord2f32(x, y + OFFSET); // Top left
	
	GX_Position3f32(-1.0f, 0.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 255, 255, 0);
	GX_TexCoord2f32(x, y); // Bottom left
	
	GX_Position3f32(0.0f, 0.0f, 0.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 255, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y); // Bottom right
	
	GX_Position3f32(0.0f, 0.0f, -1.0f);
	GX_Normal3f32(0, 0, 1);
    GX_Color4u8(255, 255, 255, 0);
	GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
	GX_End();         // Done drawing quads
}
