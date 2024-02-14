//
// Created by Romain on 07/02/2024.
//
#include "engine/render/block.h"

// TOP 0
// BOTTOM 1
// RIGHT 2
// LEFT 3
// FRONT 4
// BACK 5

BlocData blocData[106] {

		// {
        //     //Dirt
        //     .x = BLOCK_COORDS(2, 2, 2, 2, 2, 2),
		// 	.y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
		// },
		// {
        //     //Grass
        //     .x = BLOCK_COORDS(0, 2, 3, 3, 3, 3),
		// 	.y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
		// },
		// {
        //     //Stone
        //     .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
		// 	.y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
		// },
		// {
        //     //Bedrock
        //     .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
        //     .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
		// },
        // {
        //     //Log
        //     .x = BLOCK_COORDS(5, 5, 4, 4, 4, 4),
        //     .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        // },
        // {
        //     //Leaves
        //     .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
        //     .y = BLOCK_COORDS(11, 11, 11, 11, 11, 11)
        // },
        // {
        //     //Water
        //     .x = BLOCK_COORDS(13, 13, 13, 13, 13, 13),
        //     .y = BLOCK_COORDS(12, 12, 12, 12, 12, 12)
        // }


    // Air = 0,
          {
            //Air
            .x = { 0, 0, 0, 0, 0, 0 },
            .y = { 0, 0, 0, 0, 0, 0 }
        },

    // //Mineral
        {
        // Bedrock
        
             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // Stone

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
        // Andesite

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(2, 2, 2, 2, 2, 2)
        },
        {
        // SandStone

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(3, 3, 3, 3, 3, 3)
        },
        {
        // CoalOre

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(4, 4, 4, 4, 4, 4)
        },
        {
        // CopperOre

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(5, 5, 5, 5, 5, 5)
        },
        {
        // DiamondOre

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(6, 6, 6, 6, 6, 6)
        },
        {
        // EmeraldOre

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(7, 7, 7, 7, 7, 7)
        },
        {
        // GoldOre

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(8, 8, 8, 8, 8, 8)
        },
        {
        // IronOre

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(9, 9, 9, 9, 9, 9)
        },
        {
        // LapisOre

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(10, 10, 10, 10, 10, 10)
        },
        {
        // RedstoneOre

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(11, 11, 11, 11, 11, 11)
        },
        {
        // Ametyst

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(12, 12, 12, 12, 12, 12)
        },
        {
        // Granite

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(13, 13, 13, 13, 13, 13)
        },
        {
        // Diorite

             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },

    // //Clay
        {
        // Clay
        
             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // ClayWhite

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
        // ClayBrown

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(2, 2, 2, 2, 2, 2)
        },
        {
        // ClayBlack

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(3, 3, 3, 3, 3, 3)
        },
        {
        // ClayGray

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(4, 4, 4, 4, 4, 4)
        },
        {
        // ClayYellow

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(5, 5, 5, 5, 5, 5)
        },
        {
        // ClayRed

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(6, 6, 6, 6, 6, 6)
        },
        {
        // ClayLightGray

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(7, 7, 7, 7, 7, 7)
        },
        {
        // ClayPurple

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(8, 8, 8, 8, 8, 8)
        },
        {
        // ClayGreen

             .x = BLOCK_COORDS(1, 1, 1, 1, 1, 1),
             .y = BLOCK_COORDS(9, 9, 9, 9, 9, 9)
        },


    // //Powder
{
        // Dirt
        
             .x = BLOCK_COORDS(2, 2, 2, 2, 2, 2),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // Sand

             .x = BLOCK_COORDS(2, 2, 2, 2, 2, 2),
             .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
        // RedSand

             .x = BLOCK_COORDS(2, 2, 2, 2, 2, 2),
             .y = BLOCK_COORDS(2, 2, 2, 2, 2, 2)
        },
        {
        // Gravel

             .x = BLOCK_COORDS(2, 2, 2, 2, 2, 2),
             .y = BLOCK_COORDS(3, 3, 3, 3, 3, 3)
        },

    // //Soil
{
        // GrassSavanna,        
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // GrassTemperate,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(1, 0, 1, 1, 1, 1)
        },
        {
        // GrassJungle,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(2, 0, 2, 2, 2, 2)
        },
        {
        // GrassTaiga,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(3, 0, 3, 3, 3, 3)
        },
        {
        // GrassDark,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(4, 0, 4, 4, 4, 4)
        },
        {
        // GrassCold,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(5, 0, 5, 5, 5, 5)
        },
        {
        // GrassSakura,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(6, 0, 6, 6, 6, 6)
        },
        {
        // GrassBadland,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(7, 0, 7, 7, 7, 7)
        },
        {
        // GrassPodzol,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(8, 0, 8, 8, 8, 8)
        },
        {
        // GrassSnow,
             .x = BLOCK_COORDS(3, 2, 4, 4, 4, 4),
             .y = BLOCK_COORDS(9, 0, 9, 9, 9, 9)
        },

        ////Leave
        {
        // LeaveMushroomRed
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(7, 7, 7, 7, 7, 7)
        },
        {
        // LeaveMushroomBrown
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(8, 8, 8, 8, 8, 8)
        },

    // //Wood
{
        // WoodAcacia        
             .x = BLOCK_COORDS(7, 7, 6, 6, 6, 6),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // WoodOak
             .x = BLOCK_COORDS(7, 7, 6, 6, 6, 6),
             .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
        // WoodJungle
             .x = BLOCK_COORDS(7, 7, 6, 6, 6, 6),
             .y = BLOCK_COORDS(2, 2, 2, 2, 2, 2)
        },
        {
        // WoodSpruce
             .x = BLOCK_COORDS(7, 7, 6, 6, 6, 6),
             .y = BLOCK_COORDS(3, 3, 3, 3, 3, 3)
        },
        {
        // WoodDark
             .x = BLOCK_COORDS(7, 7, 6, 6, 6, 6),
             .y = BLOCK_COORDS(4, 4, 4, 4, 4, 4)
        },
        {
        // WoodBirch
             .x = BLOCK_COORDS(7, 7, 6, 6, 6, 6),
             .y = BLOCK_COORDS(5, 5, 5, 5, 5, 5)
        },
        {
        // WoodSakura
             .x = BLOCK_COORDS(7, 7, 6, 6, 6, 6),
             .y = BLOCK_COORDS(6, 6, 6, 6, 6, 6)
        },
        {
        // WoodMushroom
             .x = BLOCK_COORDS(7, 7, 6, 6, 6, 6),
             .y = BLOCK_COORDS(7, 7, 7, 7, 7, 7)
        },

    // //Plank
        {
        // PlankAcacia        
             .x = BLOCK_COORDS(8, 8, 8, 8, 8, 8),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // PlankOak
             .x = BLOCK_COORDS(8, 8, 8, 8, 8, 8),
             .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
        // PlankJungle
             .x = BLOCK_COORDS(8, 8, 8, 8, 8, 8),
             .y = BLOCK_COORDS(2, 2, 2, 2, 2, 2)
        },
        {
        // PlankSpruce
             .x = BLOCK_COORDS(8, 8, 8, 8, 8, 8),
             .y = BLOCK_COORDS(3, 3, 3, 3, 3, 3)
        },
        {
        // PlankDark
             .x = BLOCK_COORDS(8, 8, 8, 8, 8, 8),
             .y = BLOCK_COORDS(4, 4, 4, 4, 4, 4)
        },
        {
        // PlankBirch
             .x = BLOCK_COORDS(8, 8, 8, 8, 8, 8),
             .y = BLOCK_COORDS(5, 5, 5, 5, 5, 5)
        },
        {
        // PlankSakura
             .x = BLOCK_COORDS(8, 8, 8, 8, 8, 8),
             .y = BLOCK_COORDS(6, 6, 6, 6, 6, 6)
        },



    // //Misc
        {
        //Furnace
             .x = BLOCK_COORDS(13, 13, 13, 13, 13, 13),
             .y = BLOCK_COORDS(2, 2, 0, 1, 1, 1)
        },
        {
        //Crafting table
             .x = BLOCK_COORDS(13, 13, 13, 13, 13, 13),
             .y = BLOCK_COORDS(3, 3, 4, 4, 4, 4)
        },
        {
        // BrickStone        
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
        // BrickDark
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(2, 2, 2, 2, 2, 2)
        },
        {
        // BrickRed
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(3, 3, 3, 3, 3, 3)
        },  

    // //Ore Block
        {
        // CoalBlock        
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(4, 4, 4, 4, 4, 4)
        },
        {
        // CopperBlock
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(5, 5, 5, 5, 5, 5)
        },
        {
        // DiamondBlock
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(6, 6, 6, 6, 6, 6)
        },
        {
        // EmeraldBlock
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(7, 7, 7, 7, 7, 7)
        },
        {
        // GoldBlock
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(8, 8, 8, 8, 8, 8)
        },
        {
        // IronBlock
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(9, 9, 9, 9, 9, 9)
        },
        {
        // LapisBlock
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(10, 10, 10, 10, 10, 10)
        },
        {
        // RedstoneBlock
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(11, 11, 11, 11, 11, 11)
        },

    // //Misc 2
        {
        // ShroomLight        
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(12, 12, 12, 12, 12, 12)
        },
        {
        // RedstoneLamp
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(13, 13, 13, 13, 13, 13)
        },

    // //Elemental
        {
        // BlueIce        
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // StdIce
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
        // Lava
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(4, 4, 4, 4, 4, 4)
        },
        {
        // Netherrack
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(5, 5, 5, 5, 5, 5)
        },
        {
        // QuartzOre
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(6, 6, 6, 6, 6, 6)
        },
        {
        // MagmaBlock1
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(7, 7, 7, 7, 7, 7)
        },
        {
        // MagmaBlock2
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(8, 8, 8, 8, 8, 8)
        },
        {
        // MagmaBlock3
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(9, 9, 9, 9, 9, 9)
        },

    // //Misc 3
        {
        // Trinitrotoluene
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(10, 10, 10, 10, 10, 10)
        },
        {
        // QuartzBlock
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(11, 11, 11, 11, 11, 11)
        },
        {
        // Glowstone
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(12, 12, 12, 12, 12, 12)
        },
        {
        // Impostor
             .x = BLOCK_COORDS(0, 0, 0, 0, 0, 0),
             .y = BLOCK_COORDS(15, 15, 15, 15, 15, 15)
        },

          // //Plant
        {
        //Cactus
             .x = BLOCK_COORDS(9, 9, 9, 9, 9, 9),
             .y = BLOCK_COORDS(0, 0, 1, 1, 1, 1)
        },

//========================================
            // TRANSPARENT //
//========================================

          // //Leave
          {
        // LeaveAcacia
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // LeaveOak
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(1, 1, 1, 1, 1, 1)
        },
        {
        // LeaveJungle
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(2, 2, 2, 2, 2, 2)
        },
        {
        // LeaveSpruce
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(3, 3, 3, 3, 3, 3)
        },
        {
        // LeaveDark
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(4, 4, 4, 4, 4, 4)
        },
        {
        // LeaveBirch
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(5, 5, 5, 5, 5, 5)
        },
        {
        // LeaveSakura
             .x = BLOCK_COORDS(5, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(6, 6, 6, 6, 6, 6)
        },
        {
        // LeaveSnow
             .x = BLOCK_COORDS(3, 5, 5, 5, 5, 5),
             .y = BLOCK_COORDS(9, 3, 9, 9, 9, 9)
        },

        {
        // Glass
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(0, 0, 0, 0, 0, 0)
        },
        {
        // ClearIce
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(2, 2, 2, 2, 2, 2)
        },
        {
        // Water
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(3, 3, 3, 3, 3, 3)
        },


        //Bloc Breaking
        {
        // BlockBreaking0        
             .x = BLOCK_COORDS(6, 6, 6, 6, 6, 6),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking1
             .x = BLOCK_COORDS(7, 7, 7, 7, 7, 7),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking2
             .x = BLOCK_COORDS(8, 8, 8, 8, 8, 8),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking3
             .x = BLOCK_COORDS(9, 9, 9, 9, 9, 9),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking4
             .x = BLOCK_COORDS(10, 10, 10, 10, 10, 10),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking5
             .x = BLOCK_COORDS(11, 11, 11, 11, 11, 11),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking6
             .x = BLOCK_COORDS(12, 12, 12, 12, 12, 12),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking7
             .x = BLOCK_COORDS(13, 13, 13, 13, 13, 13),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking8
             .x = BLOCK_COORDS(14, 14, 14, 14, 14, 14),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },
        {
        // BlockBreaking9
             .x = BLOCK_COORDS(15, 15, 15, 15, 15, 15),
             .y = BLOCK_COORDS(14, 14, 14, 14, 14, 14)
        },



};