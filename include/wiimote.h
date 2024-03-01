//
// Created by Romain on 04/02/2024.
//

#ifndef MIICRAFT_WIIMOTE_H
#define MIICRAFT_WIIMOTE_H

#include <wiiuse/wpad.h>

class Wiimote {
    struct expansion_t data;
    int chan;
    u32 type = 0;
    f32 last_accel = 0;
	
public:
	static bool quit;
    WPADData * wd = nullptr;
	f32 x = 0, y = 0, accel = 0;
	bool connected = false;

	explicit Wiimote(int chan = WPAD_CHAN_0);
	
    void update();
	
	static void setup();
	static void sync();
};


#endif //MIICRAFT_WIIMOTE_H
