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
public:
    Renderer renderer;
    t_coord focusedBlockPos;
    bool targetable;
    Player(f32 x, f32 y, f32 z);
    Player();

    void getFocusedBlock(World& w);

    static guVector InverseVector(const guVector& v);

    void handleRotation(WPADData *);

    void handleMovement(World& w, u16 directions, bool collision = true);

    void goLeft(guVector& normalizedLook, int speed, bool collision, World& w);

    void goRight(guVector& normalizedLook, int speed, bool collision, World& w);

    void goForward(guVector& normalizedLook, int speed, bool collision, World& w);

    void goBackward(guVector& normalizedLook, int speed, bool collision, World& w);

    void goUp(t_coord coord, bool collision, World& w);

    void goDown(t_coord coord, bool collision, World& w);

    void setPos(f32 x, f32 y, f32 z);

    void DestroyBlock(t_coord coord, World& w);
};


#endif //MIICRAFTTEST_PLAYER_H
