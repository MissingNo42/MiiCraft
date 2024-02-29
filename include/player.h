//
// Created by Natha on 13/02/2024.
//

#ifndef MIICRAFTTEST_PLAYER_H
#define MIICRAFTTEST_PLAYER_H


#include "engine/render/renderer.h"
#include "inventory.h"
#include "wiimote.h"
#include "world/coord.h"
#include <wiiuse/wpad.h>
#include <vector>

class Player {
    float Velocity = 0.0;
    float Acceleration = 0.12;
    int frame_cntr = 0;
	
    int placeDelay = 0;
    int breakingState = 0;
	
    BlockCoord focusedBlockPos = {0, 0, 0};
    BlockCoord lockedBlockPos = {0, 0, 0};
    BlockCoord previousFocusedBlockPos = {0, 0, 0};
    guVector focusedBlockLook = {0, 0, 0};
    BlockType focusedBlockType = Air0;
	u8 focusedFace = 0;
	
public:
	Wiimote wiimote;
    Renderer renderer;
    Inventory inventory;
    bool gravity = true;
    bool isJumping = false;
    bool sprint = false;
    bool cameraLocked = false;
	bool wiimoteFocus = false;
    bool sneak = false;
    bool creative = false;
    BlockType hotbar[9] = {BlockType::Stone, BlockType::Dirt, BlockType::PlankOak, BlockType::WoodOak, BlockType::LeaveOak, BlockType::Glass, BlockType::Bedrock, BlockType::Glowstone, BlockType::Sand};
    
	Player(f32 x, f32 y, f32 z, int chan = WPAD_CHAN_0);
    explicit Player(int chan);

    bool getFocusedBlock();

    [[nodiscard]] f32 getFocusedBlockDistance() const;

    static guVector negateVector(const guVector& v);

    static guVector coordToGuVector(BlockCoord coord);

    void handleRotation();

    void handleGravity(BlockCoord& coord);

    void goUp(BlockCoord coord, float velocity = 1.0f, bool collision = true);

    void goDown(BlockCoord coord, float velocity = 1.0f, bool collision = true);

    void Jump();

    void setPos(f32 x, f32 y, f32 z);

    [[nodiscard]] int getFocusedFace() const;

    void placeBlock();

    void destroyBlock();

    void move(joystick_t sticks);

    [[nodiscard]] bool isUnderwater() const;
	
	void update();
};


#endif //MIICRAFTTEST_PLAYER_H
