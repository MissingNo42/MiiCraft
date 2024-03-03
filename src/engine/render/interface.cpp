
#include "player.h"
#include "engine/render/bloc.h"
#include "render/cache.h"

static const f32 menuSz = 0.65;
static const f32 menuXMid = 0.2840909090909091;
static const f32 menuYMid = 0.4431818181818182;

static const f32 slotsXpad = 0.05113636363636364;
static const f32 slotsYpad = 0.48295454545454547;

static const f32 slotsXCpad = 0.17613636363636365; // craft
static const f32 slotsYCpad = 0.10227272727272728; // craft

static const f32 slotsXMpad = 0.5625; // craft (menu)
static const f32 slotsYMpad = 0.10795454545454546; // craft (menu)

static const f32 slotsXCRpad = 0.6875; // craft result
static const f32 slotsYCRpad = 0.18181818181818182; // craft result

static const f32 slotsXMRpad = 0.8806818181818182; // craft result (menu)
static const f32 slotsYMRpad = 0.16477272727272727; // craft result (menu)

static const f32 slotsXoff = 0.10227272727272728;
static const f32 slotsYoff = 0.10227272727272728;

static const f32 slotsHpad = 0.8125; // hotbar Y padding

static const f32 slotsSz = 0.07954545454545454;
static const f32 slotsCRSz = 1.5714285714285714f * slotsSz;

// hotbar
static const f32 hotbarY = -0.65;
static const f32 hotbarH = 0.13;
static const f32 hotbarW = hotbarH * 8.272727272727273f;

static const f32 selectorSz = hotbarH * 1.0909090909090908f;
static const f32 selectorOff = 0.10989010989010989f * hotbarW;
static const f32 selectorPad = 0.001953125;

static const f32 itemW = 0.07692307692307693f * hotbarW;
static const f32 itemH = 0.6363636363636364f * hotbarH;

static const f32 itemXpad = 0.02197802197802198f * hotbarW;
static const f32 itemYpad = 0.18181818181818182f * hotbarH;
static const f32 itemXoff = 0.10989010989010989f * hotbarW;

inline u8 Player::renderRect(f32 x1, f32 y1, f32 x2, f32 y2, f32 u, f32 v, f32 u2, f32 v2) const {
	GX_Position3f32(x1, y1, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(u, v);
	
	GX_Position3f32(x2, y1, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(u2, v);
	
	GX_Position3f32(x2, y2, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(u2, v2);
	
	GX_Position3f32(x1, y2, 0);
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(u, v2);
	
	f32 a = 0, b = 0;
	if (wiimote.wd->ir.valid) {
		a = wiimote.x * (1 - x);
		b = wiimote.y * (1 - y) / renderer.camera.ratio;
	}
	
	return x1 <= a && a <= x2 && y2 <= b && b <= y1;
}

void Player::renderCursor() {
	
	if (!inventory.open && !wiimoteFocus) {
		
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing
		
		renderRect(-x, y, x, -y, BLOCK_COORD(15), BLOCK_COORD(15), BLOCK_COORD(16), BLOCK_COORD(16));
		
		GX_End();
	}
	
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
	
	f32 a = 0, b = 0;
	if (wiimote.wd->ir.valid) {
		a = wiimote.x * (1 - x);
		b = wiimote.y * (1 - y) / renderer.camera.ratio;
	}
	
	renderRect(a - x, b + y, a + x, b - y, BLOCK_COORD(15), BLOCK_COORD(15), BLOCK_COORD(16), BLOCK_COORD(16));
	
	GX_End();
}

void Player::renderInventory() {
	isValidCursor = false;
	bool collide;
	
	if (inventory.open) {
		
		/// Render background menu
		
		if (inventory.craftOpen) {
			GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing
			renderRect(-menuSz, menuSz, menuSz, -menuSz, BLOCK_COORD(16), BLOCK_COORD(16), BLOCK_COORD(27), BLOCK_COORD(27));
		} else {
			GX_Begin(GX_QUADS, GX_VTXFMT0, 8); // Start drawing
			renderRect(-menuSz, menuSz, menuSz, -menuSz, BLOCK_COORD(16), BLOCK_COORD(0), BLOCK_COORD(27), BLOCK_COORD(11));
			f32 fc = 0.1;
			
			renderRect(-menuSz + menuSz * 2 * menuXMid - fc,
					   menuSz - menuSz * 2 * menuYMid + 2.0f * fc * 11.0f / 5.0f,
					   -menuSz + menuSz * 2 * menuXMid + fc,
					   menuSz - menuSz * 2 * menuYMid,
					   BLOCK_COORD(27), BLOCK_COORD(0), BLOCK_COORD(32), BLOCK_COORD(11));
		}
		GX_End();
		
		/// Render slots
		GX_Begin(GX_QUADS, GX_VTXFMT0, 108); // Start drawing
		for (int i = 0; i < 27; i++) {
			auto &tx = blocData[inventory.inventory[1 + inventory.currentPage * 3 + i / 9][i %
			                                                                                             9].item.type];
			int X = i % 9, Y = i / 9;
			collide = renderRect(-menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff),
								           menuSz  - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff),
								           -menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz),
								           menuSz  - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff + slotsSz),
								           tx.x[2], tx.y[2], tx.x[2] + OFFSET, tx.y[2] + OFFSET);
			if (collide) selectedSlot = i, craftSlot = false, isValidCursor = true;
		}
		GX_End();
		
		/// Render hotbar slots
		GX_Begin(GX_QUADS, GX_VTXFMT0, 36);
		for (int i = 0; i < 9; i++) {
			auto& tx = blocData[inventory.inventory[0][i].item.type];
			collide = renderRect(-menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff),
			           menuSz  - menuSz * 2 * slotsHpad,
			           -menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz),
			           menuSz  - menuSz * 2 * (slotsHpad + slotsSz),
			           tx.x[2], tx.y[2], tx.x[2] + OFFSET, tx.y[2] + OFFSET);
			if (collide) selectedSlot = 27 + i, craftSlot = false, isValidCursor = true;
		}
		GX_End();
		
		/// Render craft slots
		GX_Begin(GX_QUADS, GX_VTXFMT0, 36);
		if (inventory.craftOpen) {
			for (int i = 0; i < 9; i++) {
				auto& tx = blocData[inventory.craftSlots[i].item.type];
				int X = i % 3, Y = i / 3;
				collide = renderRect(-menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff),
				           menuSz  - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff),
				           -menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz),
				           menuSz  - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff + slotsSz),
				           tx.x[2], tx.y[2], tx.x[2] + OFFSET, tx.y[2] + OFFSET);
				if (collide) selectedSlot = i, craftSlot = true, isValidCursor = true;
			}
		} else {
			for (int i = 0; i < 9; i++) {
				auto& tx = blocData[inventory.craftSlots[i].item.type];
				int X = i % 3, Y = i / 3;
				collide = renderRect(-menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff),
				           menuSz  - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff),
				           -menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz),
				           menuSz  - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff + slotsSz),
				           tx.x[2], tx.y[2], tx.x[2] + OFFSET, tx.y[2] + OFFSET);
				if (collide && X != 2 && Y != 2) selectedSlot = i, craftSlot = true, isValidCursor = true;
			}
		}
		GX_End();
		
		/// Render craft result slot
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		if (inventory.craftOpen) {
			auto& tx = blocData[inventory.craftSlots[9].item.type];
			collide = renderRect(-menuSz + menuSz * 2 * (slotsXCRpad),
			           menuSz  - menuSz * 2 * (slotsYCRpad),
			           -menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz),
			           menuSz  - menuSz * 2 * (slotsYCRpad + slotsCRSz),
			           tx.x[2], tx.y[2], tx.x[2] + OFFSET, tx.y[2] + OFFSET);
			if (collide) selectedSlot = 9, craftSlot = true, isValidCursor = true;
		} else {
			auto& tx = blocData[inventory.craftSlots[9].item.type];
			collide = renderRect(-menuSz + menuSz * 2 * (slotsXMRpad),
			           menuSz  - menuSz * 2 * (slotsYMRpad),
			           -menuSz + menuSz * 2 * (slotsXMRpad + slotsSz),
			           menuSz  - menuSz * 2 * (slotsYMRpad + slotsSz),
			           tx.x[2], tx.y[2], tx.x[2] + OFFSET, tx.y[2] + OFFSET);
			if (collide) selectedSlot = 9, craftSlot = true, isValidCursor = true;
		}
		GX_End();
		
		/// Render quantity for result
		if (inventory.craftOpen) {
			if (inventory.craftSlots[9].quantity > 0) {
				int qty = inventory.craftSlots[9].quantity;
				int D = 16 + (qty / 10), U = 16 + (qty % 10);
				
				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
				
				renderRect(-menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz / 2),
				           menuSz  - menuSz * 2 * (slotsYCRpad + slotsCRSz / 2),
				           -menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz / 2 + slotsCRSz / 4),
				           menuSz  - menuSz * 2 * (slotsYCRpad + slotsCRSz / 2 + slotsCRSz / 4),
				           BLOCK_COORD(D), BLOCK_COORD(12), BLOCK_COORD(D + 1), BLOCK_COORD(13));
				
				renderRect(-menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz / 2 + slotsCRSz / 4),
				           menuSz  - menuSz * 2 * (slotsYCRpad + slotsCRSz / 2),
				           -menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz / 2 + slotsCRSz / 2),
				           menuSz  - menuSz * 2 * (slotsYCRpad + slotsCRSz / 2 + slotsCRSz / 4),
				           BLOCK_COORD(U), BLOCK_COORD(12), BLOCK_COORD(U + 1), BLOCK_COORD(13));
				
				GX_End();
			}
		} else {
			if (inventory.craftSlots[9].quantity > 0) {
				int qty = inventory.craftSlots[9].quantity;
				int D = 16 + (qty / 10), U = 16 + (qty % 10);
				
				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
				
				renderRect(-menuSz + menuSz * 2 * (slotsXMRpad + slotsSz / 2),
				           menuSz  - menuSz * 2 * (slotsYMRpad + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXMRpad + slotsSz / 2 + slotsSz / 4),
				           menuSz  - menuSz * 2 * (slotsYMRpad + 2 * slotsSz / 3 + slotsSz / 4),
				           BLOCK_COORD(D), BLOCK_COORD(12), BLOCK_COORD(D + 1), BLOCK_COORD(13));
				
				renderRect(-menuSz + menuSz * 2 * (slotsXMRpad + slotsSz / 2 + slotsSz / 4),
				           menuSz  - menuSz * 2 * (slotsYMRpad + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXMRpad + slotsSz / 2 + slotsSz / 2),
				           menuSz  - menuSz * 2 * (slotsYMRpad + 2 * slotsSz / 3 + slotsSz / 4),
				           BLOCK_COORD(U), BLOCK_COORD(12), BLOCK_COORD(U + 1), BLOCK_COORD(13));
				
				GX_End();
			}
		}
		
		/// Render quantity for craft
		for (int i = 0; i < 9; i++) {
			if (inventory.craftOpen) {
				if (inventory.craftSlots[i].quantity > 0) {
					int qty = inventory.craftSlots[i].quantity;
					int D = 16 + (qty / 10), U = 16 + (qty % 10);
					int X = i % 3, Y = i / 3;
					
					GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
					
					renderRect(-menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz / 2),
					           menuSz - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff + slotsSz / 2),
					           -menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
					           menuSz - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff + slotsSz / 2 + slotsSz / 4),
					           BLOCK_COORD(D), BLOCK_COORD(12), BLOCK_COORD(D + 1), BLOCK_COORD(13));
					
					renderRect(-menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
					           menuSz - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff + slotsSz / 2),
					           -menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 2),
					           menuSz - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff + slotsSz / 2 + slotsSz / 4),
					           BLOCK_COORD(U), BLOCK_COORD(12), BLOCK_COORD(U + 1), BLOCK_COORD(13));
					
					GX_End();
				}
			} else {
				if (inventory.craftSlots[i].quantity > 0) {
					int qty = inventory.craftSlots[i].quantity;
					int D = 16 + (qty / 10), U = 16 + (qty % 10);
					int X = i % 3, Y = i / 3;
					
					GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
					
					renderRect(-menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz / 2),
					           menuSz - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff + 2 * slotsSz / 3),
					           -menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
					           menuSz - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff + 2 * slotsSz / 3 + slotsSz / 4),
					           BLOCK_COORD(D), BLOCK_COORD(12), BLOCK_COORD(D + 1), BLOCK_COORD(13));
					
					renderRect(-menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
					           menuSz - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff + 2 * slotsSz / 3),
					           -menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 2),
					           menuSz - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff + 2 * slotsSz / 3 + slotsSz / 4),
					           BLOCK_COORD(U), BLOCK_COORD(12), BLOCK_COORD(U + 1), BLOCK_COORD(13));
					
					GX_End();
				}
			}
		}
		
		/// Render quantity for hotbar
		for (int i = 0; i < 9; i++) {
			if (inventory.inventory[0][i].quantity > 0) {
				int qty = inventory.inventory[0][i].quantity;
				int D = 16 + (qty / 10), U = 16 + (qty % 10);
				
				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
				
				renderRect(-menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz / 2),
				           menuSz  - menuSz * 2 * (slotsHpad + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz / 2 + slotsSz / 4),
				           menuSz  - menuSz * 2 * (slotsHpad + 2 * slotsSz / 3 + slotsSz / 4),
				           BLOCK_COORD(D), BLOCK_COORD(12), BLOCK_COORD(D + 1), BLOCK_COORD(13));
				
				renderRect(-menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz / 2 + slotsSz / 4),
				           menuSz  - menuSz * 2 * (slotsHpad + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz / 2 + slotsSz / 2),
				           menuSz  - menuSz * 2 * (slotsHpad + 2 * slotsSz / 3 + slotsSz / 4),
				           BLOCK_COORD(U), BLOCK_COORD(12), BLOCK_COORD(U + 1), BLOCK_COORD(13));
				
				GX_End();
			}
		}
		
		/// Render quantity for inventory
		for (int i = 0; i < 27; i++) {
			if (inventory.inventory[1 + inventory.currentPage * 3 + i / 9][i % 9].quantity > 0) {
				int qty = inventory.inventory[1 + inventory.currentPage * 3 + i / 9][i % 9].quantity;
				int D = 16 + (qty / 10), U = 16 + (qty % 10);
				int X = i % 9, Y = i / 9;
				
				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
				
				renderRect(-menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz / 2),
				           menuSz  - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
				           menuSz  - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff + 2 * slotsSz / 3 + slotsSz / 4),
				           BLOCK_COORD(D), BLOCK_COORD(12), BLOCK_COORD(D + 1), BLOCK_COORD(13));
				
				renderRect(-menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
				           menuSz  - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 2),
				           menuSz  - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff + 2 * slotsSz / 3 + slotsSz / 4),
				           BLOCK_COORD(U), BLOCK_COORD(12), BLOCK_COORD(U + 1), BLOCK_COORD(13));
				
				GX_End();
			}
		}
	} else {
		
		/// Render hotbar
		GX_Begin(GX_QUADS, GX_VTXFMT0, 44); // Start drawing
		
		renderRect(-hotbarW / 2, hotbarY + hotbarH / 2, hotbarW / 2, hotbarY - hotbarH / 2, BLOCK_COORD(16), BLOCK_COORD(13), 0.85546875, 0.44921875);
		f32 X = -hotbarW / 2 - selectorPad + (f32)inventory.selectedSlot * selectorOff;
		renderRect(X, hotbarY + selectorSz / 2, X + selectorSz, hotbarY - selectorSz / 2, BLOCK_COORD(16), 0.44921875, 0.546875, 0.49609375);
		
		for (int i = 0; i < 9; i++) {
			auto& tx = blocData[inventory.inventory[0][i].item.type];
			renderRect(-hotbarW / 2 + itemXpad + (f32) i * itemXoff,
					   hotbarY + hotbarH / 2 - itemYpad,
					   -hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW,
					   hotbarY + hotbarH / 2 - itemYpad - itemH,
					   tx.x[2], tx.y[2], tx.x[2] + OFFSET, tx.y[2] + OFFSET);
		}
		
		GX_End();
		
		for (int i = 0; i < 9; i++) {
			if (inventory.inventory[0][i].quantity > 0) {
				int qty = inventory.inventory[0][i].quantity;
				int D = 16 + (qty / 10), U = 16 + (qty % 10);
				
				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);
				
				renderRect(-hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW / 2,
				           hotbarY + hotbarH / 2 - itemYpad - 2 * itemH / 3,
				           -hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW / 2 + itemW / 4,
				           hotbarY + hotbarH / 2 - itemYpad - 2 * itemH / 3 - itemH / 4,
				           BLOCK_COORD(D), BLOCK_COORD(12), BLOCK_COORD(D + 1), BLOCK_COORD(13));
				
				renderRect(-hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW / 2 + itemW / 4,
				           hotbarY + hotbarH / 2 - itemYpad - 2 * itemH / 3,
				           -hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW / 2 + itemW / 2,
				           hotbarY + hotbarH / 2 - itemYpad - 2 * itemH / 3 - itemH / 4,
				           BLOCK_COORD(U), BLOCK_COORD(12), BLOCK_COORD(U + 1), BLOCK_COORD(13));
				
				GX_End();
			}
		}
	}
	
	if (inventory.open && inventory.pickedItem.item.type != BlockType::Air) {
		/// Render picked item
		auto &tx = blocData[inventory.pickedItem.item.type];
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing
		
		f32 a = 0, b = 0;
		if (wiimote.wd->ir.valid) {
			a = wiimote.x * (1 - x);
			b = wiimote.y * (1 - y) / renderer.camera.ratio;
		}
	
		renderRect(a - 0.03f, b - 0.01f, a - 0.01f, b - 0.03f, tx.x[2], tx.y[2], tx.x[2] + OFFSET, tx.y[2] + OFFSET);
		
		GX_End();
	}
}


void Player::Underwater() {
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
	GX_Position3f32(-2, 1, 0);
	
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(3)); // Top left
	
	GX_Position3f32(2, 1, 0);
	
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(3)); // Top left
	
	GX_Position3f32(2, -1, 0);
	
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(16), BLOCK_COORD(4)); // Top left
	
	GX_Position3f32(-2, -1, 0);
	
	GX_Color1x8(WHITE);
	GX_TexCoord2f32(BLOCK_COORD(15), BLOCK_COORD(4)); // Top left
	GX_End();
}
