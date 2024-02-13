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
    Wiimote wiimote;
    Player(f32 x, f32 y, f32 z);
    Player();

    t_coord getFocusedBlock(World& w);

    static guVector InverseVector(const guVector& v);

    void handleRotation();

    void handleMovement(World& w, bool collision = true);

    bool handleInput(World& w);

    void goLeft(guVector& normalizedLook, int speed, bool collision, World& w);

    void goRight(guVector& normalizedLook, int speed, bool collision, World& w);

    void goForward(guVector& normalizedLook, int speed, bool collision, World& w);

    void goBackward(guVector& normalizedLook, int speed, bool collision, World& w);

    void goUp(t_coord coord, bool collision, World& w);

    void goDown(t_coord coord, bool collision, World& w);

    void setPos(f32 x, f32 y, f32 z);
};


#endif //MIICRAFTTEST_PLAYER_H
