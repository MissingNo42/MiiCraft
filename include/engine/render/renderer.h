//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_RENDERER_H
#define MIICRAFT_RENDERER_H

#include <gccore.h>
#include "camera.h"
#include "world/block.h"

#define DEFAULT_FIFO_SIZE 262144  // (256 * 1024) // TODO : Need to be increased !!!

class Renderer {
	static void * gp_fifo;
	
public:
	Camera camera;
	
	static GXColor background; // blue
	static void * frameBuffer, *frameBuffers[2];
	static int selectFrameBuffer;
	static GXRModeObj * rmode;
	static GXTexObj texture ATTRIBUTE_ALIGN(32);
	
	static void setupVideo();
	static void setupVtxDesc();
	static void setupTexture();
	
	static void setupDebugConsole();
	
	static void endFrame();
	
	void renderBloc(const guVector &coord, u32 code,
					bool top, bool bottom, bool left, bool right, bool front, bool back
					);
	
	~Renderer();

    void drawFocus(Block block, f32 x, f32 y, f32 z);
};

#endif //MIICRAFT_RENDERER_H
