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
    Slot craftSlots[10];
    Slot inventory[10][9];
    int offsetRow = 0;
    int offsetColumn = 0;
    Craft currentCraft{};
    int currentPage;

    Inventory();
    void pickItem(Slot& s, bool dividedByTwo, bool creativeInventory);
    void resetInventory();
    bool fastDrop(Slot& s, bool putInHotbar, bool putInInv, bool creativeInventory);
    Craft getCurrentCraft();
    void action(int slot, bool craftSlot, int dropMode = 0, bool fastDrop = false, bool dividedByTwo = false, bool creative = true);
    void dropItem(Slot& s, int quantity, bool creativeInventory);

    void getUniqueRecipe();

    void handleCraft(bool fastDrop, bool creative);
};




#endif //MIICRAFTTEST_INVENTORY_H
