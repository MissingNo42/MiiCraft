//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_RENDERER_H
#define MIICRAFT_RENDERER_H

#include <gccore.h>
#include "camera.h"

#define DEFAULT_FIFO_SIZE 262144 // (256 * 1024) // TODO : Need to be increased !!!

class Renderer {
	static void * gp_fifo;
	
public:
	Camera camera;
	
	static GXColor background; // blue
	static void * frameBuffer, *frameBuffers[2];
	static int selectFrameBuffer;
	static GXRModeObj * rmode;
	
	static void setupVideo();
    void setup3dMode(f32 minDist = 0.1, f32 maxDist = 300, f32 fov = 45);
    void setup2dMode() const;
	static void setupVtxDesc();
	static void setupMisc();
    static int video_init();
	
	void setupDebugConsole();
	
	static void testRender();
	
	static void endFrame();
	
	void renderBloc(const guVector &coord, u32 code,
					bool top, bool bottom, bool left, bool right, bool front, bool back
					);
    void renderCross();
};

#endif //MIICRAFT_RENDERER_H
