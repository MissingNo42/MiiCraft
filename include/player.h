//
// Created by Natha on 13/02/2024.
//

#ifndef MIICRAFTTEST_PLAYER_H
#define MIICRAFTTEST_PLAYER_H


#include "render/renderer.h"
#include "wiimote.h"

class Player {
private:
    int speed;
public:
    Renderer renderer;
    Player(f32 x, f32 y, f32 z);
    Player();

    t_coord getFocusedBlock(World& w);

    static guVector InverseVector(const guVector& v);

    void handleRotation(Wiimote&);

    void handleMovement(World& w, t_coord focusedBlockPos, bool targetable,  Wiimote&, bool collision = true );

    bool handleInput(World& w, t_coord focusedBlockPos, Wiimote&, bool targetable);

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
