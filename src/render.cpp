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
guVector cam = {0.0F, 0.0F, 0.0F},
		up = {0.0F, 1.0F, 0.0F},
		look = {0.0F, 0.0F, -1.0F};

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
	//GX_LoadPosMtxImm(modelview, GX_PNMTX0);

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
		CON_Init(xfb, 0, 0, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
}
