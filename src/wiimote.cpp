//
// Created by Romain on 04/02/2024.
//

#include "wiimote.h"
#include "engine/render/renderer.h"

void setupWiimote() {
	WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight);
}
