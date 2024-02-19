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
    BlockType focusedBlockType;
    t_coord previousFocusedBlockPos;
    guVector focusedBlockLook;
    bool isTargeting;
    bool sprint;
    int breakingState{};
    int selected_spot = 0;
    BlockType hotbar[10] = {BlockType::Stone, BlockType::Dirt, BlockType::GrassTemperate, BlockType::WoodOak, BlockType::LeaveOak, BlockType::Glass, BlockType::Bedrock, BlockType::Glowstone, BlockType::Sand, BlockType::Bedrock};
    Player(f32 x, f32 y, f32 z);
    Player();

    void getFocusedBlock(World& w);

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
