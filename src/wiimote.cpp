//
// Created by Romain on 04/02/2024.
//
#define deadzone 20
#define deadzoneTop 80


#include <cstdio>
#include "wiimote.h"


void Wiimote::update(Player& player, World& w) {

    WPAD_ScanPads();
    int wiimote_connection_status = WPAD_Probe(chan, &type);

    if (wiimote_connection_status == WPAD_ERR_NONE) {
        WPADData * wd = WPAD_Data(chan);
        player.handleRotation(wd);
    }
    if(WPAD_ButtonsDown(chan) & WPAD_BUTTON_HOME)
        exit(1);

    WPAD_Expansion(chan, &data);

    u16 directions = WPAD_ButtonsHeld(chan);
    player.handleMovement(w, directions);
    player.getFocusedBlock(w);
    player.handleAction(w, directions);
}

Wiimote::Wiimote() {
    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
    WPAD_SetVRes(WPAD_CHAN_ALL, Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight);
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
