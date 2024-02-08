//code by WinterMute
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <wiiuse/wpad.h>
#include <iostream>

#include "wiimote.h"
#include "engine/render/renderer.h"
#include "engine/render/camera.h"
#include "engine/render/light.h"

#include "texture.c"

#include "src/world/game.h"


int exiting = 0;

//Calling the function will end the while loop and properly exit the program to the HBChannel.
void reload(u32, void *) {
	exiting = 1;
}

//Calling the function will end the while loop and then properly shutdown the system
void shutdown() {
	exiting = 2;
}

int evctr = 0;

void countevs(int chan, const WPADData * data) {
	evctr++;
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
/*
void print_and_draw_wiimote_data() {
	//Makes the var wd point to the data on the wiimote
	WPADData * wd = WPAD_Data(0);
	
	printf(" Data->Err: %d\n", wd->err);
	printf(" IR Dots:\n");
	
	for (auto & i : wd->ir.dot) {
		if (i.visible) {
			printf(" %4d, %3d\n", i.rx, i.ry);
			drawdot(1024, 768, i.rx, i.ry, COLOR_YELLOW);
		} else {
			printf(" None\n");
		}
	}
	//ir.valid - TRUE is the wiimote is pointing at the screen, else it is false
	if (wd->ir.valid) {
		float theta = wd->ir.angle / 180.0 * M_PI;
		
		//ir.x/ir.y - The x/y coordinates that the wiimote is pointing to, relative to the screen.
		//ir.angle - how far (in degrees) the wiimote is twisted (based on ir)
		printf(" Cursor: %.02f,%.02f\n", wd->ir.x, wd->ir.y);
		printf(" @ %.02f deg\n", wd->ir.angle);
		
		drawdot(rmode->fbWidth, rmode->xfbHeight, wd->ir.x, wd->ir.y, COLOR_RED);
		drawdot(rmode->fbWidth, rmode->xfbHeight, wd->ir.x + 10 * sinf(theta),
		        wd->ir.y - 10 * cosf(theta), COLOR_BLUE);
	} else {
		printf(" No Cursor\n\n");
	}
	if (wd->ir.raw_valid) {
		//ir.z - How far away the wiimote is from the screen in meters
		printf(" Distance: %.02fm\n", wd->ir.z);
		//orient.yaw - The left/right angle of the wiimote to the screen
		printf(" Yaw: %.02f deg\n", wd->orient.yaw);
	} else {
		printf("\n\n");
	}
	printf(" Accel:\n");
	//accel.x/accel.y/accel.z - analog values for the accelleration of the wiimote
	//(Note: Gravity pulls downwards, so even if the wiimote is not moving,
	//one(or more) axis will have a reading as if it is moving "upwards")
	printf(" XYZ: %3d,%3d,%3d\n", wd->accel.x, wd->accel.y, wd->accel.z);
	//orient.pitch - how far the wiimote is "tilted" in degrees
	printf(" Pitch: %.02f\n", wd->orient.pitch);
	//orient.roll - how far the wiimote is "twisted" in degrees (uses accelerometer)
	printf(" Roll: %.02f\n", wd->orient.roll);
	
	print_wiimote_buttons(wd);
	
	if (wd->ir.raw_valid) {
		for (int i = 0; i < 2; i++) {
			drawdot(4, 4, wd->ir.sensorbar.rot_dots[i].x + 2, wd->ir.sensorbar.rot_dots[i].y + 2,
			        COLOR_GREEN);
		}
	}
	
	//if (wd->btns_h & WPAD_BUTTON_HOME) doreload = 1;
}*/

TPLFile TPLfile;
GXTexObj texture;


guVector InverseVector(const guVector& v){
    guVector vtemp;
    vtemp.x = - v.x;
    vtemp.y = - v.y;
    vtemp.z = - v.z;

    return vtemp;
}


void renderChunk(World& w, Renderer& renderer){
    t_coord pos(0,0,0);
    for(int offsetX = -1; offsetX<=1; offsetX ++){
        for(int offsetY= -1; offsetY<=1; offsetY++){
            for (int i = 0; i < 16; ++i) {

                pos.x = i+ offsetX * 16 + offsetY * 16;
                for (int j = 0; j < 128; ++j) {
                    pos.y = j;
                    for (int k = 0; k < 16; ++k) {
                        pos.z = k + offsetX * 16 + offsetY * 16;
                        if (w.getBlockAt(pos).type != BlockType::Air)
                        {
                            //printf("Start Rendering : x : %d, y: %d, z: %d\r", i, j, k);

                            renderer.renderBloc({static_cast<f32>(i + offsetX * 16 + offsetY * 16), static_cast<f32>(j), static_cast<f32>(k + offsetX * 16 + offsetY * 16)}, 1);
                            //printf("End Rendering : x : %d, y: %d, z: %d\r", i, j, k);

                        }
                    }
                }
            }
        }
    }


}


int main(int argc, char ** argv) {
	u32 type;
	
	PAD_Init();
	WPAD_Init();
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc();
	Renderer::setupMisc();
	

	//Light light;
	//GX_InvalidateTexAll();
	Renderer renderer;
	
	TPL_OpenTPLFromMemory(&TPLfile, (void *)texture_data, texture_sz);
	TPL_GetTexture(&TPLfile, 0, &texture);
    GX_InitTexObjLOD(&texture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
	GX_SetTevOp(GX_TEVSTAGE0,GX_MODULATE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
	GX_LoadTexObj(&texture, GX_TEXMAP0);
	//GX_InitTexObjFilterMode(&texture, GX_NEAR, GX_NEAR);
	
	setupWiimote();

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	//SYS_STDIO_Report(true);
	renderer.camera.pos.z = 5;
	renderer.camera.pos.x = -0.5;
	renderer.camera.pos.y = 0.5;
	bool txx = true;

    //pour rediriger stdout dans dolphin
    SYS_STDIO_Report(true);
    Game g;
    t_coord pos(0,0,0);
    World w = g.getWorld();

	while (!exiting) {
        //VIDEO_ClearFrameBuffer(rmode,xfb[fbi],COLOR_BLACK);

        WPAD_ScanPads();
        if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);

        struct expansion_t data;

        WPAD_Expansion(WPAD_CHAN_0, &data); // Get expansion info from the first wiimote


        s8 tpad;
        u16 directions = WPAD_ButtonsHeld(0);

        guVector move = {0,0,0};
        guVector lookN;
        lookN.x = renderer.camera.look.x;
        lookN.y = renderer.camera.look.y;
        lookN.z = renderer.camera.look.z;
		
        guVecNormalize(&lookN);

        if ( directions & WPAD_BUTTON_LEFT ) {
            guVecCross(&lookN, &renderer.camera.up, &move);
            move = InverseVector(move);
        }
        if ( directions & WPAD_BUTTON_RIGHT ) {
            guVecCross(&lookN, &renderer.camera.up, &move);
        }
        if ( directions & WPAD_BUTTON_UP ) move = lookN;
        if ( directions & WPAD_BUTTON_DOWN ) move = InverseVector(lookN);

        renderer.camera.pos.x += move.x/10;
        renderer.camera.pos.z += move.z/10;

        if ( directions & WPAD_BUTTON_A) renderer.camera.pos.y += 0.1;
        if ( directions & WPAD_BUTTON_B) renderer.camera.pos.y -= 0.1;

/*
        if ( yrot > 360.f ) yrot -= 360.f;
        if ( yrot < 0 ) yrot += 360.f;*/

        if (data.type == WPAD_EXP_NUNCHUK) { // Ensure there's a nunchuk

            tpad = data.nunchuk.js.pos.x - data.nunchuk.js.center.x;
            if ((tpad < -8) || (tpad > 8)) renderer.camera.pos.x += (f32) tpad / 10;

            tpad = data.nunchuk.js.pos.y - data.nunchuk.js.center.y;
            if ((tpad < -8) || (tpad > 8)) renderer.camera.pos.z += (f32) tpad / 10;

        }
		//renderer.camera.rotateV(-0.10);
		//renderer.camera.rotateH(0.50);
		//camera.rotateH(1);
		renderer.camera.update(false);
		//renderer.renderBloc({-1, 0, 0}, 1);
		//renderer.renderBloc({0, 0, -1}, 1);
		//renderer.renderBloc({0, -1, 0}, 1);
		renderer.renderBloc({1, 0, 0}, 1);
		renderer.renderBloc({0, 0, }, 1);
        //renderChunk(w, renderer);
		//renderer.renderBloc({4, 0, 0}, 1);
		//renderer.renderBloc({7, -1, 0}, 1);
		//renderer.renderBloc({8, 0, 0}, 1);
		//renderer.renderBloc({9, -1, 0}, 1);
		//renderer.renderBloc({1, -1, 0}, 2);
		//renderer.renderBloc({0, -1, 1}, 2);
		//renderer.renderBloc({0, 0, 1}, 3);


		//for (int X = -20; X < 20; X++) {
		//	for (int Z = -20; Z < 20; Z++) {
		//		renderer.renderBloc({static_cast<f32>(X), 0, static_cast<f32>(Z)}, 1);
		//	}
		//}
		//light.update(camera.viewMatrix);
		
		//testRender();
		//setupDebugConsole();
		
		//drawdot(rmode->fbWidth, rmode->xfbHeight, 0, 0, COLOR_YELLOW);

		//Renderer::setupDebugConsole();
		WPAD_ReadPending(WPAD_CHAN_ALL, countevs);
		int wiimote_connection_status = WPAD_Probe(0, &type);
		
		//print_wiimote_connection_status(wiimote_connection_status);
		
		if (wiimote_connection_status == WPAD_ERR_NONE) {
            WPADData * wd = WPAD_Data(0);
             if (wd->ir.valid) {
                 if(wd->ir.x <  Renderer::rmode->fbWidth/2 - Renderer::rmode->fbWidth/4)
                     renderer.camera.rotateH( M_PI / 8);
                 if(wd->ir.x >  Renderer::rmode->fbWidth/2 + Renderer::rmode->fbWidth/4)
                     renderer.camera.rotateH(- M_PI / 8);
                 if(wd->ir.y <  Renderer::rmode->xfbHeight/2 - Renderer::rmode->xfbHeight/4)
                     renderer.camera.rotateV(- M_PI / 8);
                 if(wd->ir.y >  Renderer::rmode->xfbHeight/2 + Renderer::rmode->xfbHeight/4)
                     renderer.camera.rotateV(M_PI / 8);
             }
        }
		
		Renderer::endFrame();
	}
	
	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	
	return 0;
}
