//
// Created by Natha on 20/02/2024.
//

#include "craft.h"


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

Craft Craft::craftList[10] = {
        // Craft Vide
        Craft(),

        // Buches => planches
        Craft(Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::PlankOak, 4}
        ),

        Craft(Slot{BlockType::Air, 1},
              Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::PlankOak, 4}
        ),

        Craft(Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::PlankOak, 4}
        ),

        Craft(Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::PlankOak, 4}
        ),

        Craft(Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::PlankOak, 4}
        ),

        Craft(Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::PlankOak, 4}
        ),

        Craft(Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::PlankOak, 4}
        ),

        Craft(Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::WoodOak, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::PlankOak, 4}
        ),

        Craft(Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::Air, 1},
              Slot{BlockType::WoodOak, 1},
              Slot{BlockType::PlankOak, 4}
        ),
};
bool Slot::equals(Slot slot) const {
    return slot.item.equals(item) && slot.quantity == quantity;
}
