//
// Created by Romain on 04/02/2024.
//

#ifndef MIICRAFT_RENDER_H
#define MIICRAFT_RENDER_H

#include <gccore.h>

#define DEFAULT_FIFO_SIZE 262144  // (256 * 1024)

extern void * xfb, *xfbs[2];
extern int select_fb;
extern GXRModeObj * rmode;

void setupVideo();

void setupDebugConsole();

void testRender();

inline void flushFramebuffer() {
	select_fb ^= 1;
	xfb = xfbs[select_fb];
	
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(xfb, GX_TRUE);
	
	VIDEO_SetNextFramebuffer(xfb);
	VIDEO_Flush();
	VIDEO_WaitVSync();
}

#endif //MIICRAFT_RENDER_H
