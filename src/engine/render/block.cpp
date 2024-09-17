//
// Created by Romain on 07/02/2024.
//
#include "engine/render/block.h"
#include "render/cacheUnit.h"

#define BLOC_FACE_TOP 0
#define BLOC_FACE_BOTTOM 1
#define BLOC_FACE_RIGHT 2
#define BLOC_FACE_LEFT 3
#define BLOC_FACE_FRONT 4
#define BLOC_FACE_BACK 5

BlockData blockData[]{
		
		{}, //Air
		
		/// Transparent Blocks
		
		{
				// Glass
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(0)
		},
		{
				// ClearIce
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(2)
		},
		{
				// Water
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(TextureIndex::WATER)
		},
		
		/// Semi-Transparent Blocks
		
		// //Leave
		{
				// LeaveAcacia
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(0)
		},
		{
				// LeaveOak
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(1)
		},
		{
				// LeaveJungle
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(2)
		},
		{
				// LeaveSpruce
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(3)
		},
		{
				// LeaveDark
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(4)
		},
		{
				// LeaveBirch
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(5)
		},
		{
				// LeaveSakura
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(6)
		},
		{
				// LeaveSnow
				.x = BLOCK_COORDS(3, 5, 5, 5, 5, 5),
				.y = BLOCK_COORDS(9, 3, 9, 9, 9, 9)
		},
		
		/// Opaque Blocks
		
		//Mineral
		{
				// Bedrock
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(0)
		},
		{
				// Stone
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(1)
		},
		{
				// Andesite
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(2)
		},
		{
				// SandStone
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(3)
		},
		{
				// CoalOre
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(4)
		},
		{
				// CopperOre
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(5)
		},
		{
				// DiamondOre
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(6)
		},
		{
				// EmeraldOre
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(7)
		},
		{
				// GoldOre
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(8)
		},
		{
				// IronOre
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(9)
		},
		{
				// LapisOre
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(10)
		},
		{
				// RedstoneOre
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(11)
		},
		{
				// Ametyst
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(12)
		},
		{
				// Granite
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(13)
		},
		{
				// Diorite
				
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(14)
		},
		
		// //Clay
		{
				// Clay
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(0)
		},
		{
				// ClayWhite
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(1)
		},
		{
				// ClayBrown
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(2)
		},
		{
				// ClayBlack
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(3)
		},
		{
				// ClayGray
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(4)
		},
		{
				// ClayYellow
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(5)
		},
		{
				// ClayRed
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(6)
		},
		{
				// ClayLightGray
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(7)
		},
		{
				// ClayPurple
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(8)
		},
		{
				// ClayGreen
				
				.x = BLOCK_COORDS_ALL(1),
				.y = BLOCK_COORDS_ALL(9)
		},
		
		
		// //Powder
		{
				// Dirt
				
				.x = BLOCK_COORDS_ALL(2),
				.y = BLOCK_COORDS_ALL(0)
		},
		{
				// Sand
				
				.x = BLOCK_COORDS_ALL(2),
				.y = BLOCK_COORDS_ALL(1)
		},
		{
				// RedSand
				
				.x = BLOCK_COORDS_ALL(2),
				.y = BLOCK_COORDS_ALL(2)
		},
		{
				// Gravel
				
				.x = BLOCK_COORDS_ALL(2),
				.y = BLOCK_COORDS_ALL(3)
		},
		
		// //Soil
		{
				// GrassSavanna,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(0, 0, 0)
		},
		{
				// GrassTemperate,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(1, 1, 0)
		},
		{
				// GrassJungle,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(2, 2, 0)
		},
		{
				// GrassTaiga,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(3, 3, 0)
		},
		{
				// GrassDark,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(4, 4, 0)
		},
		{
				// GrassCold,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(5, 5, 0)
		},
		{
				// GrassSakura,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(6, 6, 0)
		},
		{
				// GrassBadland,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(7, 7, 0)
		},
		{
				// GrassPodzol,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(8, 8, 0)
		},
		{
				// GrassSnow,
				.x = BLOCK_COORDS_SIDE(4, 3, 2),
				.y = BLOCK_COORDS_SIDE(9, 9, 0)
		},
		
		////Leave
		{
				// LeaveMushroomRed
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(7)
		},
		{
				// LeaveMushroomBrown
				.x = BLOCK_COORDS_ALL(5),
				.y = BLOCK_COORDS_ALL(8)
		},
		
		// //Wood
		{
				// WoodAcacia
				.x = BLOCK_COORDS_SIDE(6, 7, 7),
				.y = BLOCK_COORDS_SIDE(0, 0, 0)
		},
		{
				// WoodOak
				.x = BLOCK_COORDS_SIDE(6, 7, 7),
				.y = BLOCK_COORDS_SIDE(1, 1, 1)
		},
		{
				// WoodJungle
				.x = BLOCK_COORDS_SIDE(6, 7, 7),
				.y = BLOCK_COORDS_SIDE(2, 2, 2)
		},
		{
				// WoodSpruce
				.x = BLOCK_COORDS_SIDE(6, 7, 7),
				.y = BLOCK_COORDS_SIDE(3, 3, 3)
		},
		{
				// WoodDark
				.x = BLOCK_COORDS_SIDE(6, 7, 7),
				.y = BLOCK_COORDS_SIDE(4, 4, 4)
		},
		{
				// WoodBirch
				.x = BLOCK_COORDS_SIDE(6, 7, 7),
				.y = BLOCK_COORDS_SIDE(5, 5, 5)
		},
		{
				// WoodSakura
				.x = BLOCK_COORDS_SIDE(6, 7, 7),
				.y = BLOCK_COORDS_SIDE(6, 6, 6)
		},
		{
				// WoodMushroom
				.x = BLOCK_COORDS_SIDE(6, 7, 7),
				.y = BLOCK_COORDS_SIDE(7, 7, 7)
		},
		
		// //Plank
		{
				// PlankAcacia
				.x = BLOCK_COORDS_ALL(8),
				.y = BLOCK_COORDS_ALL(0)
		},
		{
				// PlankOak
				.x = BLOCK_COORDS_ALL(8),
				.y = BLOCK_COORDS_ALL(1)
		},
		{
				// PlankJungle
				.x = BLOCK_COORDS_ALL(8),
				.y = BLOCK_COORDS_ALL(2)
		},
		{
				// PlankSpruce
				.x = BLOCK_COORDS_ALL(8),
				.y = BLOCK_COORDS_ALL(3)
		},
		{
				// PlankDark
				.x = BLOCK_COORDS_ALL(8),
				.y = BLOCK_COORDS_ALL(4)
		},
		{
				// PlankBirch
				.x = BLOCK_COORDS_ALL(8),
				.y = BLOCK_COORDS_ALL(5)
		},
		{
				// PlankSakura
				.x = BLOCK_COORDS_ALL(8),
				.y = BLOCK_COORDS_ALL(6)
		},
		
		
		/// Misc
		{
				//Furnace
				.x = BLOCK_COORDS_SIDE(13, 13, 13),
				.y = BLOCK_COORDS_SIDE(1, 2, 2),
				.isOrientable = 1,
				.isInteractive = 1,
		},
		{
				//Crafting table
				.x = BLOCK_COORDS_SIDE(13, 13, 13),
				.y = BLOCK_COORDS_SIDE(4, 3, 3),
				.isInteractive = 1
			},
		{
				// BrickStone
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(1)
		},
		{
				// BrickDark
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(2)
		},
		{
				// BrickRed
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(3)
		},
		
		// //Ore Block
		{
				// CoalBlock
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(4)
		},
		{
				// CopperBlock
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(5)
		},
		{
				// DiamondBlock
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(6)
		},
		{
				// EmeraldBlock
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(7)
		},
		{
				// GoldBlock
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(8)
		},
		{
				// IronBlock
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(9)
		},
		{
				// LapisBlock
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(10)
		},
		{
				// RedstoneBlock
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(11)
		},
		
		// //Misc 2
		{
				// ShroomLight
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(12)
		},
		{
				// RedstoneLamp
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(13)
		},
		
		// //Elemental
		{
				// BlueIce
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(0)
		},
		{
				// StdIce
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(1)
		},
		{
				// Lava
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(4)
		},
		{
				// Netherrack
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(5)
		},
		{
				// QuartzOre
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(6)
		},
		{
				// MagmaBlock1
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(7)
		},
		{
				// MagmaBlock2
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(8)
		},
		{
				// MagmaBlock3
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(9)
		},
		
		{
				// Snow
				.x = BLOCK_COORDS_ALL(3),
				.y = BLOCK_COORDS_ALL(9)
		},
		
		// //Misc 3
		{
				// Trinitrotoluene
				.x = BLOCK_COORDS_SIDE(15, 15, 15),
				.y = BLOCK_COORDS_SIDE(10, 8, 9)
		},
		{
				// QuartzBlock
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(11)
		},
		{
				// Glowstone
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(12)
		},
		{
				// Impostor
				.x = BLOCK_COORDS_ALL(0),
				.y = BLOCK_COORDS_ALL(15)
		},
		
		// //Plant
		{
				//Cactus
				.x = BLOCK_COORDS_SIDE(9, 9, 9),
				.y = BLOCK_COORDS_SIDE(1, 0, 0)
		},
		
		
		//Block Breaking
		{
				// BlockBreaking0
				.x = BLOCK_COORDS_ALL(6),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking1
				.x = BLOCK_COORDS_ALL(7),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking2
				.x = BLOCK_COORDS_ALL(8),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking3
				.x = BLOCK_COORDS_ALL(9),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking4
				.x = BLOCK_COORDS_ALL(10),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking5
				.x = BLOCK_COORDS_ALL(11),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking6
				.x = BLOCK_COORDS_ALL(12),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking7
				.x = BLOCK_COORDS_ALL(13),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking8
				.x = BLOCK_COORDS_ALL(14),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
				// BlockBreaking9
				.x = BLOCK_COORDS_ALL(15),
				.y = BLOCK_COORDS_ALL(14)
		},
		{
			// DoorLow
			.x = BLOCK_COORDS_ALL(1),
			.y = BLOCK_COORDS_ALL(11),
			.isOrientable = 1,
			.isInteractive = 1,
		},
		{
			// DoorHigh
			.x = BLOCK_COORDS_ALL(1),
			.y = BLOCK_COORDS_ALL(10),
			.isOrientable = 1,
			.isInteractive = 1,
		},
};


void BlockData::blockInit() {

}
