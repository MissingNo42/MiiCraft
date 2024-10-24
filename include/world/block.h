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
#define isAir(x) (!(x))

/**
 * @macro isIrregular
 * @brief Check if a block is irregular (not a cube)
 * */
#define isIrregular(x) ((x) < BlockType::REGULAR)

/**
 * @macro isRegular
 * @brief Check if a block is an regular (a cube)
 * */
#define isRegular(x) ((x) < BlockType::IRREGULAR)

/**
 * @enum isTransparent
 * @brief Check if a block is a transparent cube block
 * */
#define isTransparent(x) ((x) < BlockType::SEMITRANSPARENT)

/**
 * @enum isSemiTransparent
 * @brief Check if a block is a semi-transparent cube block
 * */
#define isSemiTransparent(x) ((x) >= BlockType::SEMITRANSPARENT && (x) < BlockType::OPAQUE)

/**
 * @enum isOpaque
 * @brief Check if a block is an opaque cube block
 * */
#define isOpaque(x) ((x) >= BlockType::OPAQUE && (x) < BlockType::IRREGULAR)

/**
 * @enum BlockType
 * @brief Enumerate all the block types
 * @details faces rendering relations:
 * [T][T] -> [T  T] or [T][T']
 * [T][S] -> [T][S]
 * [T][O] -> [T [O]
 * [T][I] -> [T??I]
 *
 * [S][T] -> [S][T]
 * [S][S] -> [S][S]
 * [S][O] -> [S [O] or [S][O] with backface culling disabled
 * [S][I] -> [S??I]
 *
 * [O][T] -> [O] T]
 * [O][S] -> [O] S]
 * [O][O] -> [O  O]
 * [O][I] -> [O??I]
 * */
enum BlockType : u8 {

	/// Transparent Blocks: no texture or alpha channel (alpha in [0: 255])
	TRANSPARENT = 0,
	Air = 0,
    Glass,
    ClearIce,
    Water,
	
	/// Semi-Transparent Blocks: texture with partial full transparency (alpha = 0 or 255)
    LeaveAcacia,
	SEMITRANSPARENT = LeaveAcacia,
    LeaveOak,
    LeaveJungle,
    LeaveSpruce,
    LeaveDark,
    LeaveBirch,
    LeaveSakura,
    LeaveSnow,
	
	/// Opaque Blocks: no transparency (alpha = 255)
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

    Snow,

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
	
	
	/// Irregular Blocks: block than require special rendering / non-cube
	IRREGULAR,
    Furnace = IRREGULAR,
	DoorLow,
	DoorHigh,
	// Doors, flowers, ...

};

/**
 * @struct Block
 * @brief Structure representing a "living" block with its individual attributes
 * */
struct Block {
	// Block type
	BlockType type; // 0 = Air
	
	// Flags attributes
	union {
		u8 flags;
		u8 light;
		
		struct { // used for (Semi)Transparent blocks
			u8 naturalLight: 4;    // 0: no light, 15: full light
			u8 artificialLight: 4; // 0: no light, 15: full light
		};
		
		struct { // used for orientable blocks
			u8 orient: 2; // 0: north, 1: east, 2: south, 3: west
			u8 state: 6; // block impl specific (e.g.: irregular blocks, redstone states, ...)
		};
	};
};

union Light {
	u8 light;
	
	struct {
		u8 naturalLight: 4;    // 0: no light, 15: full light
		u8 artificialLight: 4; // 0: no light, 15: full light
	};
};

#endif //MIICRAFT_BLOCK_H

