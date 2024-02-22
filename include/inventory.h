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
    bool craftOpen;
    int selectedSlot;
    Slot pickedItem{};
    Slot craftSlots[10];
    Slot inventory[10][9];
    Craft currentCraft{};
    int currentPage;

    Inventory();
    void pickItem(int slot, bool craftSlot);
    void resetInventory();
    void dropItem(int slot, bool unique, bool craftSlot);
    Craft getCurrentCraft();
    void action(int slot, bool craftSlot);
};




#endif //MIICRAFTTEST_INVENTORY_H
