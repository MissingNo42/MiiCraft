//
// Created by Natha on 19/02/2024.
//

#include "inventory.h"

Inventory::Inventory() : open(false), selectedSlot(0), pickedItem(BlockType::Air, 1), craftSlots(), inventory(),
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
            for(int i = 0; i < 9; i++){
                craftSlots[i].quantity -= currentCraft.recipe[i].quantity;
                if(craftSlots[i].quantity == 0)
                    craftSlots[i].item = Item::itemList[0];
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
        if(slot <= 4 && slot >= 0) {
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
    for(auto& it : Craft::craftList){
        bool isMatch = true;
        for(int i = 0; i < 9; i++)
            if(!it.recipe[i].equals(craftSlots[i]))
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
