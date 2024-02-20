//
// Created by Natha on 20/02/2024.
//

#include "craft.h"
std::vector<Craft> Craft::craftList;
int Craft::numCrafts = 0;

Craft::Craft(Slot* rec, Slot res) {
    for (int i = 0; i < 9; i++)
        recipe[i] = rec[i];
    result = res;
}

Craft::Craft() {
    for (auto & i : recipe)
        i = Slot();
    result = Slot();
}

Craft::craftList =
