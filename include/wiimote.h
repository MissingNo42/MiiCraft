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
    u32 type = 0;

public:
    Wiimote();
    bool update();

    WPADData * wd;
    u16 directions;
};


#endif //MIICRAFT_WIIMOTE_H
