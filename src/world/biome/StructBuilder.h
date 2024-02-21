//
// Created by Martin on 20/02/2024.
//

#ifndef MIICRAFTTEST_STRUCTBUILDER_H
#define MIICRAFTTEST_STRUCTBUILDER_H

#include "../block.h"
#include "../world.h"

#define INIT_STRUCT_CONSTRUCTION \
t_coord blockPos(structPos.x, structPos.y, structPos.z);\
BlockType checkedBlock;


#define PLACE_BLOCK_SOFT(blockType) \
checkedBlock = w.getBlockAt(blockPos).type;\
if ((checkedBlock <= Air14) ||\
(checkedBlock >= LeaveAcacia && checkedBlock <= LeaveSnow) ||\
checkedBlock == Water   ||\
checkedBlock == Lava)\
{w.setBlockAt(blockPos, blockType, false);}

#define PLACE_BLOCK_HARD(blockType) \
{w.setBlockAt(blockPos, blockType, false);}

#define X_ABSOLUTE(increment) blockPos.x = structPos.x + increment;
#define Y_ABSOLUTE(increment) blockPos.y = structPos.y + increment;
#define Z_ABSOLUTE(increment) blockPos.z = structPos.z + increment;

// Pose le block si Feuilles ou Air deja présent
enum StructType
{
    OakTree = 0,
    BirchTree,
    AcaciaTree,
    DarkOakTree,
    SakuraTree,
    JungleTree,
    SpruceTree,
    SnowySpruceTree,
    RedMushroomTree,
    BrownMushroomTree,
    CactusTree,
    Igloo,
    House
};

class StructBuilder{
private:
public:
    static void generateStdTree(World& w, t_coord structPos, StructType treeType = OakTree)
    {
        BlockType wood; BlockType leave;
        if (treeType == BirchTree) {wood = WoodBirch; leave = LeaveBirch;}
        else if (treeType == SakuraTree) {wood = WoodSakura; leave = LeaveSakura;}
        else {wood = WoodOak; leave = LeaveOak;}
        //t_coord blockPos(structPos.x, structPos.y, structPos.z);\
        //BlockType checkedBlock;
        INIT_STRUCT_CONSTRUCTION;

        int height = rand() % 4 + 1;
        //On construit le tronc :
        for (int y = 0; y < 3+ height ; y++) {
            PLACE_BLOCK_HARD(wood);
            blockPos.y++;
        }

        //On construit les feuilles :
        for(int h = 0; h < 2; h++){
            Y_ABSOLUTE(height + h);
            for (int i = -2; i < 3; i++) {
                for (int j = -2; j < 3; j++) {
                    if(i == 0 && j == 0){
                        continue;
                    }
                    X_ABSOLUTE(i);
                    Z_ABSOLUTE(j);
                    if ((i == 2 || i == -2) && ( j == 2 || j == -2)) {
                        if (rand() %2)
                            {PLACE_BLOCK_SOFT(leave);}
                    }
                    else
                        { PLACE_BLOCK_SOFT(leave);}
                }
            }
        }
        Y_ABSOLUTE(2 + height);
        for(int i = -1; i < 2; i++){
            for(int j = -1; j < 2; j++){
                if(i == 0 && j == 0){
                    continue;
                }
                if ((i == 1 || i == -1) && ( j == 1 || j == -1)) {
                    if (rand() %2)
                    {
                        X_ABSOLUTE(i);
                        Z_ABSOLUTE(j);
                        PLACE_BLOCK_SOFT(leave);
                    }
                }
                else
                {
                    X_ABSOLUTE(i);
                    Z_ABSOLUTE(j);
                    PLACE_BLOCK_SOFT(leave);
                }
            }
        }
        Y_ABSOLUTE(3 + height);
        X_ABSOLUTE(1);
        Z_ABSOLUTE(0);
        PLACE_BLOCK_SOFT(leave);

        X_ABSOLUTE(-1);
        PLACE_BLOCK_SOFT(leave);

        X_ABSOLUTE(0);
        for (int i = -1; i < 2; ++i) {
            Z_ABSOLUTE(i);
            PLACE_BLOCK_SOFT(leave);
        }
    }

    static void generateCactus(World& w, t_coord structPos)
    {
        INIT_STRUCT_CONSTRUCTION;
        int height = rand()%18;
        if (height < 11)        {height = 1;} // 11/18
        else if (height < 16)   {height = 2;} //  5/18
        else                    {height = 3;} //  2/18

        for (int i = 0; i < height; ++i) {
            PLACE_BLOCK_SOFT(Cactus);
            blockPos.y++;
        }
    }



    static void generateIgloo(World& w, t_coord structPos)
    {
        INIT_STRUCT_CONSTRUCTION;
        X_ABSOLUTE(-3);
        Z_ABSOLUTE(-3);
        // Génère le dome
        for (int h = 0; h < 5; ++h) {
            for (int i = -3; i < 4; ++i) {
                for (int j = -3; j < 4; ++j) {
                    if (h == 0)
                    {
                        if (abs(i) + abs(j)  <= 4 )
                        {
                            PLACE_BLOCK_HARD(BlockType::Snow);
                        }
                    }
                    else if (h < 3)
                    {
                        int dist = abs(i) + abs(j);
                        if (dist == 4 )
                        {
                            PLACE_BLOCK_HARD(BlockType::Snow);
                        }
                        else if (dist < 4)
                        {
                            PLACE_BLOCK_HARD(Air);
                        }
                    }
                    else if (h < 4)
                    {
                        float dist = sqrt(i*i + j*j);
                        if (dist <= 2.5f && dist > 1.8)
                        {
                            PLACE_BLOCK_HARD(BlockType::Snow);
                        }
                        else if (dist < 1.8)
                        {
                            PLACE_BLOCK_HARD(Air);
                        }
                    }
                    else
                    {
                        if (sqrt(i*i + j*j) <= 1.8f)
                            PLACE_BLOCK_HARD(BlockType::Snow);
                    }
                    blockPos.z++;
                }
                blockPos.x++;
                Z_ABSOLUTE(-3);
            }
            blockPos.y++;
            X_ABSOLUTE(-3);
        }

        //Entrée
        Y_ABSOLUTE(0)
        Z_ABSOLUTE(-4)
        for (int i = 0; i < 4; ++i) {
            for (int j = -1; j < 2; ++j) {
                X_ABSOLUTE(j)
                if (i==0)
                    {PLACE_BLOCK_HARD(BlockType::Snow);}
                else if (i<3)
                {
                    if (j != 0) {PLACE_BLOCK_HARD(BlockType::Snow);}
                    else {PLACE_BLOCK_HARD(Air);}
                }
                else
                {
                    if (j == 0) {
                        PLACE_BLOCK_HARD(BlockType::Snow);
                        blockPos.z ++;
                        PLACE_BLOCK_HARD(BlockType::Snow);
                        break;
                    }
                }
            }
            blockPos.y++;
        }

        //fenêtres
        Y_ABSOLUTE(1)
        for (int i = 0; i < 2; ++i) {
            X_ABSOLUTE(3)
            Z_ABSOLUTE(0)
            if (i==0) {PLACE_BLOCK_HARD(ClearIce);} else {PLACE_BLOCK_HARD(Snow);}
            X_ABSOLUTE(-3)
            if (i==0) {PLACE_BLOCK_HARD(ClearIce);} else {PLACE_BLOCK_HARD(Snow);}
            X_ABSOLUTE(0)
            Z_ABSOLUTE(3)
            PLACE_BLOCK_HARD(Snow);
            blockPos.y++;
        }

        //Lampes
        X_ABSOLUTE(-1)
        Y_ABSOLUTE(2)
        Z_ABSOLUTE(2)
        PLACE_BLOCK_HARD(RedstoneLamp);
        blockPos.x++;
        PLACE_BLOCK_HARD(RedstoneBlock);
        blockPos.x++;
        PLACE_BLOCK_HARD(RedstoneLamp);
    }


// ======================= CHECK STRUCT ============================

#define INIT_STRUCT_CHECK BlockType b; t_coord checkPos = structPos; checkPos.y += 2;

#define CHECK_TREE_AT_STRUCTPOS \
    b = w.getBlockAt(checkPos).type;\
    if ((b >= WoodAcacia && b <= WoodMushroom) ||\
        b == LeaveMushroomBrown ||\
        b == LeaveMushroomRed ||\
        b == Cactus          ||\
        b == Snow             )\
        {return false;}

    static bool checkClassicTree(World& w, t_coord structPos)
    {
        INIT_STRUCT_CHECK;

        for (int i = -1; i < 2; ++i) {
            for (int j = -1; j < 2; ++j) {
                checkPos.x = structPos.x + i; checkPos.z = structPos.z + j;
                CHECK_TREE_AT_STRUCTPOS;
            }
        }
        return true;
    }
};

#endif //MIICRAFTTEST_STRUCTBUILDER_H
