//
// Created by Natha on 13/02/2024.
//

#ifndef MIICRAFTTEST_PLAYER_H
#define MIICRAFTTEST_PLAYER_H


#include "engine/render/renderer.h"
#include "inventory.h"
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
    BlockCoord focusedBlockPos;
    BlockCoord lockedBlockPos;
    BlockType focusedBlockType;
    BlockCoord previousFocusedBlockPos;
    guVector focusedBlockLook;
    bool sprint;
    bool cameraLocked;
	bool wiimoteFocus = false;
    int breakingState{};
    bool sneak = false;
    Inventory inventory;
    bool creative ;
    BlockType hotbar[9] = {BlockType::Stone, BlockType::Dirt, BlockType::PlankOak, BlockType::WoodOak, BlockType::LeaveOak, BlockType::Glass, BlockType::Bedrock, BlockType::Glowstone, BlockType::Sand};
    Player(f32 x, f32 y, f32 z);
    Player();

    bool getFocusedBlock();

    f32 getFocusedBlockDistance() const;

    static guVector InverseVector(const guVector& v);

    static guVector coordToGuVector(BlockCoord coord);

    static BlockCoord guVectorToCoord(guVector v);

    void handleRotation(WPADData *);

    void handleGravity(BlockCoord& coord);

    void goUp(BlockCoord coord, float velocity = 1.0f, bool collision = true);

    void goDown(BlockCoord coord, float velocity = 1.0f, bool collision = true);

    void Jump();

    void setPos(f32 x, f32 y, f32 z);

    [[nodiscard]] int getFocusedFace() const;

    void placeBlock();

    void destroyBlock();

    void move(joystick_t sticks);

    bool isUnderwater() const;
};


#endif //MIICRAFTTEST_PLAYER_H
