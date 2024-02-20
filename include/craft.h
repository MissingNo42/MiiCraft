//
// Created by Natha on 20/02/2024.
//

#ifndef MIICRAFTTEST_CRAFT_H
#define MIICRAFTTEST_CRAFT_H


#include <unordered_set>
#include "inventory.h"

struct Craft {
    Craft();
    Craft(Slot*, Slot);
    static std::vector<Craft> craftList;
    static int numCrafts;
    Slot recipe[9];
    Slot result;
};


#endif //MIICRAFTTEST_CRAFT_H
