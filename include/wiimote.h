//
// Created by Romain on 04/02/2024.
//

#ifndef MIICRAFT_WIIMOTE_H
#define MIICRAFT_WIIMOTE_H

#include <cstdlib>
#include <wiiuse/wpad.h>
#include "engine/render/renderer.h"
#include "world/world.h"
#include "player.h"


class Wiimote {
private:
    struct expansion_t data;
    int chan = WPAD_CHAN_0;
    u32 type = 0;

public:
    Wiimote();
    WPADData * wd;

    void update(Player& player);
};


#endif //MIICRAFT_WIIMOTE_H
