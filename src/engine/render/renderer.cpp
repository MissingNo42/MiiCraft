//
// Created by Romain on 05/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include "engine/render/renderer.h"
#include "engine/render/bloc.h"
#include "fat.h"
#include "../../world/verticalChunk.h"

Mtx                 GXmodelView2D;

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

	GX_CopyDisp(frameBuffer, GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
}

void Renderer::setup3dMode(f32 minDist, f32 maxDist, f32 fov) {
    Mtx44 m;
    guVector pl = {camera.pos.x + camera.look.x, camera.pos.y + camera.look.y, camera.pos.z + camera.look.z};

    guLookAt(camera.viewMatrix, &camera.pos, &camera.up, &pl);

    guPerspective(m, fov, (f32)rmode->fbWidth/rmode->xfbHeight, minDist, maxDist);
    GX_LoadProjectionMtx(m, GX_PERSPECTIVE);
    GX_SetZMode (GX_TRUE, GX_LEQUAL, GX_TRUE);

    GX_SetCullMode(GX_CULL_NONE);

    GX_ClearVtxDesc();

    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
}

void Renderer::setup2dMode() const {
    Mtx view;
    Mtx44 m;

    GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_TRUE);

    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

    guOrtho(m, 0, rmode->efbHeight, 0, rmode->fbWidth, 0, 1000.0f);
    GX_LoadProjectionMtx(m, GX_ORTHOGRAPHIC);

    guMtxIdentity(view);
    guMtxTransApply(view, view, 0, 0, -100.0f);
    GX_LoadPosMtxImm(view, GX_PNMTX0);

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    GX_SetNumTexGens(1);  // One texture exists
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

    GX_SetNumTevStages(1);

    GX_SetTevOp  (GX_TEVSTAGE0, GX_PASSCLR);

    GX_SetNumChans(1);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

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
    //GX_SetAlphaUpdate(GX_TRUE);
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
GXColor Renderer::background = {0xff, 0xff, 0xff, 0xff}; // blue = {0x29, 0xae, 0xea, 0xff}; // blue



void Renderer::renderBloc(const guVector &coord, u32 code,
						  bool top, bool bottom, bool left, bool right, bool front, bool back) {

	Mtx model, modelview; // Various matrices
	
	guMtxIdentity(model);
	
	guMtxTransApply(model, model, coord.x, coord.y, coord.z);
	
	guMtxConcat(camera.viewMatrix, model, modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);


	int sz = (top + bottom + left + right + front + back) << 2;
	f32 x, y;
    int alphaValue = 255;
    if(code < SOLIDBLOCK) alphaValue = 0;

	GX_Begin(GX_QUADS, GX_VTXFMT0, sz); // Start drawing
	
	// Bottom face
	if (bottom) {
		x = blocData[code].x[BLOC_FACE_BOTTOM];
		y = blocData[code].y[BLOC_FACE_BOTTOM];
		//SYS_Report("jtrdthbfdg %f %f\r", x, y);
		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y + OFFSET); // Top left

		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y); // Bottom left

		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y); // Bottom right
	}

	// Front face
	if (front) {
		x = blocData[code].x[BLOC_FACE_FRONT];
		y = blocData[code].y[BLOC_FACE_FRONT];

		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y); // Bottom left

		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y); // Bottom right

		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y + OFFSET); // Top left
	}

	// Back face
	if (back) {
		x = blocData[code].x[BLOC_FACE_BACK];
		y = blocData[code].y[BLOC_FACE_BACK];

		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y); // Bottom right

		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y + OFFSET); // Top left

		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y); // Bottom left
	}

	// Right face
	if (right) {
		x = blocData[code].x[BLOC_FACE_RIGHT];
		y = blocData[code].y[BLOC_FACE_RIGHT];

		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y); // Bottom right

		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right

		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y + OFFSET); // Top left

		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y); // Bottom left
	}

	// Left face
	if (left) {
		x = blocData[code].x[BLOC_FACE_LEFT];
		y = blocData[code].y[BLOC_FACE_LEFT];

		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y); // Bottom right

		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y); // Top right

		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top left

		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y + OFFSET); // Bottom left
	}

	// Top face
	if (top) {
		x = blocData[code].x[BLOC_FACE_TOP];
		y = blocData[code].y[BLOC_FACE_TOP];

		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y + OFFSET); // Top left

		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x, y); // Bottom left

		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y); // Bottom right

		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color4u8(255, 255, 255, alphaValue);
		GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
	}

	GX_End();         // Done drawing quads
}

int Renderer::video_init(){
    Mtx44 perspective;
    s8 error_code = 0;

    // Initialise the video subsystem
    VIDEO_Init();
    VIDEO_SetBlack(true);  // Disable video output during initialisation

    // Grab a pointer to the video mode attributes
    rmode = VIDEO_GetPreferredMode(NULL);
    if (rmode == NULL) {
        return -1;
    }

    // Video Mode Correction
    switch (rmode->viTVMode) {
        case VI_DEBUG_PAL:  // PAL 50hz 576i
            //rmode = &TVPal574IntDfScale;
            rmode = &TVPal528IntDf; // BC ...this is still wrong, but "less bad" for now
            break;
        default:
#ifdef HW_DOL
            if(VIDEO_HaveComponentCable()) {
                rmode = &TVNtsc480Prog;
            }
#endif
            break;
    }

#if defined(HW_RVL)
    // 16:9 and 4:3 Screen Adjustment for Wii
    if (CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
        rmode->viWidth = 678;
    } else {    // 4:3
        rmode->viWidth = 672;
    }
    // This probably needs to consider PAL
    rmode->viXOrigin = (VI_MAX_WIDTH_NTSC - rmode->viWidth) / 2;
#endif

    // --
    VIDEO_Configure(rmode);

    // Get some memory to use for a "double buffered" frame buffer
    if ( !(frameBuffers[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode))) ) {
        return -1;
    }
    if ( !(frameBuffers[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode))) ) {
        return -1;
    }

    VIDEO_SetNextFramebuffer(frameBuffers[1]);  // Choose a frame buffer to start with

    VIDEO_Flush();                      // flush the frame to the TV
    VIDEO_WaitVSync();                  // Wait for the TV to finish updating
    // If the TV image is interlaced it takes two passes to display the image
    if (rmode->viTVMode & VI_NON_INTERLACE) {
        VIDEO_WaitVSync();
    }

    // The FIFO is the buffer the CPU uses to send commands to the GPU
    if ( !(gp_fifo = memalign(32, DEFAULT_FIFO_SIZE)) ) {
        return -1;
    }
    memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);
    GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);

    // Clear the background to opaque black and clears the z-buffer
    GX_SetCopyClear((GXColor){ 0, 0, 0, 0 }, GX_MAX_Z24);

    if (rmode->aa) {
        GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);  // Set 16 bit RGB565
    }
    else {
        GX_SetPixelFmt(GX_PF_RGB8_Z24  , GX_ZC_LINEAR);  // Set 24 bit Z24
    }

    // Other GX setup
    f32 yscale    = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
    u32 xfbHeight = GX_SetDispCopyYScale(yscale);
    GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
    GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));

    GX_SetDispCopyGamma(GX_GM_1_0);

    // Setup the vertex descriptor
    GX_ClearVtxDesc();      // clear all the vertex descriptors
    GX_InvVtxCache();       // Invalidate the vertex cache
    GX_InvalidateTexAll();  // Invalidate all textures

    // Tells the flipper to expect direct data
    GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
    GX_SetVtxDesc(GX_VA_POS,  GX_DIRECT);
    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS,  GX_POS_XYZ,  GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST,   GX_F32, 0);
    // Colour 0 is 8bit RGBA format
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GX_SetZMode(GX_FALSE, GX_LEQUAL, GX_TRUE);

    GX_SetNumChans(1);    // colour is the same as vertex colour
    GX_SetNumTexGens(1);  // One texture exists
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

    guMtxIdentity(GXmodelView2D);
    guMtxTransApply(GXmodelView2D, GXmodelView2D, 0.0f, 0.0f, -100.0f);
    GX_LoadPosMtxImm(GXmodelView2D, GX_PNMTX0);

    guOrtho(perspective, 0.0f, rmode->efbHeight, 0.0f, rmode->fbWidth, 0.0f, 1000.0f);
    GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

    GX_SetViewport(0.0f, 0.0f, rmode->fbWidth, rmode->efbHeight, 0.0f, 1.0f);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GX_SetAlphaUpdate(GX_TRUE);
    GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GX_SetColorUpdate(GX_ENABLE);
    GX_SetCullMode(GX_CULL_NONE);

    // Schedule cleanup for when program exits

    VIDEO_SetBlack(false);  // Enable video output
    return error_code;
}
