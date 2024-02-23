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

void Inventory::pickItem(Slot& s, bool dividedByTwo, bool creativeInventory){
    pickedItem.item = s.item;
    if(creativeInventory){
        pickedItem.quantity = 64;
    }
    else if(!s.item.equals(Item::itemList[0])){
        if(dividedByTwo && s.quantity > 1 ) {
            pickedItem.quantity = s.quantity / 2;
            s.quantity -=  pickedItem.quantity;
        }
        else
        {
            pickedItem.quantity = s.quantity;
            s.quantity = 0;
            s.item = Item::itemList[0];
        }
    }
}

void Inventory::dropItem(Slot& s, int mode, bool creativeInventory){
    int quantity = 0;
    switch (mode){
        case 0:
            quantity = pickedItem.quantity;
            break;
        case 1:
            quantity = 1;
            break;
    }

    if(s.item.equals(pickedItem.item)) {
        if(creativeInventory) {
            pickedItem.item = Item::itemList[0];
            pickedItem.quantity = 0;
        }
        else if (quantity + s.quantity <= 64) {
            s.quantity += quantity;
            pickedItem.quantity -= quantity;
            if (pickedItem.quantity == 0)
                pickedItem.item = Item::itemList[0];
        } else {
            int resultQuantity = 64 - s.quantity;
            s.quantity += resultQuantity;
            pickedItem.quantity -= resultQuantity;
        }
    }
    else if (s.item.equals(Item::itemList[0])){
        s.item = pickedItem.item;
        s.quantity = quantity;
        pickedItem.quantity -= quantity;

        if (pickedItem.quantity == 0)
            pickedItem.item = Item::itemList[0];
    }
    else {
        if(creativeInventory) {
            pickedItem.item = Item::itemList[0];
            pickedItem.quantity = 0;
        }
        else if(pickedItem.quantity == quantity){
            Slot temp = pickedItem;
            pickedItem = s;
            s = temp;
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

bool Inventory::fastDrop(Slot& s, bool putInHotbar, bool putInInv, bool creativeInventory){
    if(putInInv && !creativeInventory){
        for(int i = 1; i < 10; i++) {
            for (int j = 0; j < 9; j++) {
                if (inventory[i][j].item.equals(Item::itemList[0])){
                    inventory[i][j] = s;
                    s.quantity = 0;
                    s.item = Item::itemList[0];
                    return true;
                }
                else if (inventory[i][j].item.equals(s.item)) {
                    if (inventory[i][j].quantity + s.quantity <= 64) {
                        inventory[i][j].quantity += s.quantity;
                        s.quantity = 0;
                        s.item = Item::itemList[0];
                        return true;
                    }
                    else {
                        int quantity = 64 - inventory[i][j].quantity;
                        inventory[i][j].quantity = 64;
                        s.quantity -= quantity;
                    }
                }
            }
        }
    }
    if(putInHotbar) {
        for (int i = 0; i < 9; i++) {
            if (inventory[0][i].item.equals(Item::itemList[0])){
                inventory[0][i] = s;
                s.quantity = 0;
                s.item = Item::itemList[0];
                return true;
            }
            else if (inventory[0][i].item.equals(s.item)) {
                if (inventory[0][i].quantity + s.quantity <= 64) {
                    inventory[0][i].quantity += s.quantity;
                    s.quantity = 0;
                    s.item = Item::itemList[0];
                    return true;
                }
                else {
                    int quantity = 64 - inventory[0][i].quantity;
                    inventory[0][i].quantity = 64;
                    s.quantity -= quantity;
                }
            }
        }
    }
    return false;
}

void Inventory::action(int slot, bool craftSlot, int dropMode, bool fast, bool dividedByTwo, bool creative) {
    if(!craftSlot) {
        if (slot < 27) {
            if(fast)
                fastDrop(inventory[1 + currentPage * 3 + slot / 9][slot % 9], true, false, creative);
            else if(pickedItem.item.equals(Item::itemList[0])){
                pickItem(inventory[1 + currentPage * 3 + slot / 9][slot % 9], dividedByTwo, creative);
            }else
                dropItem(inventory[1 + currentPage * 3 + slot / 9][slot % 9], dropMode, creative);
        }
        else if(slot < 36){
            if(fast)
                fastDrop(inventory[0][slot % 9], false, true, creative);
            else if(pickedItem.item.equals(Item::itemList[0])) {
                pickItem(inventory[0][slot % 9], dividedByTwo, false);
            }else
                dropItem(inventory[0][slot % 9], dropMode, false);
        }
    }
    else {
        if (slot < 9 && slot >= 0) {
            if(fast)
                fastDrop(craftSlots[slot], true, true, creative);
            else if (pickedItem.item.equals(Item::itemList[0]))
                pickItem(craftSlots[slot], dividedByTwo, false);
            else
                dropItem(craftSlots[slot], dropMode, false);
            currentCraft = getCurrentCraft();
            craftSlots[9] = currentCraft.result;
        } else if (slot == 9)  {
            handleCraft(fast, creative);
        }
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
        addItem(craftSlot.item.type, craftSlot.quantity);
        craftSlot = Slot(static_cast<BlockType>(BlockType::Air), 0);
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

void Inventory::handleCraft(bool fastMode, bool creative) {
    if(fastMode){
        bool full = false;
        while(!currentCraft.equals(Craft::craftList[0]) && !full){
            full = !fastDrop(craftSlots[9], true, true, creative);
            if(full)
                return;
            for (int i = 0; i < 3 - offsetColumn; i++) {
                for (int j = 0; j < 3 - offsetRow; j++) {
                    craftSlots[(i + offsetRow) * 3 + j + offsetColumn].quantity -= currentCraft.recipe[i * 3 +
                                                                                                       j].quantity;
                    if (craftSlots[(i + offsetRow) * 3 + j + offsetColumn].quantity == 0)
                        craftSlots[(i + offsetRow) * 3 + j + offsetColumn].item = Item::itemList[0];
                }
            }
            currentCraft = getCurrentCraft();
            craftSlots[9] = currentCraft.result;
        }
    }
    else if(pickedItem.item.equals(Item::itemList[0]) || pickedItem.item.equals(craftSlots[9].item)){
        if(pickedItem.item.equals(Item::itemList[0])) {
            pickedItem = craftSlots[9];
            for (int i = 0; i < 3 - offsetColumn; i++) {
                for (int j = 0; j < 3 - offsetRow; j++) {
                    craftSlots[(i + offsetRow) * 3 + j + offsetColumn].quantity -= currentCraft.recipe[i * 3 +
                                                                                                       j].quantity;
                    if (craftSlots[(i + offsetRow) * 3 + j + offsetColumn].quantity == 0)
                        craftSlots[(i + offsetRow) * 3 + j + offsetColumn].item = Item::itemList[0];
                }
            }
        }
        else if(pickedItem.item.equals(craftSlots[9].item)) {
            if(pickedItem.quantity + craftSlots[9].quantity <= 64) {
                pickedItem.quantity += craftSlots[9].quantity;
                for (int i = 0; i < 3 - offsetColumn; i++) {
                    for (int j = 0; j < 3 - offsetRow; j++) {
                        craftSlots[(i + offsetRow) * 3 + j + offsetColumn].quantity -= currentCraft.recipe[i * 3 +j].quantity;
                        if (craftSlots[(i + offsetRow) * 3 + j + offsetColumn].quantity == 0)
                            craftSlots[(i + offsetRow) * 3 + j + offsetColumn].item = Item::itemList[0];
                    }
                }
            }
        }
        currentCraft = getCurrentCraft();
        craftSlots[9] = currentCraft.result;
    }
}
