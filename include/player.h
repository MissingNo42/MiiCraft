//
// Created by Natha on 13/02/2024.
//

#ifndef MIICRAFTTEST_PLAYER_H
#define MIICRAFTTEST_PLAYER_H


#include "render/renderer.h"
#include <wiiuse/wpad.h>
#include <vector>

class Player {
    struct t_item{
        int quantity;
        BlockType blockType;
    };
private:
    int speed;
    int breakingState;
public:
    std::vector<t_item> inventory1;
    std::vector<t_item> inventory2;
    Renderer renderer;
    t_coord focusedBlockPos;
    t_coord previousFocusedBlockPos;
    bool targetable;
    Player(f32 x, f32 y, f32 z);
    Player();

    void getFocusedBlock(World& w);

    static guVector InverseVector(const guVector& v);

    static guVector coordToGuVector(t_coord coord);

    static t_coord guVectorToCoord(guVector v);

    void handleRotation(WPADData *);

    void handleMovement(World& w, u16 directions, bool collision = true);

    void handleAction(World& w, u16 actions);

    void goUp(t_coord coord, bool collision, World& w);

    void goDown(t_coord coord, bool collision, World& w);

    void setPos(f32 x, f32 y, f32 z);

    int getFocusedFace(World& w) const;

    void placeBlock(World& w) const;

    void destroyBlock(World& w);

    void updateMove(guVector normalizedLook, guVector &move, char direction);
};


#endif //MIICRAFTTEST_PLAYER_H
