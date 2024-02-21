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


void Wiimote::update(Player& player, World& w) {
    //printf("x: %lf, y: %lf, z: %lf\r", player.renderer.camera.pos.x, player.renderer.camera.pos.y, player.renderer.camera.pos.z);
    WPAD_ScanPads();
    int wiimote_connection_status, acc, norme;

    do {
        wiimote_connection_status = WPAD_Probe(chan, &type);
    } while (wiimote_connection_status != WPAD_ERR_NONE);

    wd = WPAD_Data(chan);
    if(!player.cameraLocked)
        player.handleRotation(wd);

    WPAD_Expansion(chan, &data);
    if(WPAD_ButtonsDown(chan) & WPAD_BUTTON_HOME)
        exit(1);

    t_coord coord(floor(player.renderer.camera.pos.x+1), floor(player.renderer.camera.pos.y), floor(player.renderer.camera.pos.z+1));
    if (player.gravity)
        player.handleGravity(w, coord);

    u16 actions = WPAD_ButtonsHeld(chan);
    if (actions & WPAD_BUTTON_PLUS)
        player.sprint = true;
    else
        player.sprint = false;

    bool isTargeting = player.getFocusedBlock(w);
    if(isTargeting){
//        if (actions & WPAD_BUTTON_MINUS)
//            player.destroyBlock(w);
//        else
//            player.breakingState = 0;


        if(actions & WPAD_BUTTON_B)
            player.placeBlock(w);

        if(WPAD_ButtonsDown(chan) & WPAD_BUTTON_1){
            if(player.cameraLocked) player.cameraLocked = false;
            else {
                player.cameraLocked = true;
                player.lockedBlockPos = player.focusedBlockPos;
            }
        }
    }
    player.placeDelay++;

    if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_LEFT)
        if (player.selected_spot > 0)
            player.selected_spot --;
    if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_RIGHT)
        if (player.selected_spot < 8)
            player.selected_spot ++;

    if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_DOWN)
        player.sneak = !player.sneak;

    if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_UP)
        player.inventory_open = !player.inventory_open;

    if (player.gravity){
        if (actions & WPAD_BUTTON_A){
            player.Jump();
        }
    }
    else {
        if (actions & WPAD_BUTTON_A)
            player.goUp(coord, w);
        if (actions & WPAD_BUTTON_B)
            player.goDown(coord, w);
    }

    if(player.cameraLocked){
        if(player.getFocusedBlockDistance() > 7)
            player.cameraLocked = false;
        else {
            player.renderer.camera.look.x = (f32) player.lockedBlockPos.x - player.renderer.camera.pos.x - 0.5f;
            player.renderer.camera.look.y =(f32) player.lockedBlockPos.y - player.renderer.camera.pos.y - 0.5f;
            player.renderer.camera.look.z = (f32) player.lockedBlockPos.z - player.renderer.camera.pos.z - 0.5f;
        }
    }

    if(wd->exp.type == WPAD_EXP_NUNCHUK) {
        joystick_t sticks = wd->exp.nunchuk.js;
        player.move(w, sticks);
        vec3w_t accel = wd->exp.nunchuk.accel;

        norme =sqrt(accel.y * accel.y + accel.x * accel.x + accel.z * accel.z);

        acc = abs( norme - last_accel);
        //printf("%d\r", acc);
        last_accel = norme;
        if (acc > 5 && isTargeting) {
            player.destroyBlock(w);
        }
        else {
            if (frame_cntr < 60)
                frame_cntr++;
            else {
                frame_cntr = 0;
                player.breakingState = 0;
            }
        }

    }

    if(player.cameraLocked)
        guVecNormalize(&player.renderer.camera.look);
    //printf(">lk : %f %f %f\r", player.renderer.camera.look.x, player.renderer.camera.look.y, player.renderer.camera.look.z);
    //printf(">lk : %f %f %f\r", player.renderer.camera.look.x, player.renderer.camera.look.y, player.renderer.camera.look.z);
}


void print_wiimote_connection_status(int wiimote_connection_status) {
    switch (wiimote_connection_status) {
        case WPAD_ERR_NO_CONTROLLER: printf(" Wiimote not connected\n");
            break;
        case WPAD_ERR_NOT_READY: printf(" Wiimote not ready\n");
            break;
        case WPAD_ERR_NONE: printf(" Wiimote ready\n");
            break;
        default: printf(" Unknown Wimote state %d\n", wiimote_connection_status);
    }
}

void print_wiimote_buttons(WPADData * wd) {
    printf(" Buttons down:\n ");
    if (wd->btns_h & WPAD_BUTTON_A) printf("A ");
    if (wd->btns_h & WPAD_BUTTON_B) printf("B ");
    if (wd->btns_h & WPAD_BUTTON_1) printf("1 ");
    if (wd->btns_h & WPAD_BUTTON_2) printf("2 ");
    if (wd->btns_h & WPAD_BUTTON_MINUS) printf("MINUS ");
    if (wd->btns_h & WPAD_BUTTON_HOME) printf("HOME ");
    if (wd->btns_h & WPAD_BUTTON_PLUS) printf("PLUS ");
    printf("\n ");
    if (wd->btns_h & WPAD_BUTTON_LEFT) printf("LEFT ");
    if (wd->btns_h & WPAD_BUTTON_RIGHT) printf("RIGHT ");
    if (wd->btns_h & WPAD_BUTTON_UP) printf("UP ");
    if (wd->btns_h & WPAD_BUTTON_DOWN) printf("DOWN ");
    printf("\n");
}
