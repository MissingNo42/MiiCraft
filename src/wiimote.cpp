//
// Created by Romain on 04/02/2024.
//

#include <cmath>
#include "wiimote.h"
#include "render/renderer.h"

void Wiimote::setup() {
	PAD_Init();
	WPAD_Init();
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_ALL, Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight);
}


void Wiimote::update() {
    WPAD_ScanPads();
	
	connected = WPAD_Probe(chan, &type) == WPAD_ERR_NONE;

    wd = WPAD_Data(chan);
	
    x = 2 * wd->ir.x / (f32)Renderer::rmode->fbWidth - 1;
    y = -2 * wd->ir.y / (f32)Renderer::rmode->xfbHeight + 1;

	if (wd->btns_d & WPAD_BUTTON_HOME) quit = true;
	
	WPAD_Expansion(chan, &data);
	
	
	if (wd->exp.type == WPAD_EXP_NUNCHUK) {
		vec3w_t acc = wd->exp.nunchuk.accel;
		
		f32 norm = std::sqrt((f32) acc.y * (f32) acc.y + (f32) acc.x * (f32) acc.x + (f32) acc.z * (f32) acc.z);
		
		accel = std::abs(norm - last_accel);
		
		last_accel = norm;
	}
}

bool Wiimote::quit = false;

Wiimote::Wiimote(int chan) : chan(chan) {
}
