#pragma once

#include <wiiuse/wpad.h>
#include "inventory.h"
#include "wiimote.h"
#include "engine/render/renderer.h"
#include "world/coord.h"

class Player {
    static constexpr f32 cursorWidth = 0.025f, cursorHeight = 0.025f;

    float Velocity = 0.0;
    float Acceleration = 0.12;
    int frame_cntr = 0;

    int placeDelay = 0;
    int breakingState = 0;

    BlockCoord focusedBlockPos{};
    BlockCoord lockedBlockPos{};
    BlockCoord previousFocusedBlockPos{};
    guVector focusedBlockLook{};
    BlockType focusedBlockType = Air;
	u8 focusedFace = 0;

	[[nodiscard]] bool renderBlockIcon(f32 x1, f32 y1, f32 x2, f32 y2, BlockType block) const;
	void renderCursor(f32 x, f32 y) const;

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

    void placeBlock();

    void destroyBlock();

    void move(joystick_t sticks);

    [[nodiscard]] bool isUnderwater() const;

	void update();

	void renderFocus();

	void renderDestroy();

	/// HUD
    void renderCursor() const;
    void renderInventory();
};
