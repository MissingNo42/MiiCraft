//
// Created by Romain on 05/02/2024.
//

#ifndef MIICRAFT_RENDERER_H
#define MIICRAFT_RENDERER_H

#include <gccore.h>
#include "camera.h"
#include "world/block.h"
#include "world/verticalChunk.h"

#define DEFAULT_FIFO_SIZE 262144  // (256 * 1024)

class Renderer {
	static void * gp_fifo;
	
public:
	Camera camera;
	
	static GXColor background;
	static void * frameBuffer, *frameBuffers[2];
	static int selectFrameBuffer;
	static GXRModeObj * rmode;

    static GXTexObj texture ATTRIBUTE_ALIGN(32);

	static void setupVideo();
	static void setupVtxDesc();
	static void setupTexture();
	
	static void setClearColor(GXColor color = background);
	
	static void endFrame();
	
	static void renderSplashScreen();
	
	void renderSky() const;
	
	void renderBlock(const guVector &coord, BlockType type, u8 lt = Air, u8 lb = Air, u8 lf = Air, u8 lk = Air, u8 lr = Air, u8 ll = Air);

    void renderFocus(f32 x, f32 y, f32 z);
	
	static void renderChunk(VerticalChunk& c);
};

#endif //MIICRAFT_RENDERER_H
