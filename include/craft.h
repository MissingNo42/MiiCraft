#pragma once

#include "item.h"

struct Slot {
    explicit Slot(BlockType type = BlockType::Air, int quantity = 0);
    [[nodiscard]] bool enoughToCraft(Slot) const;
    [[nodiscard]] bool equals(Slot) const;
    Item item;
    int quantity;
};


struct Craft {
    Craft();
    Craft(Slot, Slot, Slot, Slot, Slot, Slot, Slot, Slot, Slot, Slot);
    [[nodiscard]] bool equals(Craft) const;
    static Craft craftList[11];
    Slot recipe[9];
    Slot result;
};
