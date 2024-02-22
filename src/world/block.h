//
// Created by paulo on 06/02/2024.
//

#ifndef MIICRAFT_BLOCK_H
#define MIICRAFT_BLOCK_H
#include <iostream>
#include <gctypes.h>

/**
 * @macro isAir
 * @brief Check if a block is air
 * */
#define isAir(x) ((x) <= BlockType::Air)

/**
 * @macro isIrregular
 * @brief Check if a block is irregular (not a cube)
 * */
#define isIrregular(x) ((x) < BlockType::REGULAR)

/**
 * @macro isRegular
 * @brief Check if a block is an regular (a cube)
 * */
#define isRegular(x) ((x) >= BlockType::REGULAR)

/**
 * @enum isTransparent
 * @brief Check if a block is a transparent cube block
 * */
#define isTransparent(x) ((x) >= BlockType::TRANSPARENT && (x) < BlockType::OPAQUE)

/**
 * @enum isSemiTransparent
 * @brief Check if a block is a semi-transparent cube block
 * */
#define isSemiTransparent(x) ((x) >= BlockType::SEMITRANSPARENT && (x) < BlockType::TRANSPARENT)

/**
 * @enum isOpaque
 * @brief Check if a block is an opaque cube block
 * */
#define isOpaque(x) ((x) >= BlockType::OPAQUE)

/**
 * @enum BlockType
 * @brief Enumerate all the block types
 * */
enum BlockType : u8 {
    /// Air
	
    Air0 =      0, //Darkest value of light
    Air1 =      1,
    Air2 =      2,
    Air3 =      3,
    Air4 =      4,
    Air5 =      5,
    Air6 =      6,
    Air7 =      7,
    Air8 =      8,
    Air9 =      9,
    Air10 =     10,
    Air11 =     11,
    Air12 =     12,
    Air13 =     13,
    Air14 =     14,
    Air15 =     15, //Brightest value of light
	Air = 15,
	
	/// Irrgular Blocks
	
	// Doors, flowers, ...
	
	/// Semi-Transparent Blocks
	
    //Leave
    LeaveAcacia,
	SEMITRANSPARENT = LeaveAcacia,
	REGULAR = SEMITRANSPARENT,
    LeaveOak,
    LeaveJungle,
    LeaveSpruce,
    LeaveDark,
    LeaveBirch,
    LeaveSakura,
    LeaveSnow,
	
	/// Transparent Blocks

    //Misc
    Glass,
	TRANSPARENT = Glass,
    ClearIce,
    Water,

	/// Opaque Blocks
	
    //Mineral
	Bedrock,
    OPAQUE = Bedrock,
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


#endif //MIICRAFT_BLOCK_H

