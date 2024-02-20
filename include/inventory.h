//
// Created by Natha on 19/02/2024.
//

#ifndef MIICRAFTTEST_INVENTORY_H
#define MIICRAFTTEST_INVENTORY_H


#include <vector>
#include "item.h"
#include "craft.h"


class Inventory {
private:
public:
    bool open;
    int selectedSlot;
    Slot pickedItem{};
    Slot craftSlots[9];
    Slot inventory[4][9];
    Craft currentCraft{};

    Inventory();
    bool isOpen() const;
    void pickItem(int slot, bool craftSlot);
    void dropItem(int slot, bool unique, bool craftSlot);
    Craft getCurrentCraft();
};




#endif //MIICRAFTTEST_INVENTORY_H
