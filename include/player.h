//
// Created by Natha on 13/02/2024.
//

#ifndef MIICRAFTTEST_PLAYER_H
#define MIICRAFTTEST_PLAYER_H


#include "render/renderer.h"
#include <wiiuse/wpad.h>

class Player {
private:
    int speed;
    int breakingState;
    float Velocity = 0.0;
    float Acceleration = 0.0;
    bool Gravity = true;
public:
    Renderer renderer;
    t_coord focusedBlockPos;
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

    void goLeft(guVector& normalizedLook, bool collision, World& w);

    void goRight(guVector& normalizedLook, bool collision, World& w);

    void goForward(guVector& normalizedLook, bool collision, World& w);

    void goBackward(guVector& normalizedLook, bool collision, World& w);

    void goUp(t_coord coord, bool collision, World& w, float speed = 1.0f);

    void goDown(t_coord coord, bool collision, World& w, float speed = 1.0f);

    void setPos(f32 x, f32 y, f32 z);

    void DestroyBlock(t_coord coord, World& w);
};


#endif //MIICRAFTTEST_PLAYER_H
