//
// Created by Romain on 05/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <string.h>
#include "engine/render/renderer.h"
#include "engine/render/bloc.h"

void Renderer::setupVideo() {
    Mtx44 perspective;
    s8 error_code = 0;

    // Ensure this function is only ever called once

    // Initialise the video subsystem
    VIDEO_Init();
    VIDEO_SetBlack(true);  // Disable video output during initialisation

    // Grab a pointer to the video mode attributes
    rmode = VIDEO_GetPreferredMode(NULL);

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
    selectFrameBuffer = 0;
    // Get some memory to use for a "double buffered" frame buffer
    if ( !(frameBuffers[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode))) ) {
    }
    if ( !(frameBuffers[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode))) ) {
    }

    VIDEO_SetNextFramebuffer(frameBuffers[0]);  // Choose a frame buffer to start with

    VIDEO_Flush();                      // flush the frame to the TV
    VIDEO_WaitVSync();                  // Wait for the TV to finish updating
    // If the TV image is interlaced it takes two passes to display the image
    if (rmode->viTVMode & VI_NON_INTERLACE) {
        VIDEO_WaitVSync();
    }

    // The FIFO is the buffer the CPU uses to send commands to the GPU
    if ( !(gp_fifo = memalign(32, DEFAULT_FIFO_SIZE)) ) {

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



    guOrtho(perspective, 0.0f, rmode->efbHeight, 0.0f, rmode->fbWidth, 0.0f, 1000.0f);
    GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

    GX_SetViewport(0.0f, 0.0f, rmode->fbWidth, rmode->efbHeight, 0.0f, 1.0f);
    GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GX_SetAlphaUpdate(GX_TRUE);
    GX_SetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GX_SetColorUpdate(GX_ENABLE);
    GX_SetCullMode(GX_CULL_NONE);
    GX_SetClipMode( GX_CLIP_ENABLE );
    GX_SetScissor( 0, 0, rmode->fbWidth, rmode->efbHeight );


    // Default settings
    //GRRLIB_Settings.antialias = true;
    //GRRLIB_Settings.blend     = GRRLIB_BLEND_ALPHA;
    //GRRLIB_Settings.lights    = 0;

    // Schedule cleanup for when program exits

    // Initialise the filing system
    // if (fatInitDefault() == false) {
    //     error_code = -2;
    // }

    VIDEO_SetBlack(false);  // Enable video output

}

void Renderer::setupVtxDesc3D() {


    GX_SetCullMode(GX_CULL_NONE);

    GX_ClearVtxDesc();
    GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
    GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT); // => normal mode true

    GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    //    GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT); => texutre mod true


    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);  // => normal mode true

    GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    //GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0); // => texture mod true



//       // GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE); texture mod
//    }
//    else {
      GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
//    }
}

void Renderer::setupVtxDesc2D() {
	// setup the vertex attribute table
	GX_InvVtxCache();
	GX_InvalidateTexAll();
	GX_ClearVtxDesc();

	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

void Renderer::setupMisc() {


    GX_SetNumChans(1);    // colour is the same as vertex colour
    GX_SetNumTexGens(1);  // One texture exists
    GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
}

void Renderer::setupDebugConsole() {
	CON_InitEx(rmode, 10, 10, 100, 100);
}


void Renderer::endFrame() {
    GX_DrawDone();          // Tell the GX engine we are done drawing
    GX_InvalidateTexAll();

    selectFrameBuffer ^= 1;  // Toggle framebuffer index

    GX_SetZMode      (GX_TRUE, GX_LEQUAL, GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp      (frameBuffers[selectFrameBuffer], GX_TRUE);

    VIDEO_SetNextFramebuffer(frameBuffers[selectFrameBuffer]);  // Select eXternal Frame Buffer
    VIDEO_Flush();                      // Flush video buffer to screen
    VIDEO_WaitVSync();
}

void * Renderer::frameBuffer;
void * Renderer::frameBuffers[2];
int Renderer::selectFrameBuffer;
void * Renderer::gp_fifo = nullptr;

GXRModeObj * Renderer::rmode;
GXColor Renderer::background = {255,255,255,255}; // blue = {0x29, 0xae, 0xea, 0xff}; // blue  {0x80, 0x80, 0x80, 0xff} //nice fog


void Renderer::renderBloc(const guVector &coord, u32 code,
						  bool top, bool bottom, bool left, bool right, bool front, bool back) {
	Mtx model, modelview, norm; // Various matrices
	
	guMtxIdentity(model);

    u32 white = 0xFFFFFFFF;
	
	guMtxTransApply(model, model, coord.x, coord.y, coord.z);
	
	guMtxConcat(camera.viewMatrix, model, modelview);
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    guMtxInverse(modelview, norm);
    guMtxTranspose(norm, modelview);
    GX_LoadNrmMtxImm(modelview, GX_PNMTX0);
	
	int sz = (top + bottom + left + right + front + back) << 2;
	f32 x, y;
	
	GX_Begin(GX_QUADS, GX_VTXFMT0, sz); // Start drawing
	
	// Bottom face
	if (bottom) {
		x = blocData[code].x[BLOC_FACE_BOTTOM];
		y = blocData[code].y[BLOC_FACE_BOTTOM];

		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, -1, 0);
  GX_Color1u32(white);		////GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
		
		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, -1, 0);
        GX_Color1u32(white);

//		//GX_TexCoord2f32(x, y + OFFSET); // Top left
		
		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, -1, 0);
        GX_Color1u32(white);
//		//GX_TexCoord2f32(x, y); // Bottom left
		
		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, -1, 0);
        GX_Color1u32(white);
//		//GX_TexCoord2f32(x + OFFSET, y); // Bottom right
	}
	
	// Front face
	if (front) {
		x = blocData[code].x[BLOC_FACE_FRONT];
		y = blocData[code].y[BLOC_FACE_FRONT];
		
		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color1u32(white);
		//GX_TexCoord2f32(x, y); // Bottom left
		
		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y); // Bottom right
		
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
        GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
		
		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 0, 1);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y + OFFSET); // Top left
	}
	
	// Back face
	if (back) {
		x = blocData[code].x[BLOC_FACE_BACK];
		y = blocData[code].y[BLOC_FACE_BACK];
		
		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, -1);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y); // Bottom right
		
		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, -1);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
		
		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 0, -1);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y + OFFSET); // Top left
		
		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(0, 0, -1);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y); // Bottom left
	}
	
	// Right face
	if (right) {
		x = blocData[code].x[BLOC_FACE_RIGHT];
		y = blocData[code].y[BLOC_FACE_RIGHT];
		
		GX_Position3f32(0.0f, -1.0f, -1.0f);
		GX_Normal3f32(1, 0, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y); // Bottom right
		
		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(1, 0, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
		
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(1, 0, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y + OFFSET); // Top left
		
		GX_Position3f32(0.0f, -1.0f, 0.0f);
		GX_Normal3f32(1, 0, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y); // Bottom left
	}
	
	// Left face
	if (left) {
		x = blocData[code].x[BLOC_FACE_LEFT];
		y = blocData[code].y[BLOC_FACE_LEFT];
		
		GX_Position3f32(-1.0f, -1.0f, -1.0f);
		GX_Normal3f32(-1, 0, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y); // Bottom right
		
		GX_Position3f32(-1.0f, -1.0f, 0.0f);
		GX_Normal3f32(-1, 0, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y); // Top right
		
		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(-1, 0, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top left
		
		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(-1, 0, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y + OFFSET); // Bottom left
	}
	
	// Top face
	if (top) {
		x = blocData[code].x[BLOC_FACE_TOP];
		y = blocData[code].y[BLOC_FACE_TOP];
		
		GX_Position3f32(-1.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 1, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y + OFFSET); // Top left
		
		GX_Position3f32(-1.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 1, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x, y); // Bottom left
		
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		GX_Normal3f32(0, 1, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y); // Bottom right
		
		GX_Position3f32(0.0f, 0.0f, -1.0f);
		GX_Normal3f32(0, 1, 0);
  GX_Color1u32(white);
		//GX_TexCoord2f32(x + OFFSET, y + OFFSET); // Top right
	}
	
	GX_End();         // Done drawing quads
}
/*
Renderer::CacheChunk(const VerticalChunk& c){
	BlockType bk[16][16][16];
	
	
}*/
