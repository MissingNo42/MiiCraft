//
// Created by Natha on 19/02/2024.
//

#include "item.h"

bool Item::equals(Item item) const {
    return item.type == type;
}


Item::Item(BlockType type) : type(type){

}

std::vector<Item> Item::itemList = {
        Item(BlockType::Air),
        Item(BlockType::Bedrock),
        Item(BlockType::Andesite),
        Item(BlockType::SandStone),
        Item(BlockType::CoalOre),
        Item(BlockType::CopperOre),
        Item(BlockType::DiamondOre),
        Item(BlockType::EmeraldOre),
        Item(BlockType::GoldOre),
        Item(BlockType::IronOre),
        Item(BlockType::LapisOre),
        Item(BlockType::RedstoneOre),
        Item(BlockType::Ametyst),
        Item(BlockType::Granite),
        Item(BlockType::Diorite),
        Item(BlockType::Clay),
        Item(BlockType::ClayWhite),
        Item(BlockType::ClayBrown),
        Item(BlockType::ClayBlack),
        Item(BlockType::ClayGray),
        Item(BlockType::ClayYellow),
        Item(BlockType::ClayRed),
        Item(BlockType::ClayLightGray),
        Item(BlockType::ClayPurple),
        Item(BlockType::ClayGreen),
        Item(BlockType::Dirt),
        Item(BlockType::Sand),
        Item(BlockType::RedSand),
        Item(BlockType::Gravel),
        Item(BlockType::GrassSavanna),
        Item(BlockType::GrassTemperate),
        Item(BlockType::GrassJungle),
        Item(BlockType::GrassTaiga),
        Item(BlockType::GrassDark),
        Item(BlockType::GrassCold),
        Item(BlockType::GrassSakura),
        Item(BlockType::GrassBadland),
        Item(BlockType::GrassPodzol),
        Item(BlockType::GrassSnow),
        Item(BlockType::LeaveMushroomRed),
        Item(BlockType::LeaveMushroomBrown),
        Item(BlockType::WoodAcacia),
        Item(BlockType::WoodOak),
        Item(BlockType::WoodJungle),
        Item(BlockType::WoodSpruce),
        Item(BlockType::WoodDark),
        Item(BlockType::WoodBirch),
        Item(BlockType::WoodSakura),
        Item(BlockType::WoodMushroom),
        Item(BlockType::PlankAcacia),
        Item(BlockType::PlankOak),
        Item(BlockType::PlankJungle),
        Item(BlockType::PlankSpruce),
        Item(BlockType::PlankDark),
        Item(BlockType::PlankBirch),
        Item(BlockType::PlankSakura),
        Item(BlockType::Furnace),
        Item(BlockType::CraftingTable),
        Item(BlockType::BrickStone),
        Item(BlockType::BrickDark),
        Item(BlockType::BrickRed),
        Item(BlockType::CoalBlock),
        Item(BlockType::CopperBlock),
        Item(BlockType::DiamondBlock),
        Item(BlockType::EmeraldBlock),
        Item(BlockType::GoldBlock),
        Item(BlockType::IronBlock),
        Item(BlockType::LapisBlock),
        Item(BlockType::RedstoneBlock),
        Item(BlockType::ShroomLight),
        Item(BlockType::RedstoneLamp),
        Item(BlockType::BlueIce),
        Item(BlockType::StdIce),
        Item(BlockType::Lava),
        Item(BlockType::Netherrack),
        Item(BlockType::QuartzOre),
        Item(BlockType::MagmaBlock1),
        Item(BlockType::MagmaBlock2),
        Item(BlockType::MagmaBlock3),
        Item(BlockType::Trinitrotoluene),
        Item(BlockType::QuartzBlock),
        Item(BlockType::Glowstone),
        Item(BlockType::Impostor),
        Item(BlockType::Cactus),
        Item(BlockType::LeaveAcacia),
        Item(BlockType::LeaveOak),
        Item(BlockType::LeaveJungle),
        Item(BlockType::LeaveSpruce),
        Item(BlockType::LeaveDark),
        Item(BlockType::LeaveBirch),
        Item(BlockType::LeaveSakura),
        Item(BlockType::LeaveSnow),
        Item(BlockType::Glass),
        Item(BlockType::ClearIce),
        Item(BlockType::Water)
};

char* Item::ToString() const{
    switch (type){
        case BlockType::Air :
            return "air";
            break;
        case BlockType::WoodOak :
            return "OakWood";
            break;

        case Air0:
            break;
        case Air1:
            break;
        case Air2:
            break;
        case Air3:
            break;
        case Air4:
            break;
        case Air5:
            break;
        case Air6:
            break;
        case Air7:
            break;
        case Air8:
            break;
        case Air9:
            break;
        case Air10:
            break;
        case Air11:
            break;
        case Air12:
            break;
        case Air13:
            break;
        case Air14:
            break;
        case Bedrock:
            break;
        case Stone:
            break;
        case Andesite:
            break;
        case SandStone:
            break;
        case CoalOre:
            break;
        case CopperOre:
            break;
        case DiamondOre:
            break;
        case EmeraldOre:
            break;
        case GoldOre:
            break;
        case IronOre:
            break;
        case LapisOre:
            break;
        case RedstoneOre:
            break;
        case Ametyst:
            break;
        case Granite:
            break;
        case Diorite:
            break;
        case Clay:
            break;
        case ClayWhite:
            break;
        case ClayBrown:
            break;
        case ClayBlack:
            break;
        case ClayGray:
            break;
        case ClayYellow:
            break;
        case ClayRed:
            break;
        case ClayLightGray:
            break;
        case ClayPurple:
            break;
        case ClayGreen:
            break;
        case Dirt:
            break;
        case Sand:
            break;
        case RedSand:
            break;
        case Gravel:
            break;
        case GrassSavanna:
            break;
        case GrassTemperate:
            break;
        case GrassJungle:
            break;
        case GrassTaiga:
            break;
        case GrassDark:
            break;
        case GrassCold:
            break;
        case GrassSakura:
            break;
        case GrassBadland:
            break;
        case GrassPodzol:
            break;
        case GrassSnow:
            break;
        case LeaveMushroomRed:
            break;
        case LeaveMushroomBrown:
            break;
        case WoodAcacia:
            break;
        case WoodJungle:
            break;
        case WoodSpruce:
            break;
        case WoodDark:
            break;
        case WoodBirch:
            break;
        case WoodSakura:
            break;
        case WoodMushroom:
            break;
        case PlankAcacia:
            break;
        case PlankOak:
            break;
        case PlankJungle:
            break;
        case PlankSpruce:
            break;
        case PlankDark:
            break;
        case PlankBirch:
            break;
        case PlankSakura:
            break;
        case Furnace:
            break;
        case CraftingTable:
            break;
        case BrickStone:
            break;
        case BrickDark:
            break;
        case BrickRed:
            break;
        case CoalBlock:
            break;
        case CopperBlock:
            break;
        case DiamondBlock:
            break;
        case EmeraldBlock:
            break;
        case GoldBlock:
            break;
        case IronBlock:
            break;
        case LapisBlock:
            break;
        case RedstoneBlock:
            break;
        case ShroomLight:
            break;
        case RedstoneLamp:
            break;
        case BlueIce:
            break;
        case StdIce:
            break;
        case Lava:
            break;
        case Netherrack:
            break;
        case QuartzOre:
            break;
        case MagmaBlock1:
            break;
        case MagmaBlock2:
            break;
        case MagmaBlock3:
            break;
        case Trinitrotoluene:
            break;
        case QuartzBlock:
            break;
        case Glowstone:
            break;
        case Impostor:
            break;
        case Cactus:
            break;
        case LeaveAcacia:
            break;
        case LeaveOak:
            break;
        case LeaveJungle:
            break;
        case LeaveSpruce:
            break;
        case LeaveDark:
            break;
        case LeaveBirch:
            break;
        case LeaveSakura:
            break;
        case LeaveSnow:
            break;
        case Glass:
            break;
        case ClearIce:
            break;
        case Water:
            break;
        case BlockBreaking0:
            break;
        case BlockBreaking1:
            break;
        case BlockBreaking2:
            break;
        case BlockBreaking3:
            break;
        case BlockBreaking4:
            break;
        case BlockBreaking5:
            break;
        case BlockBreaking6:
            break;
        case BlockBreaking7:
            break;
        case BlockBreaking8:
            break;
        case BlockBreaking9:
            break;
    }
}



