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
    int wiimote_connection_status, acc, norme;

    do {
        wiimote_connection_status = WPAD_Probe(chan, &type);
    } while (wiimote_connection_status != WPAD_ERR_NONE);

    wd = WPAD_Data(chan);

    if(player.inventory.pickedItem.item.type == BlockType::Air)
        player.inventory.pickedItem.quantity = 0;

    if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_UP){
        player.inventory.open = !player.inventory.open;
        if (player.inventory.craftOpen == true)
            player.inventory.craftOpen = false;
        if (!player.inventory.open){
            player.inventory.ClearCraft();
        }

    }

    BlockCoord coord(floor(player.renderer.camera.pos.x+1), floor(player.renderer.camera.pos.y), floor(player.renderer.camera.pos.z+1));
    if (player.inventory.open){
        f32 x = 2 * wd->ir.x / (f32)Renderer::rmode->fbWidth;
        f32 y = -2 * wd->ir.y / (f32)Renderer::rmode->xfbHeight;
        //printf("x: %f, y: %f\r", x, y);
        if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_LEFT && player.inventory.currentPage > 0)
            player.inventory.currentPage--;
        if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_RIGHT && player.inventory.currentPage < 2)
            player.inventory.currentPage++;

        int l, c, slot;
        bool isValidCursor = false;
        bool craftSlot = false;

        // Inventory
        if (x > 0.32 && x < 1.73 && y > -1.62 && y < -1) {
            l = -floor((1 + y) / 0.158) - 1;
            c = floor((0.32 + x) / 0.158) - 3;
            slot = l * 9 + c - 1;
            isValidCursor = true;
        }
        if(player.inventory.craftOpen) {
            if (x > 0.49 && x < 0.975 && y > -0.925 && y < -0.486) {
                l = -floor((0.486 + y) / 0.157) - 1;
                c = floor((0.49 + x) / 0.163) - 5;
                slot = l * 3 + c - 1;
                isValidCursor = true;
                craftSlot = true;
            } else if (x > 1.3 && x < 1.55 && y > -0.81 && y < -0.59) {
                slot = 9;
                isValidCursor = true;
                craftSlot = true;
            }
        } else {
            // Craft slots
			if (x > 1.11 && x < 1.425 && y > -0.8 && y < -0.5){
	            l = -floor((0.5 + y) / 0.158) - 1;
	            c = floor((1.1 + x) / 0.158) - 13;
	            slot = l * 3 + c - 1;
	            isValidCursor = true;
	            craftSlot = true;
	        }
	        // Craft result
	        else if ( x > 1.6 && x < 1.75 && y > - 0.72 && y < -0.58){
	            slot = 9;
	            isValidCursor = true;
	            craftSlot = true;
	        }
        }

        if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_A && isValidCursor)
            player.inventory.action(slot, craftSlot, 0, false, false, player.creative);
        else if(WPAD_ButtonsDown(chan) & WPAD_BUTTON_MINUS && isValidCursor)
            player.inventory.action(slot, craftSlot, 1, false, true, player.creative);
        else if(WPAD_ButtonsDown(chan) & WPAD_BUTTON_PLUS && isValidCursor)
            player.inventory.action(slot, craftSlot, 0, true, false, player.creative);
    }
    else {
    if(!player.cameraLocked)
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

    bool isTargeting = player.getFocusedBlock();
    if(isTargeting){
//        if (actions & WPAD_BUTTON_MINUS)
//            player.destroyBlock(w);
//        else
//            player.breakingState = 0;


        if(actions & WPAD_BUTTON_B){
            if (player.focusedBlockType == BlockType::CraftingTable){
                if (player.sneak){
                    player.placeBlock();
                }
                else {
                    player.inventory.craftOpen = true;
                    player.inventory.open = true;
                }
            }
            else {
                player.placeBlock();
            }
        }


        //if(WPAD_ButtonsDown(chan) & WPAD_BUTTON_1){
        //    if(player.cameraLocked) player.cameraLocked = false;
        //    else {
        //        player.cameraLocked = true;
        //        player.lockedBlockPos = player.focusedBlockPos;
        //    }
        //}
    }
    player.placeDelay++;

    if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_LEFT)
        if (player.inventory.selectedSlot > 0)
            player.inventory.selectedSlot --;
    if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_RIGHT)
        if (player.inventory.selectedSlot < 8)
            player.inventory.selectedSlot ++;

    if (WPAD_ButtonsDown(chan) & WPAD_BUTTON_DOWN)
        player.sneak = !player.sneak;

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
        player.move(sticks);
        vec3w_t accel = wd->exp.nunchuk.accel;

        norme = sqrt(accel.y * accel.y + accel.x * accel.x + accel.z * accel.z);

        acc = abs(norme - last_accel);

        last_accel = norme;
        if (acc > 5 && isTargeting) {
            player.destroyBlock();
        } else {
            if (frame_cntr < 60)
                frame_cntr++;
            else {
                frame_cntr = 0;
                player.breakingState = 0;
            }
        }
		
        if (IS_PRESSED((&(wd->exp.nunchuk)), NUNCHUK_BUTTON_Z)){
            player.cameraLocked = true;
            player.lockedBlockPos = player.focusedBlockPos;
        } else if (IS_RELEASED((&(wd->exp.nunchuk)), NUNCHUK_BUTTON_Z)) player.cameraLocked = false;
    }
    }
    if (player.gravity)
        player.handleGravity(coord);
    if(player.cameraLocked)
        guVecNormalize(&player.renderer.camera.look);
    //printf(">lk : %f %f %f\r", player.renderer.camera.look.x, player.renderer.camera.look.y, player.renderer.camera.look.z);
    //printf(">lk : %f %f %f\r", player.renderer.camera.look.x, player.renderer.camera.look.y, player.renderer.camera.look.z);
}
