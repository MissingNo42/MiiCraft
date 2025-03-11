//
// Created by Romain on 07/02/2024.
//
#include "engine/render/block.h"
#include "world/world.h"
#include "engine/env/environment.h"


[[nodiscard]] bool inline checkBlock(BlockCoord coord, BlockType type) {
	auto& block = blockData[type];

	BlockCoord floor = coord;
	floor.y--;
	auto& floorBlock = blockData[World::getBlockAt(floor).type];

	if (!floorBlock.allowAbove) { // check if can be "attached" to side blocks
		return false;
	}

	if (block.needFloor && !floorBlock.isFloor) {
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
///////////////////////////////// Spawn //////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool spawnDefault(BlockCoord coord, BlockType type, Direction dir) {
	auto& block = blockData[type];

	if (!checkBlock(coord, type)) {
		return false;
	}

	Block bk = {type, {0}};

	if (block.isOrientable) {
		bk.orient = dir ^ 2; // facing the player
	}

	World::setBlockAt(coord, bk);
	return true;
}

bool spawnDoorLow(BlockCoord coord, BlockType type, Direction dir) {
	auto& block = blockData[type];
	BlockCoord up = {coord.y + 1, coord.x, coord.z};

	if (!checkBlock(coord, type) || !checkBlock(up, (BlockType)(type + 1))) {
		return false;
	}

	Block bkl = {type, {0}};
	Block bkh = {(BlockType)(type + 1), {0}};

	bkh.orient = bkl.orient = dir ^ 2; // facing the player

	World::setBlockAt(coord, bkl);
	World::setBlockAt(up, bkh);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////// Despawn /////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void despawnDefault(BlockCoord coord) {
	World::setBlockAt(coord, {BlockType::Air, {0}});
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////// Interact ////////////////////////////////
//////////////////////////////////////////////////////////////////////////


bool interactDoorLow(BlockCoord coord, Interaction interaction) {
	Block bk = World::getBlockAt(coord);
	switch (interaction) {
		case Interaction::Redstone:
		case Interaction::Hand:
			bk.state ^= 1; // open/close
			return true;
		default:
			return false;
	}
}

bool interactDoorHigh(BlockCoord coord, Interaction interaction) {
	return interactDoorLow({coord.y - 1, coord.x, coord.z}, interaction);
}



//////////////////////////////////////////////////////////////////////////
///////////////////////////////// Render /////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void renderVoid() {
}

//////////////////////////////////////////////////////////////////////////
/////////////////////////////// Block Data ///////////////////////////////
//////////////////////////////////////////////////////////////////////////

const BlockData blockData[]{

		{
				// Air
				.isSelectable = 0,
				.isFloor = 0,
				.isTransparent = 1,
				.isLightFilter = 0,
			},

		/// Transparent Blocks

		{
				// Glass
				.tc = TILE_INDEX_ALL(14, 0),
				.isTransparent = 1,
				.isLightFilter = 0,
		},
		{
				// ClearIce
				.tc = TILE_INDEX_ALL(15, 2),
				.isTransparent = 1,
		},
		{
				// Water
				.tc = TILE_INDEX_ALL(0, static_cast<u16>(TextureIndex::WATER)),
				.isSelectable = 0,
				.isTransparent = 1,
		},

		/// Semi-Transparent Blocks

		// //Leave
		{
				// LeaveAcacia
				.tc = TILE_INDEX_ALL(5, 0),
				.isTransparent = 1,
		},
		{
				// LeaveOak
				.tc = TILE_INDEX_ALL(5, 1),
				.isTransparent = 1,
		},
		{
				// LeaveJungle
				.tc = TILE_INDEX_ALL(5, 2),
				.isTransparent = 1,
		},
		{
				// LeaveSpruce
				.tc = TILE_INDEX_ALL(5, 3),
				.isTransparent = 1,
		},
		{
				// LeaveDark
				.tc = TILE_INDEX_ALL(5, 4),
				.isTransparent = 1,
		},
		{
				// LeaveBirch
				.tc = TILE_INDEX_ALL(5, 5),
				.isTransparent = 1,
		},
		{
				// LeaveSakura
				.tc = TILE_INDEX_ALL(5, 6),
				.isTransparent = 1,
		},
		{
				// LeaveSnow
				.tc = TILE_INDEX_SIDE(5, 9,
									  3, 9,
									  5, 3),
				.isTransparent = 1,
		},

		/// Opaque Blocks

		//Mineral
		{
				// Bedrock

				.tc = TILE_INDEX_ALL(0, 0)
		},
		{
				// Stone

				.tc = TILE_INDEX_ALL(0, 1)
		},
		{
				// Andesite

				.tc = TILE_INDEX_ALL(0, 2)
		},
		{
				// SandStone

				.tc = TILE_INDEX_ALL(0, 3)
		},
		{
				// CoalOre

				.tc = TILE_INDEX_ALL(0, 4)
		},
		{
				// CopperOre

				.tc = TILE_INDEX_ALL(0, 5)
		},
		{
				// DiamondOre

				.tc = TILE_INDEX_ALL(0, 6)
		},
		{
				// EmeraldOre

				.tc = TILE_INDEX_ALL(0, 7)
		},
		{
				// GoldOre

				.tc = TILE_INDEX_ALL(0, 8)
		},
		{
				// IronOre

				.tc = TILE_INDEX_ALL(0, 9)
		},
		{
				// LapisOre

				.tc = TILE_INDEX_ALL(0, 10)
		},
		{
				// RedstoneOre

				.tc = TILE_INDEX_ALL(0, 11)
		},
		{
				// Ametyst

				.tc = TILE_INDEX_ALL(0, 12)
		},
		{
				// Granite

				.tc = TILE_INDEX_ALL(0, 13)
		},
		{
				// Diorite

				.tc = TILE_INDEX_ALL(0, 14)
		},

		// //Clay
		{
				// Clay

				.tc = TILE_INDEX_ALL(1, 0)
		},
		{
				// ClayWhite

				.tc = TILE_INDEX_ALL(1, 1)
		},
		{
				// ClayBrown

				.tc = TILE_INDEX_ALL(1, 2)
		},
		{
				// ClayBlack

				.tc = TILE_INDEX_ALL(1, 3)
		},
		{
				// ClayGray

				.tc = TILE_INDEX_ALL(1, 4)
		},
		{
				// ClayYellow

				.tc = TILE_INDEX_ALL(1, 5)
		},
		{
				// ClayRed

				.tc = TILE_INDEX_ALL(1, 6)
		},
		{
				// ClayLightGray

				.tc = TILE_INDEX_ALL(1, 7)
		},
		{
				// ClayPurple

				.tc = TILE_INDEX_ALL(1, 8)
		},
		{
				// ClayGreen

				.tc = TILE_INDEX_ALL(1, 9)
		},


		// //Powder
		{
				// Dirt

				.tc = TILE_INDEX_ALL(2, 0)
		},
		{
				// Sand

				.tc = TILE_INDEX_ALL(2, 1)
		},
		{
				// RedSand

				.tc = TILE_INDEX_ALL(2, 2)
		},
		{
				// Gravel

				.tc = TILE_INDEX_ALL(2, 3)
		},

		// //Soil
		{
				// GrassSavanna,
				.tc = TILE_INDEX_SIDE(4, 0, 3, 0, 2, 0)
		},
		{
				// GrassTemperate,
				.tc = TILE_INDEX_SIDE(4, 1, 3, 1, 2, 0)
		},
		{
				// GrassJungle,
				.tc = TILE_INDEX_SIDE(4, 2, 3, 2, 2, 0)
		},
		{
				// GrassTaiga,
				.tc = TILE_INDEX_SIDE(4, 3, 3, 3, 2, 0)
		},
		{
				// GrassDark,
				.tc = TILE_INDEX_SIDE(4, 4, 3, 4, 2, 0)
		},
		{
				// GrassCold,
				.tc = TILE_INDEX_SIDE(4, 5, 3, 5, 2, 0)
		},
		{
				// GrassSakura,
				.tc = TILE_INDEX_SIDE(4, 6, 3, 6, 2, 0)
		},
		{
				// GrassBadland,
				.tc = TILE_INDEX_SIDE(4, 7, 3, 7, 2, 0)
		},
		{
				// GrassPodzol,
				.tc = TILE_INDEX_SIDE(4, 8, 3, 8, 2, 0)
		},
		{
				// GrassSnow,
				.tc = TILE_INDEX_SIDE(4, 9, 3, 9, 2, 0)
		},

		////Leave
		{
				// LeaveMushroomRed
				.tc = TILE_INDEX_ALL(5, 7)
		},
		{
				// LeaveMushroomBrown
				.tc = TILE_INDEX_ALL(5, 8)
		},

		// //Wood
		{
				// WoodAcacia
				.tc = TILE_INDEX_SIDE(6, 0, 7, 0, 7, 0)
		},
		{
				// WoodOak
				.tc = TILE_INDEX_SIDE(6, 1, 7, 1, 7, 1)
		},
		{
				// WoodJungle
				.tc = TILE_INDEX_SIDE(6, 2, 7, 2, 7, 2)
		},
		{
				// WoodSpruce
				.tc = TILE_INDEX_SIDE(6, 3, 7, 3, 7, 3)
		},
		{
				// WoodDark
				.tc = TILE_INDEX_SIDE(6, 4, 7, 4, 7, 4)
		},
		{
				// WoodBirch
				.tc = TILE_INDEX_SIDE(6, 5, 7, 5, 7, 5)
		},
		{
				// WoodSakura
				.tc = TILE_INDEX_SIDE(6, 6, 7, 6, 7, 6)
		},
		{
				// WoodMushroom
				.tc = TILE_INDEX_SIDE(6, 7, 7, 7, 7, 7)
		},

		// //Plank
		{
				// PlankAcacia
				.tc = TILE_INDEX_ALL(8, 0)
		},
		{
				// PlankOak
				.tc = TILE_INDEX_ALL(8, 1)
		},
		{
				// PlankJungle
				.tc = TILE_INDEX_ALL(8, 2)
		},
		{
				// PlankSpruce
				.tc = TILE_INDEX_ALL(8, 3)
		},
		{
				// PlankDark
				.tc = TILE_INDEX_ALL(8, 4)
		},
		{
				// PlankBirch
				.tc = TILE_INDEX_ALL(8, 5)
		},
		{
				// PlankSakura
				.tc = TILE_INDEX_ALL(8, 6)
		},


		/// Misc
		{
				//Crafting table
				.tc = TILE_INDEX_FACE(13, 5, 13, 3, 13, 4),
				//.isInteractive = 1
			},
		{
				// BrickStone
				.tc = TILE_INDEX_ALL(14, 1)
		},
		{
				// BrickDark
				.tc = TILE_INDEX_ALL(14, 2)
		},
		{
				// BrickRed
				.tc = TILE_INDEX_ALL(14, 3)
		},

		// //Ore Block
		{
				// CoalBlock
				.tc = TILE_INDEX_ALL(14, 4)
		},
		{
				// CopperBlock
				.tc = TILE_INDEX_ALL(14, 5)
		},
		{
				// DiamondBlock
				.tc = TILE_INDEX_ALL(14, 6)
		},
		{
				// EmeraldBlock
				.tc = TILE_INDEX_ALL(14, 7)
		},
		{
				// GoldBlock
				.tc = TILE_INDEX_ALL(14, 8)
		},
		{
				// IronBlock
				.tc = TILE_INDEX_ALL(14, 9)
		},
		{
				// LapisBlock
				.tc = TILE_INDEX_ALL(14, 10)
		},
		{
				// RedstoneBlock
				.tc = TILE_INDEX_ALL(14, 11)
		},

		// //Misc 2
		{
				// ShroomLight
				.tc = TILE_INDEX_ALL(14, 12),
				.emittedLight = 15,
		},
		{
				// RedstoneLamp
				.tc = TILE_INDEX_ALL(14, 13),
				.emittedLight = 15,
		},

		// //Elemental
		{
				// BlueIce
				.tc = TILE_INDEX_ALL(15, 0)
		},
		{
				// StdIce
				.tc = TILE_INDEX_ALL(15, 1)
		},
		{
				// Lava
				.tc = TILE_INDEX_ALL(15, 4),
				.emittedLight = 15,
		},
		{
				// Netherrack
				.tc = TILE_INDEX_ALL(15, 5)
		},
		{
				// QuartzOre
				.tc = TILE_INDEX_ALL(15, 6)
		},
		{
				// MagmaBlock1
				.tc = TILE_INDEX_ALL(15, 7)
		},
		{
				// MagmaBlock2
				.tc = TILE_INDEX_ALL(15, 8)
		},
		{
				// MagmaBlock3
				.tc = TILE_INDEX_ALL(15, 9)
		},
		{
				// Snow
				.tc = TILE_INDEX_ALL(3, 9)
		},

		// //Misc 3
		{
				// Trinitrotoluene TNT
				.tc = TILE_INDEX_SIDE(15, 10, 15, 8, 15, 9)
		},
		{
				// QuartzBlock
				.tc = TILE_INDEX_ALL(15, 11)
		},
		{
				// Glowstone
				.tc = TILE_INDEX_ALL(15, 12),
				.emittedLight = 15,
		},
		{
				// Impostor
				.tc = TILE_INDEX_ALL(0, 15)
		},

		// //Plant
		{
				//Cactus
				.tc = TILE_INDEX_SIDE(9, 1, 9, 0, 9, 0)
		},


		//Block Breaking
		{
				// BlockBreaking0
				.tc = TILE_INDEX_ALL(6, 14)
		},
		{
				// BlockBreaking1
				.tc = TILE_INDEX_ALL(7, 14)
		},
		{
				// BlockBreaking2
				.tc = TILE_INDEX_ALL(8, 14)
		},
		{
				// BlockBreaking3
				.tc = TILE_INDEX_ALL(9, 14)
		},
		{
				// BlockBreaking4
				.tc = TILE_INDEX_ALL(10, 14)
		},
		{
				// BlockBreaking5
				.tc = TILE_INDEX_ALL(11, 14)
		},
		{
				// BlockBreaking6
				.tc = TILE_INDEX_ALL(12, 14)
		},
		{
				// BlockBreaking7
				.tc = TILE_INDEX_ALL(13, 14)
		},
		{
				// BlockBreaking8
				.tc = TILE_INDEX_ALL(14, 14)
		},
		{
				// BlockBreaking9
				.tc = TILE_INDEX_ALL(15, 14)
		},
		{
				//Furnace
				.tc = TILE_INDEX_FACE(13, 1, 13, 2, 13, 0),
				.isOrientable = 1,
		},
		{
			// DoorLow
			.tc = {
				static_cast<u16>(TextureIndex::WOOD_DOOR_NORTH),
				static_cast<u16>(TextureIndex::WOOD_DOOR_EAST),
				static_cast<u16>(TextureIndex::WOOD_DOOR_SOUTH),
				static_cast<u16>(TextureIndex::WOOD_DOOR_WEST),
				static_cast<u16>(TextureIndex::WOOD_DOOR_TOP),
				static_cast<u16>(TextureIndex::WOOD_DOOR_BOTTOM)
			},
			.isOrientable = 1,
			.interactive = interactDoorLow,
		},
		{
			// DoorHigh
			.tc = TILE_INDEX_ALL(0, 0), // unused
			.isOrientable = 1,
			.interactive = interactDoorHigh,
		},
};
