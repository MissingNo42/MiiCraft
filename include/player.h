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
    float Velocity = 0.0;
    float Acceleration = 0.12;
public:
    int placeDelay;
    bool gravity = true;
    bool isJumping = false;
    Renderer renderer;
    t_coord focusedBlockPos;
    t_coord lockedBlockPos;
    BlockType focusedBlockType;
    t_coord previousFocusedBlockPos;
    guVector focusedBlockLook;
    bool sprint;
    bool cameraLocked;
    int breakingState{};
    int selected_spot = 0;
    bool sneak = false;
    BlockType hotbar[9] = {BlockType::Stone, BlockType::Dirt, BlockType::PlankOak, BlockType::WoodOak, BlockType::LeaveOak, BlockType::Glass, BlockType::Bedrock, BlockType::Glowstone, BlockType::Sand};
    Player(f32 x, f32 y, f32 z);
    Player();

    bool getFocusedBlock(World& w);

    f32 getFocusedBlockDistance() const;

    static guVector InverseVector(const guVector& v);

    static guVector coordToGuVector(t_coord coord);

    static t_coord guVectorToCoord(guVector v);

    void handleRotation(WPADData *);

    void handleGravity(World& w, t_coord& coord);

    void goUp(t_coord coord, World& w, float velocity = 1.0f, bool collision = true);

    void goDown(t_coord coord, World& w, float velocity = 1.0f, bool collision = true);

    void Jump();

    void setPos(f32 x, f32 y, f32 z);

    int getFocusedFace(World& w) const;

    void placeBlock(World& w);

    void destroyBlock(World& w);

    void move(World& w, joystick_t sticks);
};


#endif //MIICRAFTTEST_PLAYER_H
