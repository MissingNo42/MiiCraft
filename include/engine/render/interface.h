//
// Created by paulo on 21/02/2024.
//

#ifndef MIICRAFTTEST_INTERFACE_H
#define MIICRAFTTEST_INTERFACE_H

#include "player.h"
#include "wiimote.h"

class HUD {
    f32 x = 0.05f, y = 0.05f, a = 0, b = 0;
public:

    void renderCursor(Player &player);
    void renderInventory(Player &player) const;
    void Underwater() const;
};
#endif //MIICRAFTTEST_INTERFACE_H
