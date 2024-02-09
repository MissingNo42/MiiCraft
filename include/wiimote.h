//
// Created by Romain on 04/02/2024.
//

#ifndef MIICRAFT_WIIMOTE_H
#define MIICRAFT_WIIMOTE_H

#include <cstdlib>
#include <wiiuse/wpad.h>
#include "engine/render/renderer.h"

class Wiimote {
private:
    struct expansion_t data;
    int chan = WPAD_CHAN_0;
    int speed = 1;
    u32 type = 0;

    void handleMovement(Renderer& renderer, u16 directions);
    static void handleRotation(Renderer& renderer, WPADData * wd);

public:
    Wiimote();
    void update(Renderer& renderer);
};


#endif //MIICRAFT_WIIMOTE_H
