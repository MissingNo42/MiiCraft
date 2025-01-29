#pragma once

#include <gccore.h>

#include "cacheUnit.h"
#include "camera.h"
#include "engine/env/environment.h"
#include "world/block.h"
#include "world/Chunk.h"

#define DEFAULT_FIFO_SIZE 262144  // (256 * 1024)  TODO: check if accurate

class Renderer {
	static void * gp_fifo;
	static Environment environment;

public:
	Camera camera;

	static constexpr GXColor background{0xff, 0xff, 0xff, 0xff}; // white
	static void * frameBuffer, *frameBuffers[2];
	static int selectFrameBuffer;
	static GXRModeObj * rmode;

	static GXTexObj mainTexture ATTRIBUTE_ALIGN(32);
    static GXTexObj nightTexture ATTRIBUTE_ALIGN(32);

	static void setupVideo();

	static void setVertexFormat(bool advanced = false);

	static void setupVertexAttributeTable();

	static void setShader(bool advance = false);

	static void setupTexture();

	static void setClearColor(const GXColor color = background) {
		GX_SetCopyClear(color, GX_MAX_Z24);
	}

	static void endFrame();

	static void renderSplashScreen();

	void renderBlock(const guVector &coord, BlockType type, u8 lt = Air, u8 lb = Air, u8 lf = Air, u8 lk = Air, u8 lr = Air, u8 ll = Air);

    void renderFocus(f32 x, f32 y, f32 z);
    void renderVector(f32 x, f32 y, f32 z, u16 color);

	static void renderChunk(Chunk& c);
	static void renderRect(f32 x1, f32 y1, f32 x2, f32 y2, u16 lt, u16 rt, u16 lb, u16 rb);
	static void renderValue(f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, u8 value);
	static void Underwater();

	static void animateWater() {
		environment.textureControl.animateWater();
	}

	static void update() {
		environment.update();
	}

	void renderSky() const {
		environment.skyControl.render(camera);
	}
};
