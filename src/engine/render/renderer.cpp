//
// Created by Romain on 05/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <cstring>
#include "engine/render/renderer.h"
#include "engine/render/bloc.h"
#include "texture.c"
#include "engine/render/cache.h"

void * Renderer::frameBuffer;
void * Renderer::frameBuffers[2];
int Renderer::selectFrameBuffer;
void * Renderer::gp_fifo = nullptr;

GXTexObj Renderer::texture ATTRIBUTE_ALIGN(32);

GXRModeObj * Renderer::rmode;
GXColor Renderer::background = {0x29, 0xae, 0xea, 0xff}; // blue


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
	GX_SetCopyClear({0x31, 0x31, 0x31, 0xFF}, GX_MAX_Z24);
	
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
	
	GX_CopyDisp(frameBuffer, GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

     GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);


    GX_SetNumTevStages(1);
//
//    GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);

    GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetZCompLoc(GX_FALSE);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GX_SetColorUpdate(GX_TRUE);
    GX_SetAlphaUpdate(GX_TRUE);
}

void Renderer::setupVtxDesc() {
	// setup the vertex attribute table
	GX_ClearVtxDesc();
	GX_InvVtxCache();
	
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_S8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	GX_SetArray(GX_VA_CLR0, (void *)Lights, 4);

}

void Renderer::setupTexture() {

    TPLFile  TPLfile;
	// set number of rasterized color channels
	GX_SetNumChans(2);
	
	//set number of textures to generate
	GX_SetNumTexGens(1);
	
	GX_InvalidateTexAll();

    TPL_OpenTPLFromMemory(&TPLfile, (void *)texture_data, texture_sz);
    TPL_GetTexture(&TPLfile, 0, &texture);
    GX_InitTexObjLOD(&texture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);


    GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
    GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);

    GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

    GX_LoadTexObj(&texture, GX_TEXMAP0);
}

void Renderer::endFrame() {
	GX_CopyDisp(frameBuffer, GX_TRUE);
	
	GX_DrawDone();

    VIDEO_SetNextFramebuffer(frameBuffer);
    VIDEO_Flush();
    VIDEO_WaitVSync();

	selectFrameBuffer ^= 1;
	frameBuffer = frameBuffers[selectFrameBuffer];
}


void Renderer::renderSky() const {
	/*
	f32 x = camera.max * tanf(camera.fovx), y = camera.max * tanf(camera.fovy), z = camera.max - .5;

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);

    GX_Position3f32(-x, y, -z);
    GX_Color1x8(BLUE);
    GX_TexCoord2f32(BLOCK_COORD(3), BLOCK_COORD(9));
	
    GX_Position3f32(x, y, -z);
    GX_Color1x8(BLUE);
    GX_TexCoord2f32(BLOCK_COORD(3), BLOCK_COORD(9));
	
    GX_Position3f32(x, -y, -z);
    GX_Color1x8(BLUE);
    GX_TexCoord2f32(BLOCK_COORD(3), BLOCK_COORD(9));
	
    GX_Position3f32(-x, -y, -z);
    GX_Color1x8(BLUE);
    GX_TexCoord2f32(BLOCK_COORD(3), BLOCK_COORD(9));
	
	GX_End();*/
}


void Renderer::renderBlock(const guVector &coord, BlockType type, u8 lt, u8 lb, u8 lf, u8 lk, u8 lr, u8 ll) {
    //Mtx model, modelview; // Various matrices

    //guMtxIdentity(model);

    //guMtxTransApply(model, model, coord.x, coord.y, coord.z);

    //guMtxConcat(camera.view3D, model, modelview);
    //GX_LoadPosMtxImm(modelview, GX_PNMTX0);
	
	f32
	mx = coord.x - 1,
	my = coord.y - 1,
	mz = coord.z - 1;

    f32 x, y;

    GX_Begin(GX_QUADS, GX_VTXFMT0, 24); // Start drawing

    // Bottom face
    x = blocData[type].x[BLOC_FACE_BOTTOM];
    y = blocData[type].y[BLOC_FACE_BOTTOM];

    GX_Position3f32(mx, my, coord.z);
	GX_Color1x8(lb << 2);
    GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right

    GX_Position3f32(coord.x, my, coord.z);
	GX_Color1x8(lb << 2);
    GX_TexCoord2f32(x, y + OFFSET); // Bottom left

    GX_Position3f32(coord.x, my, mz);
	GX_Color1x8(lb << 2);
    GX_TexCoord2f32(x, y); // Top left

    GX_Position3f32(mx, my, mz);
	GX_Color1x8(lb << 2);
    GX_TexCoord2f32(x + OFFSET, y); // Top right
    

    // Front face
    x = blocData[type].x[BLOC_FACE_FRONT];
    y = blocData[type].y[BLOC_FACE_FRONT];

    GX_Position3f32(mx, coord.y, coord.z);
	GX_Color1x8(lf << 2);
    GX_TexCoord2f32(x, y); // Top left

    GX_Position3f32(coord.x, coord.y, coord.z);
	GX_Color1x8(lf << 2);
    GX_TexCoord2f32(x + OFFSET, y); // Top right

    GX_Position3f32(coord.x, my, coord.z);
	GX_Color1x8(lf << 2);
    GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right

    GX_Position3f32(mx, my, coord.z);
	GX_Color1x8(lf << 2);
    GX_TexCoord2f32(x, y + OFFSET); // Bottom left
    

    // Back face
    x = blocData[type].x[BLOC_FACE_BACK];
    y = blocData[type].y[BLOC_FACE_BACK];

    GX_Position3f32(coord.x, my, mz);
	GX_Color1x8(lk << 2);
    GX_TexCoord2f32(x, y + OFFSET); // Bottom left

    GX_Position3f32(coord.x, coord.y, mz);
	GX_Color1x8(lk << 2);
    GX_TexCoord2f32(x, y); // Top left

    GX_Position3f32(mx, coord.y, mz);
	GX_Color1x8(lk << 2);
    GX_TexCoord2f32(x + OFFSET, y); // Top right

    GX_Position3f32(mx, my, mz);
	GX_Color1x8(lk << 2);
    GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right


    // Right face
    x = blocData[type].x[BLOC_FACE_RIGHT];
    y = blocData[type].y[BLOC_FACE_RIGHT];

    GX_Position3f32(coord.x, my, coord.z);
	GX_Color1x8(lr << 2);
    GX_TexCoord2f32(x, y + OFFSET); // Bottom left

    GX_Position3f32(coord.x, coord.y, coord.z);
	GX_Color1x8(lr << 2);
    GX_TexCoord2f32(x, y); // Top left

    GX_Position3f32(coord.x, coord.y, mz);
	GX_Color1x8(lr << 2);
    GX_TexCoord2f32(x + OFFSET, y); // Top right

    GX_Position3f32(coord.x, my, mz);
	GX_Color1x8(lr << 2);
    GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right


    // Left face
    x = blocData[type].x[BLOC_FACE_LEFT];
    y = blocData[type].y[BLOC_FACE_LEFT];

    GX_Position3f32(mx, coord.y, mz);
	GX_Color1x8(ll << 2);
    GX_TexCoord2f32(x, y); // Bottom left

    GX_Position3f32(mx, coord.y, coord.z);
	GX_Color1x8(ll << 2);
    GX_TexCoord2f32(x + OFFSET, y); // Top left

    GX_Position3f32(mx, my, coord.z);
	GX_Color1x8(ll << 2);
    GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

    GX_Position3f32(mx, my, mz);
	GX_Color1x8(ll << 2);
    GX_TexCoord2f32(x, y + OFFSET); // Bottom right


    // Top face
    x = blocData[type].x[BLOC_FACE_TOP];
    y = blocData[type].y[BLOC_FACE_TOP];

    GX_Position3f32(coord.x, coord.y, mz);
	GX_Color1x8(lt << 2);
    GX_TexCoord2f32(x + OFFSET, y); // Top right

    GX_Position3f32(coord.x, coord.y, coord.z);
	GX_Color1x8(lt << 2);
    GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right

    GX_Position3f32(mx, coord.y, coord.z);
	GX_Color1x8(lt << 2);
    GX_TexCoord2f32(x, y + OFFSET); // Bottom left

    GX_Position3f32(mx, coord.y, mz);
	GX_Color1x8(lt << 2);
    GX_TexCoord2f32(x, y); // Top left

    GX_End();         // Done drawing quads
}

void Renderer::renderFocus(f32 x, f32 y, f32 z) {
	f32
	mx = x - 1,
	my = y - 1,
	mz = z - 1;

    GX_SetLineWidth(20, GX_VTXFMT0);
	
    GX_Begin(GX_LINESTRIP, GX_VTXFMT0, 26);

        GX_Position3f32(mx, my, mz);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(x, my, mz);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(x, my, z);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(mx, my, z);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Bottom right

		
        GX_Position3f32(mx, my, z);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(x, my, z);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(x, y, z);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(mx, y, z);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Top left

		
        GX_Position3f32(mx, my, z);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(mx, my, mz);
		GX_Color1x8(BLACK);
		GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(mx, y, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(x, y, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top left

		
        GX_Position3f32(x, my, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(x, my, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(x, y, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(x, y, z);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top left

		
        GX_Position3f32(x, my, z);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(mx, my, z);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(mx, my, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(mx, my, z);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top right

		
        GX_Position3f32(mx, y, z);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(mx, y, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(mx, y, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(mx, y, z);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Bottom left

		
        GX_Position3f32(x, y, z);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(x, y, mz);
		GX_Color1x8(BLACK);
        GX_TexCoord2f32(0, OFFSET); // Top right

    GX_End();
	
    GX_SetLineWidth(1.0f, GX_VTXFMT0);
}

void Renderer::renderSplashScreen() {
	GX_Begin(GX_QUADS, GX_VTXFMT0, 16); // Start drawing
	
	const f32 yr = .2;
	const f32 xr = 0.8660254037844386 * 2; // sqrt(3/4) * 2
	
	GX_Position3f32(0, 2 * yr, 0);
	GX_Color1x8(61);
	GX_TexCoord2f32(BLOCK_COORD(3), BLOCK_COORD(2)); // Top left
	
	GX_Position3f32(yr * xr, yr, 0);
	GX_Color1x8(61);
	GX_TexCoord2f32(BLOCK_COORD(4), BLOCK_COORD(2)); // Top right
	
	GX_Position3f32(0, 0, 0);
	GX_Color1x8(61);
	GX_TexCoord2f32(BLOCK_COORD(4), BLOCK_COORD(3)); // Bottom right
	
	GX_Position3f32(-yr * xr, yr, 0);
	GX_Color1x8(61);
	GX_TexCoord2f32(BLOCK_COORD(3), BLOCK_COORD(3)); // Bottom left
	
	
	GX_Position3f32(0, 0, 0);
	GX_Color1x8(62);
	GX_TexCoord2f32(BLOCK_COORD(4), BLOCK_COORD(2)); // Top left
	
	GX_Position3f32(yr * xr, yr, 0);
	GX_Color1x8(62);
	GX_TexCoord2f32(BLOCK_COORD(5), BLOCK_COORD(2)); // Top right
	
	GX_Position3f32(yr * xr, -yr, 0);
	GX_Color1x8(62);
	GX_TexCoord2f32(BLOCK_COORD(5), BLOCK_COORD(3)); // Bottom right
	
	GX_Position3f32(0, -2 * yr, 0);
	GX_Color1x8(62);
	GX_TexCoord2f32(BLOCK_COORD(4), BLOCK_COORD(3)); // Bottom left
	
	
	GX_Position3f32(-yr * xr, yr, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(4), BLOCK_COORD(2)); // Top left
	
	GX_Position3f32(0, 0, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(5), BLOCK_COORD(2)); // Top right
	
	GX_Position3f32(0, -2 * yr, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(5), BLOCK_COORD(3)); // Bottom right
	
	GX_Position3f32(-yr * xr, -yr, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(4), BLOCK_COORD(3)); // Bottom left
	
	const f32 bh = .2;
	
	GX_Position3f32(-bh * 3.5f, -yr * 1.5f, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(8), BLOCK_COORD(15)); // Top left
	
	GX_Position3f32(bh * 3.5f, -yr * 1.5f, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(15)); // Top right
	
	GX_Position3f32(bh * 3.5f, -yr * 1.5f - bh, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(16)); // Bottom right
	
	GX_Position3f32(-bh * 3.5f, -yr * 1.5f - bh, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(8), BLOCK_COORD(16)); // Bottom left
	
	GX_End();
}

void Renderer::setClearColor(GXColor color) {
	GX_SetCopyClear(color, GX_MAX_Z24);
}
