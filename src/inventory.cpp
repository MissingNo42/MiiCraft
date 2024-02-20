//
// Created by Natha on 19/02/2024.
//

#include "inventory.h"

Inventory::Inventory() : open(false), selectedSlot(0), craftSlots(), inventory(){
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 9; j++)
            inventory[i][j] = Slot( 17+j+i*9,1);;
}

Slot::Slot(int id, int quantity) : item(id), quantity(quantity){}

void Inventory::pickItem(int slot, bool craftSlot) {
    Slot temp = pickedItem;
    if(!craftSlot){
        pickedItem = inventory[slot/9][slot%9];
        inventory[slot/9][slot%9] = temp;
    }
    else {
        if(slot < 4 && slot > 0) {
            pickedItem = craftSlots[slot];
            craftSlots[slot] = temp;
            currentCraft = getCurrentCraft();
        }
        else if(slot == 4 && pickedItem.item.equals(Item::itemList[0])){
            pickedItem = craftSlots[slot];
            for(int i = 0; i < 4; i++){
                craftSlots[slot].quantity -= currentCraft.recipe[i].quantity;
                if(craftSlots[i].quantity == 0)
                    craftSlots[i].item = Item::itemList[0];
            }
        }
    }
}

void Inventory::dropItem(int slot, bool unique, bool craftSlot){
    Slot temp = inventory[slot / 9][slot % 9];
    if(!craftSlot) {
        if(unique && inventory[slot / 9][slot % 9].item.equals(Item::itemList[0])) {
            inventory[slot / 9][slot % 9].item = pickedItem.item;
            inventory[slot / 9][slot % 9].quantity++;
            pickedItem.quantity--;
            if(pickedItem.quantity == 0)
                pickedItem.item = Item::itemList[0];
        }
        else{
            inventory[slot / 9][slot % 9] = pickedItem;
            pickedItem = temp;
        }
    }
    else{
        if(slot < 4 && slot > 0) {
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
        }
    }
}

Craft Inventory::getCurrentCraft() {
    for(auto& it : Craft::craftList){
        bool isMatch = true;
        for(int i = 0; i < 9; i++)
            if(!it.recipe[i].equals(currentCraft.recipe[i]))
                isMatch = false;
        if(isMatch)
            return it;
    }
    return Craft::craftList[0];

}

bool Inventory::isOpen() const{
    return open;
}
