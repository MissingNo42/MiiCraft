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

    void handleMovement(Camera& camera);
    void handleRotation(Camera& camera);

public:
	u16 hold = 0, pressed = 0, released = 0;
	WPADData * wd;
	
    explicit Wiimote(int chan = WPAD_CHAN_0);
    bool update(Camera& camera);
};


#endif //MIICRAFT_WIIMOTE_H
