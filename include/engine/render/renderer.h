//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_RENDERER_H
#define MIICRAFT_RENDERER_H

#include <gccore.h>

#define DEFAULT_FIFO_SIZE 262144  // (256 * 1024) // TODO : Need to be increased !!!

class Renderer {
	static void * gp_fifo;
public:
	static GXColor background; // blue
	static void * frameBuffer, *frameBuffers[2];
	static int selectFrameBuffer;
	static GXRModeObj * rmode;
	
	static void setupVideo();
	static void setupVtxDesc();
	
	static void setupDebugConsole();
	
	static void testRender();
	
	static void endFrame();
};

#endif //MIICRAFT_RENDERER_H
