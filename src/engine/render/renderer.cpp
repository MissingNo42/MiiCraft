//
// Created by Romain on 05/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <cstring>
#include "engine/render/renderer.h"
#include "engine/render/block.h"
#include "texture.h"
#include "engine/render/cacheUnit.h"

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
    GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_S8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	
	GX_SetArray(GX_VA_CLR0, (void *)Lights, 4);
	GX_SetArray(GX_VA_TEX0, (void *)TexCoord, 8);
}

void Renderer::setupTexture() {

    TPLFile TPLfile;
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

    GX_LoadTexObj(&texture, GX_TEXMAP0);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
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


void Renderer::Underwater() {
	
	/// Runder blue overlay
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
	
	renderRect(-2, 1, 2, -1,
	           TXCOORD(15, 3),
	           TXCOORD(16, 3),
	           TXCOORD(15, 4),
	           TXCOORD(16, 4));
	GX_End();
}

void Renderer::renderRect(f32 x1, f32 y1, f32 x2, f32 y2, u16 lt, u16 rt, u16 lb, u16 rb) {
	GX_Position3f32(x1, y1, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord1x16(lt);
	
	GX_Position3f32(x2, y1, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord1x16(rt);
	
	GX_Position3f32(x2, y2, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord1x16(rb);
	
	GX_Position3f32(x1, y2, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord1x16(lb);
}

void Renderer::renderValue(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, u8 value) { // x1 < [D] < x2 < [U] < x3
	u16 D = value / 10, U = value % 10;
	
	renderRect(x1, y1, x2, y2,
	           TextureIndex::NUMBER_0T + D,
	           TextureIndex::NUMBER_0T + D + 1,
	           TextureIndex::NUMBER_0B + D,
	           TextureIndex::NUMBER_0B + D + 1);
	
	renderRect(x2, y1, x3, y2,
	           TextureIndex::NUMBER_0T + U,
	           TextureIndex::NUMBER_0T + U + 1,
	           TextureIndex::NUMBER_0B + U,
	           TextureIndex::NUMBER_0B + U + 1);
}

void Renderer::renderSky() const {
	f32 x = 1000, z = 1000, y = 130;
	
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
	
    GX_Position3f32(-x + camera.pos.x, y, z + camera.pos.z);
    GX_Color1x8(WHITE);
    GX_TexCoord1x16(TextureIndex::CLOUD_RB);
	
    GX_Position3f32(x + camera.pos.x, y, z + camera.pos.z);
    GX_Color1x8(WHITE);
    GX_TexCoord1x16(TextureIndex::CLOUD_LB);
	
    GX_Position3f32(x + camera.pos.x, y, -z + camera.pos.z);
    GX_Color1x8(WHITE);
    GX_TexCoord1x16(TXCOORD(0, 16));
	
    GX_Position3f32(-x + camera.pos.x, y, -z + camera.pos.z);
    GX_Color1x8(WHITE);
    GX_TexCoord1x16(TXCOORD(16, 16));
	
	GX_End();
}

static inline void renderVertex(f32 x, f32 y, f32 z, u8 c, u16 tc) {
	GX_Position3f32(x, y, z);
	GX_Color1x8(c);
	GX_TexCoord1x16(tc);
}

void Renderer::renderBlock(const guVector &coord, BlockType type, u8 lt, u8 lb, u8 lf, u8 lk, u8 lr, u8 ll) {
	f32
	mx = coord.x - 1,
	my = coord.y - 1,
	mz = coord.z - 1;

    GX_Begin(GX_QUADS, GX_VTXFMT0, 24);

    // Bottom face
    u16 tx = blockData[type].x[BLOCK_FACE_BOTTOM];
    u16 ty = blockData[type].y[BLOCK_FACE_BOTTOM];
	u16 tc = TXCOORD(tx, ty);
	
	renderVertex(mx, my, coord.z, lb << 2, tc + 18); // RB
	renderVertex(coord.x, my, coord.z, lb << 2, tc + 17); // LB
	renderVertex(coord.x, my, mz, lb << 2, tc); // LT
	renderVertex(mx, my, mz, lb << 2, tc + 1); // RT
    

    // Front face
    tx = blockData[type].x[BLOCK_FACE_FRONT];
    ty = blockData[type].y[BLOCK_FACE_FRONT];
	tc = TXCOORD(tx, ty);

	renderVertex(mx, coord.y, coord.z, lf << 2, tc); // LT
	renderVertex(coord.x, coord.y, coord.z, lf << 2, tc + 1); // RT
	renderVertex(coord.x, my, coord.z, lf << 2, tc + 18); // RB
	renderVertex(mx, my, coord.z, lf << 2, tc + 17); // LB
	   

    // Back face
    tx = blockData[type].x[BLOCK_FACE_BACK];
    ty = blockData[type].y[BLOCK_FACE_BACK];
	tc = TXCOORD(tx, ty);
	
	renderVertex(coord.x, my, mz, lk << 2, tc + 17); // LB
	renderVertex(coord.x, coord.y, mz, lk << 2, tc); // LT
	renderVertex(mx, coord.y, mz, lk << 2, tc + 1); // RT
	renderVertex(mx, my, mz, lk << 2, tc + 18); // RB


    // Right face
    tx = blockData[type].x[BLOCK_FACE_RIGHT];
    ty = blockData[type].y[BLOCK_FACE_RIGHT];
	tc = TXCOORD(tx, ty);
	
	renderVertex(coord.x, my, coord.z, lr << 2, tc + 17); // LB
	renderVertex(coord.x, coord.y, coord.z, lr << 2, tc); // LT
	renderVertex(coord.x, coord.y, mz, lr << 2, tc + 1); // RT
	renderVertex(coord.x, my, mz, lr << 2, tc + 18); // RB


    // Left face
    tx = blockData[type].x[BLOCK_FACE_LEFT];
    ty = blockData[type].y[BLOCK_FACE_LEFT];
	tc = TXCOORD(tx, ty);
	
	renderVertex(mx, coord.y, mz, ll << 2, tc); // LB
	renderVertex(mx, coord.y, coord.z, ll << 2, tc + 1); // LT
	renderVertex(mx, my, coord.z, ll << 2, tc + 18); // RT
	renderVertex(mx, my, mz, ll << 2, tc + 17); // RB


    // Top face
    tx = blockData[type].x[BLOCK_FACE_TOP];
    ty = blockData[type].y[BLOCK_FACE_TOP];
	tc = TXCOORD(tx, ty);
	
	renderVertex(coord.x, coord.y, mz, lt << 2, tc + 1); // RT
	renderVertex(coord.x, coord.y, coord.z, lt << 2, tc + 18); // LT
	renderVertex(mx, coord.y, coord.z, lt << 2, tc + 17); // LB
	renderVertex(mx, coord.y, mz, lt << 2, tc); // RB

    GX_End();
}

void Renderer::renderFocus(f32 x, f32 y, f32 z) {
	f32
	mx = x - 1,
	my = y - 1,
	mz = z - 1;

    GX_SetLineWidth(20, GX_VTXFMT0);
	
    GX_Begin(GX_LINESTRIP, GX_VTXFMT0, 26);

		renderVertex(mx, my, mz, BLACK, 0);  // RT
		renderVertex(x, my, mz, BLACK, 0);  // LT
		renderVertex(x, my, z, BLACK, 0);  // LB
		renderVertex(mx, my, z, BLACK, 0);  // RB
	
		renderVertex(mx, my, z, BLACK, 0);  // Bottom left
		renderVertex(x, my, z, BLACK, 0);  // Bottom right
		renderVertex(x, y, z, BLACK, 0);  // Top right
		renderVertex(mx, y, z, BLACK, 0);  // Top left
		
		renderVertex(mx, my, z, BLACK, 0);  // Top left
		renderVertex(mx, my, mz, BLACK, 0);  // Bottom right
		renderVertex(mx, y, mz, BLACK, 0);  // Top right
		renderVertex(x, y, mz, BLACK, 0);  // Top left
	
		renderVertex(x, my, mz, BLACK, 0);  // Bottom left
		renderVertex(x, my, mz, BLACK, 0);  // Bottom right
		renderVertex(x, y, mz, BLACK, 0);  // Top right
		renderVertex(x, y, z, BLACK, 0);  // Top left
		
		renderVertex(x, my, z, BLACK, 0);
		renderVertex(mx, my, z, BLACK, 0);
		renderVertex(mx, my, mz, BLACK, 0);
		renderVertex(mx, my, z, BLACK, 0);
		
		renderVertex(mx, y, z, BLACK, 0);
		renderVertex(mx, y, mz, BLACK, 0);
		renderVertex(mx, y, mz, BLACK, 0);
		renderVertex(mx, y, z, BLACK, 0);
		
		renderVertex(x, y, z, BLACK, 0);
		renderVertex(x, y, mz, BLACK, 0);

    GX_End();
	
    GX_SetLineWidth(1, GX_VTXFMT0);
}

void Renderer::renderVector(f32 x, f32 y, f32 z, u8 color) {
	
    GX_SetLineWidth(20, GX_VTXFMT0);
	
    GX_Begin(GX_LINESTRIP, GX_VTXFMT0, 2);
	
		renderVertex(x/z * 0.2, y/z * 0.2, (z<0) ? -0.2: 0.2, color, 0);
		renderVertex(x/2, y/2, z/2, color, 0);

    GX_End();
	
    GX_SetLineWidth(10, GX_VTXFMT0);
	
    GX_Begin(GX_LINESTRIP, GX_VTXFMT0, 2);
	
		renderVertex(x/2, y/2, z/2, color, 0);
		renderVertex(x, y, z, color, 0);

    GX_End();
	
    GX_SetLineWidth(1, GX_VTXFMT0);
}

void Renderer::renderSplashScreen() {
	GX_Begin(GX_QUADS, GX_VTXFMT0, 16); // Start drawing
	
	const f32 yr = .2;
	const f32 xr = 0.8660254037844386 * 2; // sqrt(3/4) * 2
	
	renderVertex(0, 2 * yr, 0, 61, TXCOORD(3, 2)); // Top left
	renderVertex(yr * xr, yr, 0, 61, TXCOORD(4, 2)); // Top right
	renderVertex(0, 0, 0, 61, TXCOORD(4, 3)); // Bottom right
	renderVertex(-yr * xr, yr, 0, 61, TXCOORD(3, 3)); // Bottom left
	
	renderVertex(0, 0, 0, 62, TXCOORD(4, 2)); // Top left
	renderVertex(yr * xr, yr, 0, 62, TXCOORD(5, 2)); // Top right
	renderVertex(yr * xr, -yr, 0, 62, TXCOORD(5, 3)); // Bottom right
	renderVertex(0, -2 * yr, 0, 62, TXCOORD(4, 3)); // Bottom left
	
	renderVertex(-yr * xr, yr, 0, WHITE, TXCOORD(4, 2)); // Top left
	renderVertex(0, 0, 0, WHITE, TXCOORD(5, 2)); // Top right
	renderVertex(0, -2 * yr, 0, WHITE, TXCOORD(5, 3)); // Bottom right
	renderVertex(-yr * xr, -yr, 0, WHITE, TXCOORD(4, 3)); // Bottom left
	
	const f32 bh = .2;
	
	renderVertex(-bh * 3.5f, -yr * 1.5f, 0, WHITE, TXCOORD(8, 15)); // Top left
	renderVertex(bh * 3.5f, -yr * 1.5f, 0, WHITE, TXCOORD(15, 15)); // Top right
	renderVertex(bh * 3.5f, -yr * 1.5f - bh, 0, WHITE, TXCOORD(15, 16)); // Bottom right
	renderVertex(-bh * 3.5f, -yr * 1.5f - bh, 0, WHITE, TXCOORD(8, 16)); // Bottom left
	
	GX_End();
}

void Renderer::setClearColor(GXColor color) {
	GX_SetCopyClear(color, GX_MAX_Z24);
}
