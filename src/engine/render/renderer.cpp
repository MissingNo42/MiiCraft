//
// Created by Romain on 05/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include "engine/render/renderer.h"
#include "engine/render/bloc.h"
#include "../../world/block.h"
#include "texture.c"
#include "render/cache.h"

void * Renderer::frameBuffer;
void * Renderer::frameBuffers[2];
int Renderer::selectFrameBuffer;
void * Renderer::gp_fifo = nullptr;

GXTexObj Renderer::texture ATTRIBUTE_ALIGN(32);


GXRModeObj * Renderer::rmode;
GXColor Renderer::background =  {0x29, 0xae, 0xea, 0xff}; // blue = {0x29, 0xae, 0xea, 0xff}; // blue


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
	
	//GX_SetCullMode(GX_CULL_FRONT); // TODO: set to GX_CULL_BACK to disable backface culling (INIT as default)
	GX_CopyDisp(frameBuffer, GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

    ///FOG
    /*
    GXColor greyBackground = {0x80, 0x80, 0x80, 0xff};
//    GX_SetFog(GX_FOG_PERSP_LIN, 900, 990, 20, 1200, greyBackground);

//    GXFogAdjTbl* fogTable = (GXFogAdjTbl*)memalign(32, 8 * sizeof(GXFogAdjTbl));
//
//    f32 projmtx[4][4] = {
//            {2.0f / rmode->fbWidth, 0.0f, 0.0f, 0.0f},
//            {0.0f, 2.0f / rmode->efbHeight, 0.0f, 0.0f},
//            {0.0f, 0.0f, 0.0f, 0.0f},
//            {-1.0f, -1.0f, 0.0f, 1.0f}
//    };
//
//    GX_InitFogAdjTable(fogTable, rmode->fbWidth, projmtx);
//    GX_SetFogRangeAdj(true, 500, fogTable);

	// setup texture coordinate generation
	// args: texcoord slot 0-7, matrix type, source to generate texture coordinates from, matrix to use
	*/

    ///End Fog

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
	GX_SetVtxDesc(GX_VA_NRM, GX_INDEX8);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	GX_SetArray(GX_VA_NRM, (void *)Normals, 3 * sizeof(Normals[0]));

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
	//GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	//GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(frameBuffer, GX_TRUE);
	
	GX_DrawDone();

    VIDEO_SetNextFramebuffer(frameBuffer);
    VIDEO_Flush();
    VIDEO_WaitVSync();

	selectFrameBuffer ^= 1;
	frameBuffer = frameBuffers[selectFrameBuffer];


}



void Renderer::renderBloc(const guVector &coord, u32 code,
						  int top, int bottom, int left, int right, int front, int back,
                          int topVal, int bottomVal, int leftVal, int rightVal, int frontVal, int backVal,
                          int topIndex, int bottomIndex, int leftIndex, int rightIndex, int frontIndex, int backIndex)

    {





    int sz = (top + bottom + left + right + front + back) << 2;
    if(sz == 0) return;


//    Mtx model, modelview; // Various matrices

    //guMtxIdentity(model);
//
    //guMtxTransApply(model, model, coord.x, coord.y, coord.z);
//
    //guMtxConcat(camera.view3D, model, modelview);
    //GX_LoadPosMtxImm(modelview, GX_PNMTX0);


	f32
	mx = coord.x - 1,
	my = coord.y - 1,
	mz = coord.z - 1;

    f32 x, y;

    u32 white = 0xffffffff;
    u32 shadow = 0x808080ff;

    GX_Begin(GX_QUADS, GX_VTXFMT0, sz); // Start drawing

    // Bottom face
    if (bottom) {
        x = blocData[code].x[BLOC_FACE_BOTTOM];
        y = blocData[code].y[BLOC_FACE_BOTTOM];

        GX_Position3f32(mx, my, coord.z);
        GX_Normal1x8(1);
        if(bottomVal){
            GX_Color1u32(shadowGradient[bottomIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right

        GX_Position3f32(coord.x, my, coord.z);
        GX_Normal1x8(1);
        if(bottomVal){
            GX_Color1u32(shadowGradient[bottomIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y + OFFSET); // Bottom left

        GX_Position3f32(coord.x, my, mz);
        GX_Normal1x8(1);
        if(bottomVal){
            GX_Color1u32(shadowGradient[bottomIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y); // Top left

        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(1);
        if(bottomVal){
            GX_Color1u32(shadowGradient[bottomIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y); // Top right
    }

    // Front face
    if (front) {
        x = blocData[code].x[BLOC_FACE_FRONT];
        y = blocData[code].y[BLOC_FACE_FRONT];

        GX_Position3f32(mx, coord.y, coord.z);
        GX_Normal1x8(4);
        if(frontVal){
            GX_Color1u32(shadowGradient[frontIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y); // Top left

        GX_Position3f32(coord.x, coord.y, coord.z);
        GX_Normal1x8(4);
        if(frontVal){
            GX_Color1u32(shadowGradient[frontIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y); // Top right

        GX_Position3f32(coord.x, my, coord.z);
        GX_Normal1x8(4);
        if(frontVal){
            GX_Color1u32(shadowGradient[frontIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right

        GX_Position3f32(mx, my, coord.z);
        GX_Normal1x8(4);
        if(frontVal){
            GX_Color1u32(shadowGradient[frontIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y + OFFSET); // Bottom left
    }

    // Back face
    if (back) {
        x = blocData[code].x[BLOC_FACE_BACK];
        y = blocData[code].y[BLOC_FACE_BACK];

        GX_Position3f32(coord.x, my, mz);
        GX_Normal1x8(5);
        if(backVal){
            GX_Color1u32(shadowGradient[backIndex]);
        }
        else {
            GX_Color1u32(white);
        }

        GX_TexCoord2f32(x, y + OFFSET); // Bottom left

        GX_Position3f32(coord.x, coord.y, mz);
        GX_Normal1x8(5);
        if(backVal){
            GX_Color1u32(shadowGradient[backIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y); // Top left

        GX_Position3f32(mx, coord.y, mz);
        GX_Normal1x8(5);
        if(backVal){
            GX_Color1u32(shadowGradient[backIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y); // Top right

        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(5);
        if(backVal){
            GX_Color1u32(shadowGradient[backIndex]);
        }
        else {
            GX_Color1u32(white);
        }

        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right

    }

    // Right face
    if (right) {
        x = blocData[code].x[BLOC_FACE_RIGHT];
        y = blocData[code].y[BLOC_FACE_RIGHT];

        GX_Position3f32(coord.x, my, coord.z);
        GX_Normal1x8(2);
        if(rightVal){
            GX_Color1u32(shadowGradient[rightIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y + OFFSET); // Bottom left

        GX_Position3f32(coord.x, coord.y, coord.z);
        GX_Normal1x8(2);
        if(rightVal){
            GX_Color1u32(shadowGradient[rightIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y); // Top left

        GX_Position3f32(coord.x, coord.y, mz);
        GX_Normal1x8(2);
        if(rightVal){
            GX_Color1u32(shadowGradient[rightIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y); // Top right

        GX_Position3f32(coord.x, my, mz);
        GX_Normal1x8(2);
        if(rightVal){
            GX_Color1u32(shadowGradient[rightIndex]);
        }
        else {
            GX_Color1u32(white);
        }

        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right
    }

    // Left face
    if (left) {
        x = blocData[code].x[BLOC_FACE_LEFT];
        y = blocData[code].y[BLOC_FACE_LEFT];

        GX_Position3f32(mx, coord.y, mz);
        GX_Normal1x8(3);
        if(leftVal){
            GX_Color1u32(shadowGradient[leftIndex]);
        }
        else {
            GX_Color1u32(white);
        }

        GX_TexCoord2f32(x, y); // Bottom left

        GX_Position3f32(mx, coord.y, coord.z);
        GX_Normal1x8(3);
        if(leftVal){
            GX_Color1u32(shadowGradient[leftIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y); // Top left

        GX_Position3f32(mx, my, coord.z);
        GX_Normal1x8(3);
        if(leftVal){
            GX_Color1u32(shadowGradient[leftIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(3);
        if(leftVal){
            GX_Color1u32(shadowGradient[leftIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y + OFFSET); // Bottom right
    }

    // Top face
    if (top) {
        x = blocData[code].x[BLOC_FACE_TOP];
        y = blocData[code].y[BLOC_FACE_TOP];

        GX_Position3f32(coord.x, coord.y, mz);
        GX_Normal1x8(0);
        if(topVal){
            GX_Color1u32(shadowGradient[topIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x + OFFSET, y); // Top right

        GX_Position3f32(coord.x, coord.y, coord.z);
        GX_Normal1x8(0);
        if(topVal){
            GX_Color1u32(shadowGradient[topIndex]);
        }
        else {
            GX_Color1u32(white);
        }

        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Bottom right

        GX_Position3f32(mx, coord.y, coord.z);
        GX_Normal1x8(0);
        if(topVal){
            GX_Color1u32(shadowGradient[topIndex]);
        }
        else {
            GX_Color1u32(white);
        }

        GX_TexCoord2f32(x, y + OFFSET); // Bottom left

        GX_Position3f32(mx, coord.y, mz);
        GX_Normal1x8(0);
        if(topVal){
            GX_Color1u32(shadowGradient[topIndex]);
        }
        else {
            GX_Color1u32(white);
        }
        GX_TexCoord2f32(x, y); // Top left
    }

    GX_End();         // Done drawing quads
}

void Renderer::drawFocus(Block block, f32 x, f32 y, f32 z) {
    Mtx model, modelview; // Various matrices

    //guMtxIdentity(model);
//
    //guMtxTransApply(model, model, x, y, z);
//
    //guMtxConcat(camera.view3D, model, modelview);
    //GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	f32
	mx = x - 1,
	my = y - 1,
	mz = z - 1;

    u32 black = 0x000000ff;

    GX_SetLineWidth(20.0f, GX_VTXFMT0);

    GX_Begin(GX_LINESTRIP, GX_VTXFMT0, 26);

        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(1);
    GX_Color1u32(black);
        GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(x, my, mz);
        GX_Normal1x8(1);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(x, my, z);
        GX_Normal1x8(1);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(mx, my, z);
        GX_Normal1x8(1);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(mx, my, z);
        GX_Normal1x8(4);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(x, my, z);
        GX_Normal1x8(4);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(x, y, z);
        GX_Normal1x8(4);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(mx, y, z);
        GX_Normal1x8(4);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(mx, my, z);
        GX_Normal1x8(4);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(5);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(mx, y, mz);
        GX_Normal1x8(5);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(x, y, mz);
        GX_Normal1x8(5);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(x, my, mz);
        GX_Normal1x8(5);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(x, my, mz);
        GX_Normal1x8(2);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(x, y, mz);
        GX_Normal1x8(2);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(x, y, z);
        GX_Normal1x8(2);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(x, my, z);
        GX_Normal1x8(2);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(mx, my, z);
        GX_Normal1x8(2);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(3);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(mx, my, z);
        GX_Normal1x8(3);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top right

        GX_Position3f32(mx, y, z);
        GX_Normal1x8(3);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(mx, y, mz);
        GX_Normal1x8(3);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(mx, y, mz);
        GX_Normal1x8(0);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top left

        GX_Position3f32(mx, y, z);
        GX_Normal1x8(0);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom left

        GX_Position3f32(x, y, z);
        GX_Normal1x8(0);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Bottom right

        GX_Position3f32(x, y, mz);
        GX_Normal1x8(0);
    GX_Color1u32(black);

    GX_TexCoord2f32(0, OFFSET); // Top right

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
        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(1);
        GX_TexCoord2f32(x + (f32) state * OFFSET, y + (f32) state * OFFSET); // Top right

        GX_Position3f32(x, my, mz);
        GX_Normal1x8(1);
        GX_TexCoord2f32(x, y + (f32) state * OFFSET); // Top left

        GX_Position3f32(x, my, z);
        GX_Normal1x8(1);
        GX_TexCoord2f32(x, y); // Bottom left

        GX_Position3f32(mx, my, z);
        GX_Normal1x8(1);
        GX_TexCoord2f32(x + (f32) state * OFFSET, y); // Bottom right

        GX_Position3f32(mx, my, z);
        GX_Normal1x8(4);
        GX_TexCoord2f32(x, y); // Bottom left

        GX_Position3f32(x, my, z);
        GX_Normal1x8(4);
        GX_TexCoord2f32(x + OFFSET, y); // Bottom right

        GX_Position3f32(x, y, z);
        GX_Normal1x8(4);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

        GX_Position3f32(mx, y, z);
        GX_Normal1x8(4);
        GX_TexCoord2f32(x, y + OFFSET); // Top left

    // Back face

        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(5);
        GX_TexCoord2f32(x + OFFSET, y); // Bottom right

        GX_Position3f32(mx, y, mz);
        GX_Normal1x8(5);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

        GX_Position3f32(x, y, mz);
        GX_Normal1x8(5);
        GX_TexCoord2f32(x, y + OFFSET); // Top left

        GX_Position3f32(x, my, mz);
        GX_Normal1x8(5);
        GX_TexCoord2f32(x, y); // Bottom left

    // Right face

        GX_Position3f32(x, my, mz);
        GX_Normal1x8(2);
        GX_TexCoord2f32(x + OFFSET, y); // Bottom right

        GX_Position3f32(x, y, mz);
        GX_Normal1x8(2);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

        GX_Position3f32(x, y, z);
        GX_Normal1x8(2);
        GX_TexCoord2f32(x, y + OFFSET); // Top left

        GX_Position3f32(x, my, z);
        GX_Normal1x8(2);
        GX_TexCoord2f32(x, y); // Bottom left

    // Left face

        GX_Position3f32(mx, my, mz);
        GX_Normal1x8(3);
        GX_TexCoord2f32(x, y); // Bottom right

        GX_Position3f32(mx, my, z);
        GX_Normal1x8(3);
        GX_TexCoord2f32(x + OFFSET, y); // Top right

        GX_Position3f32(mx, y, z);
        GX_Normal1x8(3);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top left

        GX_Position3f32(mx, y, mz);
        GX_Normal1x8(3);
        GX_TexCoord2f32(x, y + OFFSET); // Bottom left

    // Top face
        GX_Position3f32(mx, y, mz);
        GX_Normal1x8(0);
        GX_TexCoord2f32(x, y + OFFSET); // Top left

        GX_Position3f32(mx, y, z);
        GX_Normal1x8(0);
        GX_TexCoord2f32(x, y); // Bottom left

        GX_Position3f32(x, y, z);
        GX_Normal1x8(0);
        GX_TexCoord2f32(x + OFFSET, y); // Bottom right

        GX_Position3f32(x, y, mz);
        GX_Normal1x8(0);
        GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

    GX_End();         // Done drawing quads
}*/

/*
Renderer::CacheChunk(const VerticalChunk& c){
	BlockType bk[16][16][16];
	
	
}*/
