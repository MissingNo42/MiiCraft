//
// Created by Romain on 04/02/2024.
//
#define deadzone 20
#define deadzoneTop 80


#include <cstdio>
#include "wiimote.h"

Wiimote::Wiimote() {
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_ALL, Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight);
}

void Wiimote::update(Player& player) {
    //printf("x: %lf, y: %lf, z: %lf\r", player.renderer.camera.pos.x, player.renderer.camera.pos.y, player.renderer.camera.pos.z);
    WPAD_ScanPads();
    int wiimote_connection_status;

    do {
        wiimote_connection_status = WPAD_Probe(chan, &type);
    } while (wiimote_connection_status != WPAD_ERR_NONE);

    wd = WPAD_Data(chan);
    player.handleRotation(wd);

    WPAD_Expansion(chan, &data);
    if(WPAD_ButtonsDown(chan) & WPAD_BUTTON_HOME)
        exit(1);

    BlockCoord coord(floor(player.renderer.camera.pos.x + 1), floor(player.renderer.camera.pos.y), floor(player.renderer.camera.pos.z + 1));
    if (player.gravity)
        player.handleGravity(coord);

    u16 actions = WPAD_ButtonsHeld(chan);
    if (actions & WPAD_BUTTON_PLUS)
        player.sprint = true;
    else
        player.sprint = false;

    player.getFocusedBlock();

    if (actions & WPAD_BUTTON_MINUS && player.isTargeting)
        player.destroyBlock();
    else
        player.breakingState = 0;

    if(actions & WPAD_BUTTON_B && player.isTargeting) {
	    player.placeBlock();
    }
    player.placeDelay++;

    if (player.gravity){
        if (actions & WPAD_BUTTON_A){
            player.Jump();
        }
    }
    else {
        if (actions & WPAD_BUTTON_A)
            player.goUp(coord);
        if (actions & WPAD_BUTTON_B)
            player.goDown(coord);
    }
    if(wd->exp.type == WPAD_EXP_NUNCHUK) {
        joystick_t sticks = wd->exp.nunchuk.js;
        player.move(sticks);
    }
}
