//
// Created by Natha on 20/02/2024.
//

#ifndef MIICRAFTTEST_CRAFT_H
#define MIICRAFTTEST_CRAFT_H

#include "item.h"

struct Slot {
    explicit Slot(int index = 0, int quantity = 0);
    bool equals(Slot) const;
    Item item;
    int quantity;
};


struct Craft {
    Craft();
    Craft(Slot, Slot, Slot, Slot, Slot, Slot, Slot, Slot, Slot, Slot);
    static Craft craftList[10];
    Slot recipe[9];
    Slot result;
};


#endif //MIICRAFTTEST_CRAFT_H
