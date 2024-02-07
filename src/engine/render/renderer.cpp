//
// Created by Romain on 05/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include "engine/render/renderer.h"


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
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	
	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY); // TODO gtttttttt
	
}

void Renderer::setupMisc() {
	
	// set number of rasterized color channels
	GX_SetNumChans(1);
	
	//set number of textures to generate
	GX_SetNumTexGens(1);
	
	GX_InvalidateTexAll();
}

void Renderer::setupDebugConsole() {
	CON_InitEx(rmode, 10, 10, 100, 100);
}

void Renderer::testRender() {
	GXColor background = {0, 0, 0, 0xff};
	GX_SetCopyClear(background, 0x00ffffff);
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
GXColor Renderer::background = {0, 0, 0x90, 0xff}; // blue


void Renderer::renderBloc(const guVector &coord, const GXColor& color) {
	const f32 cx = coord.x;
	const f32 cy = coord.y;
	const f32 cz = coord.z;
	const f32 mx = coord.x - 1;
	const f32 my = coord.y - 1;
	const f32 mz = coord.z - 1;
	
	GX_Begin(GX_QUADS, GX_VTXFMT0, 24); // Start drawing
	// Bottom face
	GX_Position3f32(mx, my, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 1.0f); // Top right
	
	GX_Position3f32(cx, my, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 1.0f); // Top left
	
	GX_Position3f32(cx, my, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 0.0f); // Bottom left
	
	GX_Position3f32(mx, my, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 0.0f); // Bottom right
	
	// Front face
	GX_Position3f32(mx, my, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 0.0f); // Bottom left
	
	GX_Position3f32(cx, my, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 0.0f); // Bottom right
	
	GX_Position3f32(cx, cy, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 1.0f); // Top right
	
	GX_Position3f32(mx, cy, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 1.0f); // Top left
	
	// Back face
	GX_Position3f32(mx, my, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 0.0f); // Bottom right
	
	GX_Position3f32(mx, cy, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 1.0f); // Top right
	
	GX_Position3f32(cx, cy, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 1.0f); // Top left
	
	GX_Position3f32(cx, my, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 0.0f); // Bottom left
	
	// Right face
	GX_Position3f32(cx, my, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 0.0f); // Bottom right
	
	GX_Position3f32(cx, cy, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 1.0f); // Top right
	
	GX_Position3f32(cx, cy, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 1.0f); // Top left
	
	GX_Position3f32(cx, my, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 0.0f); // Bottom left
	
	// Left face
	GX_Position3f32(cx, my, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 0.0f); // Bottom right
	
	GX_Position3f32(mx, my, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 0.0f); // Top right
	
	GX_Position3f32(mx, cy, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 1.0f); // Top left
	
	GX_Position3f32(mx, cy, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 1.0f); // Bottom left
	
	// Top face
	GX_Position3f32(mx, cy, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 1.0f); // Top left
	
	GX_Position3f32(mx, cy, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(0.0f, 0.0f); // Bottom left
	
	GX_Position3f32(cx, cy, cz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 0.0f); // Bottom right
	
	GX_Position3f32(cx, cy, mz);
	GX_Normal3f32(0, 0, 1);
	GX_Color3f32(color.r, color.g, color.b);
	GX_TexCoord2f32(1.0f, 1.0f); // Top right
	
	GX_End();         // Done drawing quads
}
