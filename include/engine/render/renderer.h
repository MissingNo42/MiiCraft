//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_RENDERER_H
#define MIICRAFT_RENDERER_H

#include <gccore.h>
#include "camera.h"
#include "../../../src/world/block.h"

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
	
	static void endFrame();
	
	void renderBloc(const guVector &coord, u32 code,
					int top, int bottom, int left, int right, int front, int back,
                    int topVal = 0, int bottomVal = 0, int leftVal = 0, int rightVal = 0, int frontVal = 0, int backVal = 0,
                    int topIndex = 0, int bottomIndex = 0, int leftIndex = 0, int rightIndex = 0, int frontIndex = 0, int backIndex = 0);

    void drawFocus(BlockType block, f32 x, f32 y, f32 z);


    //Generate a grey color gradient (with 1 the darkest (not black)
    u32 shadowGradient[16] = {
           0x0e0e0eff,
           0x131313ff,
           0x171717ff,
           0x1c1c1cff,
           0x222222ff,
           0x292929ff,
           0x303030ff,
           0x393939ff,
           0x434343ff,
           0x4f4f4fff,
           0x5d5d5dff,
           0x6e6e6eff,
           0x848484ff,
           0xa0a0a0ff,
           0xc5c5c5ff,
           0xfafafaff,
    };


};

#endif //MIICRAFT_RENDERER_H
