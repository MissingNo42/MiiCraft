//
// Created by Natha on 19/02/2024.
//

#include "inventory.h"

Inventory::Inventory() : open(false), craftOpen(true), selectedSlot(0), pickedItem(BlockType::Air, 1), craftSlots(), inventory(),
                         currentCraft(Craft::craftList[0]), currentPage(0) {

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            inventory[i+1][j] = Slot(static_cast<BlockType>(BlockType::Air), 0);
        }
    }
    for (int i = 0; i < 9; i++) {
        inventory[0][i] = Slot(static_cast<BlockType>(BlockType::Air), 0);
    }
    for (auto & craftSlot : craftSlots) {
        craftSlot = Slot(static_cast<BlockType>(BlockType::Air), 0);
    }
}

void Inventory::pickItem(int slot, bool craftSlot) {
    Slot temp = pickedItem;
    if(!craftSlot){
        if(slot < 27) {
            pickedItem = inventory[1 + currentPage * 3 + slot / 9][slot % 9];
            inventory[1 + currentPage * 3 + slot / 9][slot % 9] = temp;
        }
        else if(slot < 36){
            pickedItem = inventory[0][slot % 9];
            inventory[0][slot % 9] = temp;
        }
    }
    else {
        if(slot < 9 && slot >= 0) {
            pickedItem = craftSlots[slot];
            craftSlots[slot] = temp;
            currentCraft = getCurrentCraft();
        }
        else if(slot == 9 && pickedItem.item.equals(Item::itemList[0])){
            pickedItem = craftSlots[slot];
            for(int i = 0; i < 3 - offsetColumn; i++){
                for(int j = 0; j < 3 - offsetRow; j++) {
                    craftSlots[(i + offsetRow)*3 + j + offsetColumn].quantity -= currentCraft.recipe[i*3+j].quantity;
                    if (craftSlots[(i + offsetRow)*3 + j + offsetColumn].quantity == 0)
                        craftSlots[(i + offsetRow)*3 + j + offsetColumn].item = Item::itemList[0];
                }
            }
        }
        currentCraft = getCurrentCraft();
        craftSlots[9] = currentCraft.result;
    }
}

void Inventory::dropItem(int slot, bool unique, bool craftSlot){
    Slot temp;
    if(!craftSlot) {
        if (slot < 27) {
            temp = inventory[1 + currentPage * 3 + slot / 9][slot % 9];
            if (unique && inventory[1 + currentPage * 3 + slot / 9][slot % 9].item.equals(Item::itemList[0])) {
                inventory[1 + currentPage * 3 + slot / 9][slot % 9].item = pickedItem.item;
                inventory[1 + currentPage * 3 + slot / 9][slot % 9].quantity++;
                pickedItem.quantity--;
                if (pickedItem.quantity == 0)
                    pickedItem.item = Item::itemList[0];
            } else {
                if (pickedItem.item.equals(inventory[1 + currentPage * 3 + slot / 9][slot % 9].item)) {
                    if (unique) {
                        inventory[1 + currentPage * 3 + slot / 9][slot % 9].quantity++;
                        pickedItem.quantity--;
                        if (pickedItem.quantity == 0)
                            pickedItem.item = Item::itemList[0];
                    }
                    else {
                        int stack = inventory[1 + currentPage * 3 + slot / 9][slot % 9].quantity + pickedItem.quantity;
                        if(stack > 64){
                            pickedItem.quantity = stack - 64;
                            inventory[1 + currentPage * 3 + slot / 9][slot % 9].quantity = 64;
                        }
                        else{
                            inventory[1 + currentPage * 3 + slot / 9][slot % 9].quantity += pickedItem.quantity;
                            pickedItem.quantity = 0;
                            }
                        if (pickedItem.quantity == 0)
                            pickedItem.item = Item::itemList[0];
                    }
                }
                else{
                inventory[1 + currentPage * 3 + slot / 9][slot % 9] = pickedItem;
                pickedItem = temp;
                }
            }
        }
        else if(slot < 36){
            temp = inventory[0][slot % 9];
            if(unique && inventory[0][slot % 9].item.equals(Item::itemList[0])){
                inventory[0][slot % 9].item = pickedItem.item;
                inventory[0][slot % 9].quantity++;
                pickedItem.quantity--;
                if(pickedItem.quantity == 0)
                    pickedItem.item = Item::itemList[0];
            }
            else {
                if (pickedItem.item.equals(inventory[0][slot % 9].item)) {
                    int stack = inventory[0][slot % 9].quantity + pickedItem.quantity;

                    if (stack > 64) {
                        pickedItem.quantity = stack - 64;
                        inventory[0][slot % 9].quantity = 64;
                    } else {
                        inventory[0][slot % 9].quantity += pickedItem.quantity;
                        pickedItem.quantity = 0;
                    }
                    if (pickedItem.quantity == 0)
                        pickedItem.item = Item::itemList[0];
                }
                else {
                    inventory[0][slot % 9] = pickedItem;
                    pickedItem = temp;
                }
            }
        }
    }
    else{
        temp = craftSlots[slot];
        if(slot <= 8 && slot >= 0) {
            if(unique && craftSlots[slot].item.equals(Item::itemList[0])){
                craftSlots[slot].item = pickedItem.item;
                craftSlots[slot].quantity++;
                pickedItem.quantity--;
                if(pickedItem.quantity == 0)
                    pickedItem.item = Item::itemList[0];
            }
            else {
                craftSlots[slot] = pickedItem;
                pickedItem = temp;
            }
            currentCraft = getCurrentCraft();
            craftSlots[9] = currentCraft.result;
        }
    }
}

Craft Inventory::getCurrentCraft() {
    getUniqueRecipe();
    for(auto& it : Craft::craftList){
        bool isMatch = true;
        for(int i = 0; i < 9; i++)
            if(!currentCraft.recipe[i].enoughToCraft(it.recipe[i]))
                isMatch = false;
        if(isMatch) {
            return it;
        }
    }
    return Craft::craftList[0];
}

void Inventory::action(int slot, bool craftSlot) {
    if(pickedItem.item.equals(Item::itemList[0])){
        pickItem(slot, craftSlot);
    }
    else{
        dropItem(slot, false, craftSlot);
    }
}

void Inventory::resetInventory() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            inventory[i+1][j] = Slot(static_cast<BlockType>(BlockType::Air +i * 9 + j), 1);
        }
    }
    inventory[1][0] = Slot(static_cast<BlockType>(BlockType::Air), 0);
}

void Inventory::ClearCraft() {
    for (auto & craftSlot : craftSlots) {
        for (auto & c : inventory){
            for (auto & p : c) {
                if (craftSlot.item.equals(p.item) && !craftSlot.item.equals(Item::itemList[0])) {
                    int stack = p.quantity + craftSlot.quantity;
                    if (stack > 64) {
                        craftSlot.quantity = stack - 64;
                        p.quantity = 64;
                    } else {
                        p.quantity += craftSlot.quantity;
                        craftSlot.quantity = 0;
                        craftSlot.item = Item::itemList[0];
                    }
                }
            }
        }
        if (!craftSlot.item.equals(Item::itemList[0])){
            for (auto & c : inventory) {
                for (auto &p: c) {
                    if (craftSlot.item.equals(Item::itemList[0])) {
                        p = craftSlot;
                        craftSlot.item = Item::itemList[0];
                        craftSlot.quantity = 0;

                    }
                }
            }
        }
        //printf("%d %d\r", craftSlot.item ,craftSlot.quantity);
    }
}

void Inventory::addItem(BlockType type, int quantity) {
    printf("Added\r");
    for (auto & c : inventory) {
        for (auto & p : c) {
            if (quantity == 0)
                break;
            if (type==p.item.type && type!=Item::itemList[0].type) {
                int stack = p.quantity + quantity;
                if (stack > 64) {
                    quantity = stack - 64;
                    p.quantity = 64;
                } else {
                    p.quantity += quantity;
                    quantity = 0;
                }
            }
        }
        if (quantity != 0) {
            for (auto & c : inventory) {
                for (auto & p : c) {
                    if (quantity == 0)
                        break;
                    if (type!=Item::itemList[0].type && p.item.equals(Item::itemList[0])) {
                        p = Slot(type, quantity);
                        printf("%d %d\r", p.item.type, p.quantity);
                        quantity = 0;
                    }
                }
            }
        }
    }
}

void Inventory::getUniqueRecipe() {
    bool isValidRow = false;
    bool isValidColumn = false;
    int count = 0;
    offsetRow = 0;
    offsetColumn = 0;
    for(int i = 0; i < 9; i++)
        currentCraft.recipe[i] = craftSlots[i];
    while((!isValidRow || !isValidColumn) && count < 3 ){
        for(int i = 0; i < 3; i++)
            if(!currentCraft.recipe[i].item.equals(Item::itemList[0]))
                isValidRow = true;

        for(int i = 0; i < 3; i++)
            if(!currentCraft.recipe[i * 3].item.equals(Item::itemList[0]))
                isValidColumn = true;

        if(!isValidRow) {
            offsetRow++;
            for(int i = 1; i < 3; i++){
                for(int j = 0; j < 3; j++){
                    currentCraft.recipe[(i-1) * 3 + j] = currentCraft.recipe[i * 3 + j];
                }
            }
            for(int i = 0; i < 3; i++){
                currentCraft.recipe[6 + i].item = Item::itemList[0];
                currentCraft.recipe[6 + i].quantity = 0;
            }
        }
        if(!isValidColumn) {
            offsetColumn++;
            for(int i = 1; i < 3; i++)
                for(int j = 0; j < 3; j++)
                    currentCraft.recipe[j * 3 + (i-1)] = currentCraft.recipe[j * 3 + i];
            for(int i = 0; i < 3; i++){
                currentCraft.recipe[i * 3 + 2].item = Item::itemList[0];
                currentCraft.recipe[i * 3 + 2].quantity = 0;
            }
        }
        if(++count == 3){
            offsetRow = 0;
            offsetColumn = 0;
        }
    }
}
