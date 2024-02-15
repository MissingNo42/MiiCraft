//
// Created by Natha on 13/02/2024.
//

#ifndef MIICRAFTTEST_PLAYER_H
#define MIICRAFTTEST_PLAYER_H


#include "render/renderer.h"
#include <wiiuse/wpad.h>
#include <vector>

class Player {
private:
    int speed;
    int breakingState;
    float Velocity = 0.0;
    float Acceleration = 0.0;
    bool gravity = true;
    bool isJumping = false;
public:
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

    void goDown(t_coord coord, bool collision, World& w, float speed = 1.0f);

    void Jump();

    void setPos(f32 x, f32 y, f32 z);

    int getFocusedFace(World& w) const;

    void placeBlock(World& w) const;

    void destroyBlock(World& w);

    void updateMove(guVector normalizedLook, guVector &move, char direction);
};


#endif //MIICRAFTTEST_PLAYER_H
