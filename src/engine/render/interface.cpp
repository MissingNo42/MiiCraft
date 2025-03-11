
#include "player.h"
#include "engine/render/block.h"
#include "engine/render/renderer.h"

static constexpr f32 menuSz = 0.65;
static constexpr f32 menuXMid = 0.2840909090909091;
static constexpr f32 menuYMid = 0.4431818181818182;

static constexpr f32 slotsXpad = 0.05113636363636364;
static constexpr f32 slotsYpad = 0.48295454545454547;

static constexpr f32 slotsXCpad = 0.17613636363636365; // craft
static constexpr f32 slotsYCpad = 0.10227272727272728; // craft

static constexpr f32 slotsXMpad = 0.5625; // craft (menu)
static constexpr f32 slotsYMpad = 0.10795454545454546; // craft (menu)

static constexpr f32 slotsXCRpad = 0.6875; // craft result
static constexpr f32 slotsYCRpad = 0.18181818181818182; // craft result

static constexpr f32 slotsXMRpad = 0.8806818181818182; // craft result (menu)
static constexpr f32 slotsYMRpad = 0.16477272727272727; // craft result (menu)

static constexpr f32 slotsXoff = 0.10227272727272728;
static constexpr f32 slotsYoff = 0.10227272727272728;

static constexpr f32 slotsHpad = 0.8125; // hotbar Y padding

static constexpr f32 slotsSz = 0.07954545454545454;
static constexpr f32 slotsCRSz = 1.5714285714285714f * slotsSz;

// hotbar
static constexpr f32 hotbarY = -0.65;
static constexpr f32 hotbarH = 0.13;
static constexpr f32 hotbarW = hotbarH * 8.272727272727273f;

static constexpr f32 selectorSz = hotbarH * 1.0909090909090908f;
static constexpr f32 selectorOff = 0.10989010989010989f * hotbarW;
static constexpr f32 selectorPad = 0.001953125;

static constexpr f32 itemW = 0.07692307692307693f * hotbarW;
static constexpr f32 itemH = 0.6363636363636364f * hotbarH;

static constexpr f32 itemXpad = 0.02197802197802198f * hotbarW;
static constexpr f32 itemYpad = 0.18181818181818182f * hotbarH;
static constexpr f32 itemXoff = 0.10989010989010989f * hotbarW;


inline bool Player::renderBlockIcon(f32 x1, f32 y1, f32 x2, f32 y2, BlockType block) const {

	if (block) {
		auto& tx = blockData[block];
		Renderer::renderRect(x1, y1, x2, y2,
				   tx.tc[BlockFace::North],
				   tx.tc[BlockFace::North] + 17,
				   tx.tc[BlockFace::North] + 1,
				   tx.tc[BlockFace::North] + 18);
	} else { // render "void rect" to avoid GX crash (declared rendered vertices)
		Renderer::renderRect(0, 0, 0, 0, 0, 0, 0, 0);
	}

	f32 a = 0, b = 0;
	if (wiimote.wd->ir.valid) {
		a = wiimote.x * (1 - cursorWidth);
		b = wiimote.y * (1 - cursorHeight) / renderer.camera.ratio;
	}

	return x1 <= a && a <= x2 && y2 <= b && b <= y1;
}

void Player::renderCursor(f32 x, f32 y) const {
	s32 W = Renderer::rmode->fbWidth, H = Renderer::rmode->efbHeight,
	originX = renderer.camera.format & Camera::SplitLeft ? 0: W >> 1,
	originY = renderer.camera.format & Camera::SplitTop ? 0: H >> 1;
	f32 width =  renderer.camera.format & Camera::SplitRight && renderer.camera.format & Camera::SplitLeft ? (f32)W: (f32)W / 2.f;
	f32 height = renderer.camera.format & Camera::SplitBottom && renderer.camera.format & Camera::SplitTop ? (f32)H: (f32)H / 2.f;

	s32 limitX = originX + (s32)(width);
	s32 limitY = originY + (s32)(height);
	printf(">>>>>>>>>> %f\r", renderer.camera.ratio);
	s32 w = (s32)(cursorWidth * width / renderer.camera.ratio), h = (s32)(cursorHeight * height);

	s32 ox = originX + (s32)(x * width);
	s32 oy = originY + (s32)(y * height);

	for (s32 r = 0; r <= 1; r++) { // render the horizontal line

		if (oy + r < 0 || oy + r >= (s32)limitY) continue;
		for (s32 sx = ox - w, sX = ox + w; sx <= sX; sx++) {

			if (sx < 0 || sx >= (s32)limitX) continue;
			GXColor c;
			GX_PeekARGB(sx, oy + r, &c);
			c.r ^= 0xff;
			c.g ^= 0xff;
			c.b ^= 0xff;
			c.a = 0xff;
			GX_PokeARGB(sx, oy + r, c);
		}
	}

	for (s32 r = 0; r <= 1; r++) { // render the vertical line

		if (ox + r < 0 || ox + r >= (s32)limitX) continue;
		for (s32 sy = oy - h, sY = oy + h; sy <= sY; sy++) {

			if (sy == oy || sy == oy + 1) continue;
			if (sy < 0 || sy >= (s32)limitY) continue;
			GXColor c;
			GX_PeekARGB(ox + r, sy, &c);
			c.r ^= 0xff;
			c.g ^= 0xff;
			c.b ^= 0xff;
			c.a = 0xff;
			GX_PokeARGB(ox + r, sy, c);
		}
	}
}

void Player::renderCursor() const {

	if (!inventory.open && !wiimoteFocus) {
		renderCursor(0.5f, 0.5f);
	}

	f32 a = 0, b = 0;
	if (wiimote.wd->ir.valid) {
		a = wiimote.x * (1 - cursorWidth);
		b = wiimote.y * (cursorHeight - 1);
	}

	renderCursor(a / 2.f + 0.5f, b / 2.f + 0.5f);
}

void Player::renderInventory() {
	isValidCursor = false;

	if (inventory.open) {

		/// Render background menu

		if (inventory.craftOpen) {
			GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing
			Renderer::renderRect(-menuSz, menuSz, menuSz, -menuSz,
			           TXCOORD(16, 16),
			           static_cast<u16>(TextureIndex::CRAFT_MENU_RT),
			           static_cast<u16>(TextureIndex::CRAFT_MENU_LB),
			           static_cast<u16>(TextureIndex::CRAFT_MENU_RB));
		} else {
			GX_Begin(GX_QUADS, GX_VTXFMT0, 8); // Start drawing
			Renderer::renderRect(-menuSz, menuSz, menuSz, -menuSz,
			           TXCOORD(16, 0),
			           static_cast<u16>(TextureIndex::MENU_RT),
			           TXCOORD(16, 11),
			           static_cast<u16>(TextureIndex::MENU_RB));
			f32 fc = 0.1;

			Renderer::renderRect(-menuSz + menuSz * 2 * menuXMid - fc,
			           menuSz - menuSz * 2 * menuYMid + 2.0f * fc * 11.0f / 5.0f,
			           -menuSz + menuSz * 2 * menuXMid + fc,
			           menuSz - menuSz * 2 * menuYMid,
			           static_cast<u16>(TextureIndex::MENU_MII_LT),
			           static_cast<u16>(TextureIndex::MENU_MII_RT),
			           static_cast<u16>(TextureIndex::MENU_MII_LB),
			           static_cast<u16>(TextureIndex::MENU_MII_RB));
		}
		GX_End();

		/// Render slots
		GX_Begin(GX_QUADS, GX_VTXFMT0, 108); // Start drawing
		for (int i = 0; i < 27; i++) {
			int X = i % 9, Y = i / 9;
			if(renderBlockIcon(-menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff),
									  menuSz - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff),
									  -menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz),
									  menuSz - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff + slotsSz),
									  inventory.inventory[1 + inventory.currentPage * 3 + Y][X].item.type))
				selectedSlot = i, craftSlot = false, isValidCursor = true;
		}
		GX_End();

		/// Render hotbar slots
		GX_Begin(GX_QUADS, GX_VTXFMT0, 36);
		for (int i = 0; i < 9; i++) {
			if(renderBlockIcon(-menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff),
			                     menuSz - menuSz * 2 * slotsHpad,
			                     -menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz),
			                     menuSz - menuSz * 2 * (slotsHpad + slotsSz),
			                     inventory.inventory[0][i].item.type))
				selectedSlot = 27 + i, craftSlot = false, isValidCursor = true;
		}

		GX_End();

		/// Render craft slots
		GX_Begin(GX_QUADS, GX_VTXFMT0, 36);

		if (inventory.craftOpen) {
			for (int i = 0; i < 9; i++) {
				int X = i % 3, Y = i / 3;
				if(renderBlockIcon(-menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff),
				                     menuSz - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff),
				                     -menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz),
				                     menuSz - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff + slotsSz),
				                     inventory.craftSlots[i].item.type))
					selectedSlot = i, craftSlot = true, isValidCursor = true;
			}
		} else {
			for (int i = 0; i < 9; i++) {
				int X = i % 3, Y = i / 3;
				if (renderBlockIcon(-menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff),
				                     menuSz - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff),
				                     -menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz),
				                     menuSz - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff + slotsSz),
				                     inventory.craftSlots[i].item.type) && X != 2 && Y != 2)
					selectedSlot = i, craftSlot = true, isValidCursor = true;
			}
		}

		GX_End();

		/// Render craft result slot
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
		if (inventory.craftOpen ?
			renderBlockIcon(-menuSz + menuSz * 2 * (slotsXCRpad),
			                     menuSz - menuSz * 2 * (slotsYCRpad),
			                     -menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz),
			                     menuSz - menuSz * 2 * (slotsYCRpad + slotsCRSz),
			                     inventory.craftSlots[9].item.type)
								 :
			renderBlockIcon(-menuSz + menuSz * 2 * (slotsXMRpad),
			                     menuSz - menuSz * 2 * (slotsYMRpad),
			                     -menuSz + menuSz * 2 * (slotsXMRpad + slotsSz),
			                     menuSz - menuSz * 2 * (slotsYMRpad + slotsSz),
			                     inventory.craftSlots[9].item.type))
			selectedSlot = 9, craftSlot = true, isValidCursor = true;
		GX_End();

		/// Render quantity for result
		if (inventory.craftSlots[9].quantity > 0) {

			GX_Begin(GX_QUADS, GX_VTXFMT0, 8);

			if (inventory.craftOpen) {
				Renderer::renderValue(-menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz / 2),
				           menuSz - menuSz * 2 * (slotsYCRpad + slotsCRSz / 2),
				           -menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz / 2 + slotsCRSz / 4),
				           menuSz - menuSz * 2 * (slotsYCRpad + slotsCRSz / 2 + slotsCRSz / 4),
						   -menuSz + menuSz * 2 * (slotsXCRpad + slotsCRSz / 2 + slotsCRSz / 2),
								   inventory.craftSlots[9].quantity);
			} else {
				Renderer::renderValue(-menuSz + menuSz * 2 * (slotsXMRpad + slotsSz / 2),
				           menuSz - menuSz * 2 * (slotsYMRpad + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXMRpad + slotsSz / 2 + slotsSz / 4),
				           menuSz - menuSz * 2 * (slotsYMRpad + 2 * slotsSz / 3 + slotsSz / 4),
				           -menuSz + menuSz * 2 * (slotsXMRpad + slotsSz / 2 + slotsSz / 2),
						   inventory.craftSlots[9].quantity);
			}

			GX_End();
		}

		/// Render quantity for craft
		for (int i = 0; i < 9; i++) {

			if (inventory.craftSlots[i].quantity > 0) {
				int X = i % 3, Y = i / 3;

				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);

				if (inventory.craftOpen) {
					Renderer::renderValue(-menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz / 2),
					           menuSz - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff + slotsSz / 2),
					           -menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
					           menuSz - menuSz * 2 * (slotsYCpad + (f32) Y * slotsYoff + slotsSz / 2 + slotsSz / 4),
					           -menuSz + menuSz * 2 * (slotsXCpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 2),
							   inventory.craftSlots[i].quantity);
				} else {
					Renderer::renderValue(-menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz / 2),
					           menuSz - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff + 2 * slotsSz / 3),
					           -menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
					           menuSz - menuSz * 2 * (slotsYMpad + (f32) Y * slotsYoff + 2 * slotsSz / 3 + slotsSz / 4),
					           -menuSz + menuSz * 2 * (slotsXMpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 2),
							   inventory.craftSlots[i].quantity);
				}

				GX_End();
			}
		}

		/// Render quantity for hotbar
		for (int i = 0; i < 9; i++) {
			if (inventory.inventory[0][i].quantity > 0) {

				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);

				Renderer::renderValue(-menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz / 2),
				           menuSz - menuSz * 2 * (slotsHpad + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz / 2 + slotsSz / 4),
				           menuSz - menuSz * 2 * (slotsHpad + 2 * slotsSz / 3 + slotsSz / 4),
				           -menuSz + menuSz * 2 * (slotsXpad + (f32) i * slotsXoff + slotsSz / 2 + slotsSz / 2),
						   inventory.inventory[0][i].quantity);

				GX_End();
			}
		}

		/// Render quantity for inventory
		for (int i = 0; i < 27; i++) {
			int X = i % 9, Y = i / 9, qty = inventory.inventory[1 + inventory.currentPage * 3 + Y][X].quantity;

			if (qty) {

				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);

				Renderer::renderValue(-menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz / 2),
				           menuSz - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff + 2 * slotsSz / 3),
				           -menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 4),
				           menuSz - menuSz * 2 * (slotsYpad + (f32) Y * slotsYoff + 2 * slotsSz / 3 + slotsSz / 4),
				           -menuSz + menuSz * 2 * (slotsXpad + (f32) X * slotsXoff + slotsSz / 2 + slotsSz / 2),
						   qty);

				GX_End();
			}
		}
	} else {

		/// Render hotbar
		GX_Begin(GX_QUADS, GX_VTXFMT0, 44); // Start drawing

		Renderer::renderRect(-hotbarW / 2, hotbarY + hotbarH / 2, hotbarW / 2, hotbarY - hotbarH / 2,
		           TXCOORD(16, 13),
		           static_cast<u16>(TextureIndex::HOTBAR_RT),
		           static_cast<u16>(TextureIndex::HOTBAR_LB),
		           static_cast<u16>(TextureIndex::HOTBAR_RB));

		f32 X = -hotbarW / 2 - selectorPad + (f32)inventory.selectedSlot * selectorOff;
		Renderer::renderRect(X, hotbarY + selectorSz / 2, X + selectorSz, hotbarY - selectorSz / 2,
		           static_cast<u16>(TextureIndex::HOTBAR_SELECTOR_LT),
		           static_cast<u16>(TextureIndex::HOTBAR_SELECTOR_RT),
		           static_cast<u16>(TextureIndex::HOTBAR_SELECTOR_LB),
		           static_cast<u16>(TextureIndex::HOTBAR_SELECTOR_RB));

		for (int i = 0; i < 9; i++) {
			(void)renderBlockIcon(-hotbarW / 2 + itemXpad + (f32) i * itemXoff,
			           hotbarY + hotbarH / 2 - itemYpad,
			           -hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW,
			           hotbarY + hotbarH / 2 - itemYpad - itemH,
			           inventory.inventory[0][i].item.type);
		}

		GX_End();

		for (int i = 0; i < 9; i++) {
			int qty = inventory.inventory[0][i].quantity;

			if (qty) {
				GX_Begin(GX_QUADS, GX_VTXFMT0, 8);

				Renderer::renderValue(-hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW / 2,
				           hotbarY + hotbarH / 2 - itemYpad - 2 * itemH / 3,
				           -hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW / 2 + itemW / 4,
				           hotbarY + hotbarH / 2 - itemYpad - 2 * itemH / 3 - itemH / 4,
				           -hotbarW / 2 + itemXpad + (f32) i * itemXoff + itemW / 2 + itemW / 2,
						   qty);

				GX_End();
			}
		}
	}

	if (inventory.open && inventory.pickedItem.item.type) {

		f32 a = 0, b = 0;
		if (wiimote.wd->ir.valid) {
			a = wiimote.x * (1 - cursorWidth);
			b = wiimote.y * (1 - cursorHeight) / renderer.camera.ratio;
		}

		GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Start drawing

		(void)renderBlockIcon(a - 0.03f, b - 0.01f, a - 0.01f, b - 0.03f, inventory.pickedItem.item.type);

		GX_End();
	}
}



