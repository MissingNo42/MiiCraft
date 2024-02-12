//
// Created by Romain on 04/02/2024.
//

#ifndef MIICRAFT_WIIMOTE_H
#define MIICRAFT_WIIMOTE_H

#include <cstdlib>
#include <wiiuse/wpad.h>
#include "engine/render/renderer.h"
#include "world/world.h"


class Wiimote {
private:
    struct expansion_t data;
    int chan = WPAD_CHAN_0;
    int speed = 1;
    u32 type = 0;
    bool collision = true;

    void handleMovement(Renderer& renderer, u16 directions, World& w);
    static void handleRotation(Renderer& renderer, WPADData * wd);

public:
    Wiimote() {
        WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);
        WPAD_SetVRes(WPAD_CHAN_ALL, Renderer::rmode->fbWidth, Renderer::rmode->xfbHeight);
    }
    void update(Renderer& renderer, World& w);
};


#endif //MIICRAFT_WIIMOTE_H
