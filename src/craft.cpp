//
// Created by Natha on 20/02/2024.
//

#include "craft.h"

bool Slot::enoughToCraft(Slot s) const {
    return s.item.equals(item) && s.quantity <= quantity;
}


Slot::Slot(BlockType type, int quantity) : item(type), quantity(quantity){

}

Craft::Craft() {
    for (auto & i : recipe)
        i = Slot();
    result = Slot();
}

Craft::Craft(Slot s1, Slot s2, Slot s3, Slot s4, Slot s5, Slot s6, Slot s7, Slot s8, Slot s9, Slot r) {
    recipe[0] = s1;
    recipe[1] = s2;
    recipe[2] = s3;
    recipe[3] = s4;
    recipe[4] = s5;
    recipe[5] = s6;
    recipe[6] = s7;
    recipe[7] = s8;
    recipe[8] = s9;
    result = r;
}

Craft Craft::craftList[11] = {
        // Craft Vide
        Craft(),

        // Buches => planches
        Craft(Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 0},
              Slot{BlockType::Air, 0},
              Slot{BlockType::Air, 0},
              Slot{BlockType::Air, 0},
              Slot{BlockType::Air, 0},
              Slot{BlockType::Air, 0},
              Slot{BlockType::Air, 0},
              Slot{BlockType::Air, 0},
              Slot{BlockType::PlankOak, 4}
        ),

        // Crafting Table
        Craft(Slot{BlockType::PlankOak, 1},
        Slot{BlockType::PlankOak, 1},
        Slot{BlockType::Air, 0},
        Slot{BlockType::PlankOak, 1},
        Slot{BlockType::PlankOak, 1},
        Slot{BlockType::Air, 0},
        Slot{BlockType::Air, 0},
        Slot{BlockType::Air, 0},
        Slot{BlockType::Air, 0},
        Slot{BlockType::CraftingTable, 1}
        ),

        Craft(Slot{BlockType::Stone,1},
              Slot{BlockType::Stone,1},
              Slot{BlockType::Stone,1},
              Slot{BlockType::Stone,1},
              Slot{BlockType::Air,1},
              Slot{BlockType::Stone,1},
              Slot{BlockType::Stone,1},
              Slot{BlockType::Stone,1},
              Slot{BlockType::Stone,1},
              Slot{BlockType::Furnace,1}
        ),
};
