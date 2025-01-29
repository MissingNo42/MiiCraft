#pragma once

#include <vector>
#include "world/block.h"

class Item {

public:
    BlockType type;
    explicit Item(BlockType type = BlockType::Air);
    static std::vector<Item> itemList;
    [[nodiscard]] bool equals(Item) const;
};
