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
    Oak = 0,
    BirchTree,
    AcaciaTree,
    DarkOakTree,
    CherryTree,
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
    static void generateOak(World& w, t_coord structPos)
    {
        //t_coord blockPos(structPos.x, structPos.y, structPos.z);\
        //BlockType checkedBlock;
        INIT_STRUCT_CONSTRUCTION;



        int height = rand() % 4 + 1;
        //On construit le tronc :
        for (int y = 0; y < 3+ height ; y++) {
            PLACE_BLOCK_HARD(WoodOak);
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
                            {PLACE_BLOCK_SOFT(LeaveOak);}
                    }
                    else
                        { PLACE_BLOCK_SOFT(LeaveOak);}
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
                        PLACE_BLOCK_SOFT(LeaveOak);
                    }
                }
                else
                {
                    X_ABSOLUTE(i);
                    Z_ABSOLUTE(j);
                    PLACE_BLOCK_SOFT(LeaveOak);
                }
            }
        }
        Y_ABSOLUTE(3 + height);
        X_ABSOLUTE(1);
        Z_ABSOLUTE(0);
        PLACE_BLOCK_SOFT(LeaveOak);

        X_ABSOLUTE(-1);
        PLACE_BLOCK_SOFT(LeaveOak);

        X_ABSOLUTE(0);
        for (int i = -1; i < 2; ++i) {
            Z_ABSOLUTE(i);
            PLACE_BLOCK_SOFT(LeaveOak);
        }
    }

#define INIT_STRUCT_CHECK BlockType b; t_coord checkPos = structPos; checkPos.y += 2;

#define CHECK_TREE_AT_STRUCTPOS \
    b = w.getBlockAt(checkPos).type;\
    if ((b >= WoodAcacia && b <= WoodMushroom) ||\
        b == LeaveMushroomBrown ||\
        b == LeaveMushroomRed ||\
        b == Cactus)            \
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
