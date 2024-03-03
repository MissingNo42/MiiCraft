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
    static constexpr f32 x = 0.05f, y = 0.05f;
	
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
	
	u8 renderRect(f32 x, f32 y, f32 x2, f32 y2, f32 u, f32 v, f32 u2, f32 v2) const;
	
public:
	Wiimote wiimote;
    Renderer renderer;
    Inventory inventory;
	
	int selectedSlot = 0;
	bool isValidCursor = false;
	bool craftSlot = false;
	
    bool gravity = true;
    bool isJumping = false;
    bool sprint = false;
    bool cameraLocked = false;
	bool wiimoteFocus = false;
    bool sneak = false;
    bool creative = false;
	bool focusing = false; // true -> focus must be rendered
	bool destroying = false; // true -> anim must be rendered
	
	Player(f32 x, f32 y, f32 z, int chan = WPAD_CHAN_0);
    explicit Player(int chan);

    bool getFocusedBlock();

    [[nodiscard]] f32 getFocusedBlockDistance() const;

    static guVector negateVector(const guVector& v);

    static guVector coordToGuVector(BlockCoord coord);

    void handleRotation();

    void handleGravity(BlockCoord& coord);

    void goUp(float velocity = 1.0f, bool collision = true);

    void goDown(float velocity = 1.0f, bool collision = true);

    void Jump();

    void setPos(f32 x, f32 y, f32 z);

    [[nodiscard]] int getFocusedFace() const;

    void placeBlock();

    void destroyBlock();

    void move(joystick_t sticks);

    [[nodiscard]] bool isUnderwater() const;
	
	void update();
	
	void renderFocus();
	
	void renderDestroy();
	
	/// HUD
    void renderCursor();
    void renderInventory();
    static void Underwater();
};


#endif //MIICRAFTTEST_PLAYER_H
