//
// Created by Natha on 19/02/2024.
//

#ifndef MIICRAFTTEST_ITEM_H
#define MIICRAFTTEST_ITEM_H


#include <vector>
#include "world/block.h"

class Item {
    int id;
public:
    explicit Item(int id = 0);
    static std::vector<Item> itemList;
    [[nodiscard]] bool equals(Item) const;
};


#endif //MIICRAFTTEST_ITEM_H
