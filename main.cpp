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

#include "wiimote.h"
#include "engine/render/renderer.h"
#include "engine/render/camera.h"
#include "engine/render/light.h"
#include "pl.h"
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

//Draw a square on the screen (May draw rectangles as well, I am uncertain).
//*xfb - framebuffer
//*rmode - the current video mode (# lines,progressive or interlaced, NTSC or PAL etc.) see libogc/gc/ogc/gx_struct.h
// for the definition
//w - Width of screen (Used as scale factor in converting fx to pixel coordinates)
//h - Height of screen (Used as scale factor in converting fy to pixel coordinates)
//fx - X coordinate to draw on the screen (0-w)
//fy - Y coordinate to draw on the screen (!unsure!-h)
//color - the color of the rectangle (Examples: COLOR_YELLOW, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_BLACK, COLOR_WHITE)
void drawdot(float w, float h, float fx, float fy, u32 color) {

    //*fb - !unsure!
    //px - !unsure!
    //py - !unsure!
    //x - !unsure!
    //y - !unsure!

    u32 * fb = (u32 *) Renderer::frameBuffer;
    int px, py;
    int x, y;

    y = fy * Renderer::rmode->xfbHeight / h;
    x = fx * Renderer::rmode->fbWidth / w / 2;

    for (py = y - 4; py <= (y + 4); py++) {
        if (py < 0 || py >= Renderer::rmode->xfbHeight)
            continue;
        for (px = x - 2; px <= (x + 2); px++) {
            if (px < 0 || px >= Renderer::rmode->fbWidth / 2)
                continue;
            fb[Renderer::rmode->fbWidth / VI_DISPLAY_PIX_SZ * py + px] = color;
        }
    }

}

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
		
		drawdot(Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight, wd->ir.x, wd->ir.y, COLOR_RED);
		drawdot(Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight, wd->ir.x + 10 * sinf(theta),
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
}

GXTexObj texture;

void loadTexture(const u8 *data, u32 width, u32 height) {
    GX_InitTexObj(&texture, (void *) data, width, height, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GX_LoadTexObj(&texture, GX_TEXMAP0);
}

guVector InverseVector(const guVector& v){
    guVector vtemp;
    vtemp.x = - v.x;
    vtemp.y = - v.y;
    vtemp.z = - v.z;

    return vtemp;
}


int main(int argc, char ** argv) {
	u32 type;
	
	PAD_Init();
	WPAD_Init();
	
	Renderer::setupVideo();
	Renderer::setupVtxDesc();

	//Light light;
	//GX_InvalidateTexAll();

	Camera camera;

	loadTexture(pl.pixel_data, 64, 64);

	setupWiimote();

	SYS_SetResetCallback(reload);
	SYS_SetPowerCallback(shutdown);
	camera.pos.z = 8;
    camera.pos.y = 70;
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
        lookN.x = camera.look.x;
        lookN.y = camera.look.y;
        lookN.z = camera.look.z;
        printf("x : %lf, y: %lf, z: %lf\n", lookN.x, lookN.y, lookN.z);
        guVecNormalize(&lookN);
        if ( directions & WPAD_BUTTON_LEFT ) {
            guVecCross(&lookN, &camera.up, &move);
            move = InverseVector(move);
        }
        if ( directions & WPAD_BUTTON_RIGHT ) {
            guVecCross(&lookN, &camera.up, &move);
        }
        if ( directions & WPAD_BUTTON_UP ) move = lookN;
        if ( directions & WPAD_BUTTON_DOWN ) move = InverseVector(lookN);

        camera.pos.x += move.x/10;
        camera.pos.z += move.z/10;

        if ( directions & WPAD_BUTTON_A) camera.pos.y += 0.1;
        if ( directions & WPAD_BUTTON_B) camera.pos.y -= 0.1;

/*
        if ( yrot > 360.f ) yrot -= 360.f;
        if ( yrot < 0 ) yrot += 360.f;*/

        if (data.type == WPAD_EXP_NUNCHUK) { // Ensure there's a nunchuk

            tpad = data.nunchuk.js.pos.x - data.nunchuk.js.center.x;
            if ((tpad < -8) || (tpad > 8)) camera.pos.x += (f32) tpad / 10;

            tpad = data.nunchuk.js.pos.y - data.nunchuk.js.center.y;
            if ((tpad < -8) || (tpad > 8)) camera.pos.z += (f32) tpad / 10;

        }
/*
		Renderer::renderBloc({-1, 0, 0});
		Renderer::renderBloc({0, 0, -1});
		Renderer::renderBloc({0, -1, 0});
		Renderer::renderBloc({1, 0, 0});
		Renderer::renderBloc({0, 0, 1});
		Renderer::renderBloc({0, 1, 0});
*/






        for (int i = 0; i < 16; ++i) {
            pos.x = i;
            for (int j = 0; j < 128; ++j) {
                pos.y = j;
                for (int k = 0; k < 16; ++k) {
                    pos.z = k;
                    if (w.getBlockAt(pos).type != BlockType::Air)
                    {
                        //printf("Start Rendering : x : %d, y: %d, z: %d\r", i, j, k);

                        Renderer::renderBloc({static_cast<f32>(i), static_cast<f32>(j), static_cast<f32>(k)});
                        //printf("End Rendering : x : %d, y: %d, z: %d\r", i, j, k);

                    }
                }
            }
        }

		//light.update(camera.viewMatrix);
		
		//testRender();

		//Renderer::setupDebugConsole();
		WPAD_ReadPending(WPAD_CHAN_ALL, countevs);
		int wiimote_connection_status = WPAD_Probe(0, &type);
		
		//print_wiimote_connection_status(wiimote_connection_status);
		
		if (wiimote_connection_status == WPAD_ERR_NONE) {
            WPADData * wd = WPAD_Data(0);
             if (wd->ir.valid) {
                 if(wd->ir.x <  Renderer::rmode->fbWidth/2 - Renderer::rmode->fbWidth/4)
                     camera.rotateH( M_PI / 8);
                 if(wd->ir.x >  Renderer::rmode->fbWidth/2 + Renderer::rmode->fbWidth/4)
                     camera.rotateH(- M_PI / 8);
                 if(wd->ir.y <  Renderer::rmode->xfbHeight/2 - Renderer::rmode->xfbHeight/4)
                     camera.rotateV(- M_PI / 8);
                 if(wd->ir.y >  Renderer::rmode->xfbHeight/2 + Renderer::rmode->xfbHeight/4)
                     camera.rotateV(M_PI / 8);
             }
        }
		
		//drawdot(Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight, 0, 0, COLOR_YELLOW);
        camera.update();
		Renderer::endFrame();
	}
	
	if (exiting == 2) SYS_ResetSystem(SYS_SHUTDOWN, 0, 0);
	
	return 0;
}
