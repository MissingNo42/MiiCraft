//
// Created by paulo on 21/02/2024.
//

#ifndef MIICRAFTTEST_INTERFACE_H
#define MIICRAFTTEST_INTERFACE_H

#include "player.h"
#include "wiimote.h"

class GUI {
    f32 x= 0.05,y= 0.05, a, b;
public :
    const u32 white = 0xFFFFFFFF;

    void renderCursor(Player &player, Wiimote &wiimote);
    void renderInventory(Player &player) const;
};
#endif //MIICRAFTTEST_INTERFACE_H
