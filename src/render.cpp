//
// Created by Romain on 04/02/2024.
//

#include <gccore.h>
#include <malloc.h>
#include <cstring>

#include "render.h"

void * xfb, * xfbs[2];
GXRModeObj * rmode;
int select_fb = 0;


Mtx view;
Mtx44 perspective;
Mtx model, modelview;

void setupVideo() {
	
	VIDEO_Init();
	
	rmode = VIDEO_GetPreferredMode(nullptr);
	
	// double buffering, prevents flickering
	xfb = xfbs[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	xfbs[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_SetBlack(false);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	// setup the fifo and then init the flipper
	void * gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);
	
	GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
	
	// clears the bg to color and clears the z buffer
	GXColor background = {0, 0, 0x60, 0xff};
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
	
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(xfb, GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
	
	
	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	
	// setup the vertex attribute table
	// describes the data
	// args: vat location 0-7, type of data, data format, size, scale
	// so for ex. in the first call we are sending position data with
	// 3 values X,Y,Z of size F32. scale sets the number of fractional
	// bits for non float data.
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);
	
	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	
	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam = {0.0F, 0.0F, 0.0F},
			up = {0.0F, 1.0F, 0.0F},
			look = {0.0F, 0.0F, -1.0F};
	guLookAt(view, &cam, &up, &look);
	
	
	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 90,
	// and aspect ratio based on the display resolution
	f32 w = rmode->viWidth;
	f32 h = rmode->viHeight;
	guPerspective(perspective, 45, (f32) w / h, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
}

void testRender() {
	
	guVector triAxis = {0,1,0};
	guVector cubeAxis = {1,1,1};

	// do this before drawing
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);

	guMtxIdentity(model);
	guMtxRotAxisDeg(model, &triAxis, .5);
	guMtxTransApply(model, model, -1.5f,0.0f,-6.0f);
	guMtxConcat(view,model,modelview);
	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 12);		// Draw A Pyramid

		GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top of Triangle (front)
		GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
		GX_Position3f32(-1.0f,-1.0f, 1.0f);	// Left of Triangle (front)
		GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green
		GX_Position3f32( 1.0f,-1.0f, 1.0f);	// Right of Triangle (front)
		GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue

		GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top of Triangle (Right)
		GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
		GX_Position3f32( 1.0f,-1.0f, 1.0f);	// Left of Triangle (Right)
		GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
		GX_Position3f32( 1.0f,-1.0f,-1.0f);	// Right of Triangle (Right)
		GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green

		GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top of Triangle (Back)
		GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
		GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Left of Triangle (Back)
		GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
		GX_Position3f32( 1.0f,-1.0f,-1.0f);	// Right of Triangle (Back)
		GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green

		GX_Position3f32( 0.0f, 1.0f, 0.0f);		// Top of Triangle (Left)
		GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
		GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Left of Triangle (Left)
		GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
		GX_Position3f32(-1.0f,-1.0f, 1.0f);	// Right of Triangle (Left)
		GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green

	GX_End();

	guMtxIdentity(model);
	guMtxRotAxisDeg(model, &cubeAxis, .5);
	guMtxTransApply(model, model, 1.5f,0.0f,-7.0f);
	guMtxConcat(view,model,modelview);
	// load the modelview matrix into matrix memory
	GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	GX_Begin(GX_QUADS, GX_VTXFMT0, 24);			// Draw a Cube

		GX_Position3f32( 1.0f, 1.0f,-1.0f);	// Top Left of the quad (top)
		GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green
		GX_Position3f32(-1.0f, 1.0f,-1.0f);	// Top Right of the quad (top)
		GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green
		GX_Position3f32(-1.0f, 1.0f, 1.0f);	// Bottom Right of the quad (top)
		GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green
		GX_Position3f32( 1.0f, 1.0f, 1.0f);		// Bottom Left of the quad (top)
		GX_Color3f32(0.0f,1.0f,0.0f);			// Set The Color To Green

		GX_Position3f32( 1.0f,-1.0f, 1.0f);	// Top Left of the quad (bottom)
		GX_Color3f32(1.0f,0.5f,0.0f);			// Set The Color To Orange
		GX_Position3f32(-1.0f,-1.0f, 1.0f);	// Top Right of the quad (bottom)
		GX_Color3f32(1.0f,0.5f,0.0f);			// Set The Color To Orange
		GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Bottom Right of the quad (bottom)
		GX_Color3f32(1.0f,0.5f,0.0f);			// Set The Color To Orange
		GX_Position3f32( 1.0f,-1.0f,-1.0f);	// Bottom Left of the quad (bottom)
		GX_Color3f32(1.0f,0.5f,0.0f);			// Set The Color To Orange

		GX_Position3f32( 1.0f, 1.0f, 1.0f);		// Top Right Of The Quad (Front)
		GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
		GX_Position3f32(-1.0f, 1.0f, 1.0f);	// Top Left Of The Quad (Front)
		GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
		GX_Position3f32(-1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Front)
		GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red
		GX_Position3f32( 1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Front)
		GX_Color3f32(1.0f,0.0f,0.0f);			// Set The Color To Red

		GX_Position3f32( 1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Back)
		GX_Color3f32(1.0f,1.0f,0.0f);			// Set The Color To Yellow
		GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Back)
		GX_Color3f32(1.0f,1.0f,0.0f);			// Set The Color To Yellow
		GX_Position3f32(-1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Back)
		GX_Color3f32(1.0f,1.0f,0.0f);			// Set The Color To Yellow
		GX_Position3f32( 1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Back)
		GX_Color3f32(1.0f,1.0f,0.0f);			// Set The Color To Yellow

		GX_Position3f32(-1.0f, 1.0f, 1.0f);	// Top Right Of The Quad (Left)
		GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
		GX_Position3f32(-1.0f, 1.0f,-1.0f);	// Top Left Of The Quad (Left)
		GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
		GX_Position3f32(-1.0f,-1.0f,-1.0f);	// Bottom Left Of The Quad (Left)
		GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue
		GX_Position3f32(-1.0f,-1.0f, 1.0f);	// Bottom Right Of The Quad (Left)
		GX_Color3f32(0.0f,0.0f,1.0f);			// Set The Color To Blue

		GX_Position3f32( 1.0f, 1.0f,-1.0f);	// Top Right Of The Quad (Right)
		GX_Color3f32(1.0f,0.0f,1.0f);			// Set The Color To Violet
		GX_Position3f32( 1.0f, 1.0f, 1.0f);		// Top Left Of The Quad (Right)
		GX_Color3f32(1.0f,0.0f,1.0f);			// Set The Color To Violet
		GX_Position3f32( 1.0f,-1.0f, 1.0f);	// Bottom Left Of The Quad (Right)
		GX_Color3f32(1.0f,0.0f,1.0f);			// Set The Color To Violet
		GX_Position3f32( 1.0f,-1.0f,-1.0f);	// Bottom Right Of The Quad (Right)
		GX_Color3f32(1.0f,0.0f,1.0f);			// Set The Color To Violet

	GX_End();									// Done Drawing The Quad

	// do this stuff after drawing
	GX_DrawDone();

}

void setupDebugConsole() {
	//CON_InitEx(rmode, 10, 10, rmode->fbWidth / 4, rmode->xfbHeight - 20);
}
