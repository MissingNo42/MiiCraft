//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_BLOCK_H
#define MIICRAFT_BLOCK_H
#include <iostream>
#include <gctypes.h>
#define SOLIDBLOCK 2

enum BlockType : u8 {
////     Air =0,
////     Bedrock,
////     Dirt,
//     Grass,
////     Stone,
////     Sand,
////     Snow,
////     Ice,
////     RedClay,
////     WhiteClay,
////
////     //Wood
////
//     Log, //Oak
//     Leaves, //Oak
////     AcaciaWood,
////     AcaciaLeave,
////     SpruceWood,
////     SpruceLeave,
////     JungleWood,
////     JungleLeave,
////
////     Cactus,

    Air = 0,

    //Mineral
    Bedrock,
    Stone,
    Andesite,
    SandStone,
    CoalOre,
    CopperOre,
    DiamondOre,
    EmeraldOre,
    GoldOre,
    IronOre,
    LapisOre,
    RedstoneOre,
    Ametyst,
    Granite,
    Diorite,

    //Clay
    Clay,
    ClayWhite,
    ClayBrown,
    ClayBlack,
    ClayGray,
    ClayYellow,
    ClayRed,
    ClayLightGray,
    ClayPurple,
    ClayGreen,


    //Powder
    Dirt,
    Sand,
    RedSand,
    Gravel,

    //Soil
    GrassSavanna,
    GrassTemperate,
    GrassJungle,
    GrassTaiga,
    GrassDark,
    GrassCold,
    GrassSakura,
    GrassBadland,
    GrassPodzol,
    GrassSnow,

    //Leave
    LeaveMushroomRed,
    LeaveMushroomBrown,

    //Wood
    WoodAcacia,
    WoodOak,
    WoodJungle,
    WoodSpruce,
    WoodDark,
    WoodBirch,
    WoodSakura,
    WoodMushroom,

    //Plank
    PlankAcacia,
    PlankOak,
    PlankJungle,
    PlankSpruce,
    PlankDark,
    PlankBirch,
    PlankSakura,


    //Misc
    Furnace,
    CraftingTable,
    BrickStone,
    BrickDark,
    BrickRed,

    //Ore Block
    CoalBlock,
    CopperBlock,
    DiamondBlock,
    EmeraldBlock,
    GoldBlock,
    IronBlock,
    LapisBlock,
    RedstoneBlock,

    //Misc 2
    ShroomLight,
    RedstoneLamp,

    //Elemental
    BlueIce,
    StdIce,
    Lava,
    Netherrack,
    QuartzOre,
    MagmaBlock1,
    MagmaBlock2,
    MagmaBlock3,

    //Misc 3
    Trinitrotoluene,
    QuartzBlock,
    Glowstone,
    Impostor,

    //Plant
    Cactus,

    ////=============
    //   TRANSPARENT
    ////=============

    //Leave
    LeaveAcacia,
    LeaveOak,
    LeaveJungle,
    LeaveSpruce,
    LeaveDark,
    LeaveBirch,
    LeaveSakura,
    LeaveSnow,

    //Misc 
    Glass,
    ClearIce,
    Water,


    //Bloc Breaking
    BlockBreaking0,
    BlockBreaking1,
    BlockBreaking2,
    BlockBreaking3,
    BlockBreaking4,
    BlockBreaking5,
    BlockBreaking6,
    BlockBreaking7,
    BlockBreaking8,
    BlockBreaking9,


};

class Block {
public:
    BlockType type;
    Block(BlockType bt);
    Block();
    std::string toString() const;
};

#endif //MIICRAFT_BLOCK_H
