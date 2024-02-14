//
// Created by Romain on 04/02/2024.
//
#define deadzone 20
#define deadzoneTop 80


#include <cstdio>
#include "wiimote.h"


void Wiimote::handleRotation(Camera& camera) {
    if (wd->ir.valid) {
        if(wd->ir.x <  (f32) Renderer::rmode->fbWidth/2 - deadzone)
        {
            f32 angle = wd->ir.x / (f32) ((f32) Renderer::rmode->fbWidth/2 - deadzone) * 6;
            camera.rotateH( M_PI /(2 + angle));
        }
        if(wd->ir.x >  (f32) Renderer::rmode->fbWidth/2 + deadzone)
        {
            f32 angle = (wd->ir.x - (f32) Renderer::rmode->fbWidth/2 + deadzone) / (f32) ((f32) Renderer::rmode->fbWidth/2 - deadzone) * 6;
            camera.rotateH( - (f32) M_PI /(8 - angle));
        }
        if(wd->ir.y <  (f32) Renderer::rmode->xfbHeight/2 - deadzoneTop)
        {
            f32 angle = wd->ir.y / (f32) ((f32) Renderer::rmode->xfbHeight/2 - deadzoneTop) * 6;
            camera.rotateV( - (f32) M_PI /(2 +angle));
        }
        if(wd->ir.y >  (f32) Renderer::rmode->xfbHeight/2 + deadzone)
        {
            f32 angle = (wd->ir.y - (f32) Renderer::rmode->xfbHeight/2 + deadzone) / ((f32) Renderer::rmode->xfbHeight/2 - deadzone) * 6;
            camera.rotateV( M_PI /(8 - angle));
        }
    }
}

bool Wiimote::update(Camera &camera, World& w) {

    WPAD_ScanPads();
    if(WPAD_ButtonsDown(chan) & WPAD_BUTTON_HOME) return true;

    WPAD_Expansion(chan, &data); // Get expansion info from the first wiimote

    hold = WPAD_ButtonsHeld(chan);
	pressed = WPAD_ButtonsDown(chan);
	released = WPAD_ButtonsUp(chan);
	
    handleMovement(camera, w);
    //WPAD_ReadPending(WPAD_CHAN_ALL, countevs);
    int wiimote_connection_status = WPAD_Probe(0, &type);

    if (wiimote_connection_status == WPAD_ERR_NONE) {
        wd = WPAD_Data(0);
        handleRotation(camera);
    }
	
	return false;
}


void Wiimote::handleMovement(Camera& camera, World& w) {
    guVector normalizedLook = camera.look;
    guVecNormalize(&normalizedLook);
    t_coord coord((int)camera.pos.x+1, (int)camera.pos.y, (int)camera.pos.z+1);

	bool collision = false;
	
    speed = 1;
    if ( hold & WPAD_BUTTON_PLUS)
        speed = 2;
    if ( hold & WPAD_BUTTON_LEFT )
        camera.goLeft(normalizedLook, speed, collision, w);
    if ( hold & WPAD_BUTTON_RIGHT )
        camera.goRight(normalizedLook, speed, collision, w);
    if ( hold & WPAD_BUTTON_UP )
        camera.goForward(normalizedLook, speed, collision, w);
    if ( hold & WPAD_BUTTON_DOWN )
        camera.goBackward(normalizedLook,speed, collision, w);
    //if ( directions & WPAD_BUTTON_PLUS)
    //    speed = 3;
    //if ( directions & WPAD_BUTTON_LEFT )
    //    renderer.camera.goLeft(normalizedLook, speed, collision, w);
    //if ( directions & WPAD_BUTTON_RIGHT )
    //    renderer.camera.goRight(normalizedLook, speed, collision, w);
    //if ( directions & WPAD_BUTTON_UP )
    //    renderer.camera.goForward(normalizedLook, speed, collision, w);
    //if ( directions & WPAD_BUTTON_DOWN )
    //    renderer.camera.goBackward(normalizedLook, speed, collision, w);

    if ( hold & WPAD_BUTTON_A ) {
        if ( collision ){
            coord.y += 1;
            if (w.getBlockAt(coord).type == BlockType::Air)
                camera.pos.y += 0.1;
        }
        else
            camera.pos.y += 0.1;
        camera.update(true);
        }

    if ( hold & WPAD_BUTTON_B) {
        if ( collision ){
            coord.y -= 1;
            if (w.getBlockAt(coord).type == BlockType::Air)
                camera.pos.y -= 0.1;
        }
        else
            camera.pos.y -= 0.1;
        camera.update(true);
    }
}


Wiimote::Wiimote(int chan): chan(chan) {
    WPAD_SetDataFormat(chan, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(chan, Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight);
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
