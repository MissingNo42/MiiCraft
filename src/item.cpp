//
// Created by Natha on 19/02/2024.
//

#include "item.h"

bool Item::equals(Item item) const {
    return item.id == id;
}

Item::Item(int id) : id(id) {}
