//
// Created by Natha on 13/02/2024.
//

#include <cmath>
#include <algorithm>
#include "player.h"
#include "render/bloc.h"
#include "wiimote.h"
#include "world/world.h"
#include "utils/matrix.h"

Player::Player(int chan) : Player(0, 0, 0, chan) {}

Player::Player(f32 x, f32 y, f32 z, int chan) : wiimote(chan) {
	renderer.camera.pos.x = x;
	renderer.camera.pos.y = y;
	renderer.camera.pos.z = z;
	
}

f32 Player::getFocusedBlockDistance() const {
	f32 dx = (f32) focusedBlockPos.x - renderer.camera.pos.x;
	f32 dy = (f32) focusedBlockPos.y - renderer.camera.pos.y;
	f32 dz = (f32) focusedBlockPos.z - renderer.camera.pos.z;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

bool Player::getFocusedBlock() {
	BlockType type = BlockType::Air;
	BlockCoord pos(0, 0, 0);
	
	if (wiimoteFocus && !cameraLocked) { // avoid lock glitching
		guVector cpos = renderer.camera.pos;
		guVector dir = renderer.camera.look;
		guVector right, axis;
		
		f32 uA = renderer.camera.fovx * wiimote.x / 2.f, vA = renderer.camera.fovy * -wiimote.y / 2.f;
		f32 uD = uA * 180.f / (f32) M_PI, vD = vA * 180.f / (f32) M_PI;
		
		guVecCross(&renderer.camera.up, &dir, &right); // Calculate the right axis (cross product of look-at and up)
		VecRotAxis(&dir, right, vD); // rotV
		
		guVecCross(&right, &dir, &axis); // Calculate the right axis (cross product of look-at and up)
		VecRotAxis(&dir, axis, uD); // rotH (relative up axis)
		
		guVecNormalize(&dir);
		
		f32 xs = dir.x < 0 ? 0 : 1;
		f32 ys = dir.y < 0 ? 0 : 1;
		f32 zs = dir.z < 0 ? 0 : 1;
		f32 dist = 0;
		
		do {
			f32 xq = floorf(cpos.x + xs); // get the coord of the nearest block
			f32 yq = floorf(cpos.y + ys); // get the coord of the nearest block
			f32 zq = floorf(cpos.z + zs); // get the coord of the nearest block
			
			const f32 ZERO = -0.0f;
			if (*(s32 *)&xq == *(s32 *)&ZERO) xq = -1.0f; // apply correction : floorf(-0) == -0 -> -1, test through int for safety
			if (*(s32 *)&yq == *(s32 *)&ZERO) yq = -1.0f; // apply correction : floorf(-0) == -0 -> -1, test through int for safety
			if (*(s32 *)&zq == *(s32 *)&ZERO) zq = -1.0f; // apply correction : floorf(-0) == -0 -> -1, test through int for safety
			
			if (dir.x < 0) xq = nextafterf(xq, -INFINITY); // for negative vector, minimal shift to negative -> no impact on test but shift on floor
			if (dir.y < 0) yq = nextafterf(yq, -INFINITY); // for negative vector, minimal shift to negative -> no impact on test but shift on floor
			if (dir.z < 0) zq = nextafterf(zq, -INFINITY); // for negative vector, minimal shift to negative -> no impact on test but shift on floor
			
			f32 dx = (xq - cpos.x) / dir.x; // get the delta, always > 0
			f32 dy = (yq - cpos.y) / dir.y; // get the delta, always > 0
			f32 dz = (zq - cpos.z) / dir.z; // get the delta, always > 0
			
			if (dx < dy && dx < dz) {
				dist += dx; // works since dir is normalized
				cpos.x = xq;
				cpos.y += dx * dir.y;
				cpos.z += dx * dir.z;
				focusedFace = dir.x < 0 ? BLOC_FACE_RIGHT : BLOC_FACE_LEFT;
			} else if (dy < dz) {
				dist += dy; // works since dir is normalized
				cpos.x += dy * dir.x;
				cpos.y = yq;
				cpos.z += dy * dir.z;
				focusedFace = dir.y < 0 ? BLOC_FACE_TOP : BLOC_FACE_BOTTOM;
			} else {
				dist += dz; // works since dir is normalized
				cpos.x += dz * dir.x;
				cpos.y += dz * dir.y;
				cpos.z = zq;
				focusedFace = dir.z < 0 ? BLOC_FACE_FRONT : BLOC_FACE_BACK;
			}
			
			pos = BlockCoord((int)( floorf(cpos.x) + 1), (int) (floorf(cpos.y) + 1), (int) (floorf(cpos.z) + 1)); // apply negative render correction
			type = World::getBlockAt(pos);
			
			if (type > BlockType::Air && type != Water) {
				guVector dt = {(f32)pos.x - 0.5f, (f32)pos.y - 0.5f, (f32)pos.z - 0.5f};
				guVecSub(&dt, &renderer.camera.pos, &dt);
				
				if (guVecDotProduct(&dt, &dt) < 25) { // check the dist with block center
					previousFocusedBlockPos = focusedBlockPos;
					focusedBlockLook = {cpos.x + 1, cpos.y + 1, cpos.z + 1};
					focusedBlockPos = pos;
					focusedBlockType = type;
					return true;
				}
				return false;
			}
		} while (dist < 6); // 6 is the max distance to check : corner or back faces
		
	} else {
		f32 distance = 0,
				x = renderer.camera.pos.x + 1,
				y = renderer.camera.pos.y + 1,
				z = renderer.camera.pos.z + 1;
		while ((type <= BlockType::Air || type == Water) && distance <= 5) {
			x += renderer.camera.look.x / 200;
			y += renderer.camera.look.y / 200;
			z += renderer.camera.look.z / 200;
			distance += 0.005;
			pos = BlockCoord((int) floorf(x), (int) floorf(y), (int) floorf(z));
			type = World::getBlockAt(pos);
		}
		if (type > BlockType::Air && type != Water) {
			previousFocusedBlockPos = focusedBlockPos;
			focusedBlockLook = {x, y, z};
			focusedBlockPos = pos;
			focusedBlockType = type;
			focusedFace = getFocusedFace();
			return true;
		}
	}
	return false;
}


guVector Player::negateVector(const guVector &v) {
	return {-v.x, -v.y, -v.z};
}


void Player::goUp(float velocity, bool collision) {
	f32 size = sneak ? 0.70005 : 0.40005;
	f32 x = renderer.camera.pos.x + 1;
	f32 y = renderer.camera.pos.y + 1;
	f32 z = renderer.camera.pos.z + 1;
	if (collision) {
		if (World::getBlockAt({(int) floorf(x + 0.3f), (int) ceilf(y + size + velocity / 10 - 1), (int) floorf(z + 0.3f)}) <=
		    BlockType::Air
		    &&
		    World::getBlockAt({(int) floorf(x - 0.3f), (int) ceilf(y + size + velocity / 10 - 1), (int) floorf(z + 0.3f)}) <=
		    BlockType::Air
		    &&
		    World::getBlockAt({(int) floorf(x + 0.3f), (int) ceilf(y + size + velocity / 10 - 1), (int) floorf(z - 0.3f)}) <=
		    BlockType::Air
		    &&
		    World::getBlockAt({(int) floorf(x - 0.3f), (int) ceilf(y + size + velocity / 10 - 1), (int) floorf(z - 0.3f)}) <=
		    BlockType::Air) {
			if (!cameraLocked) {
				renderer.camera.pos.y += velocity / 10;
			} else {
				renderer.camera.pos.y += velocity / 10;
				renderer.camera.look.y -= velocity / 10;
			}
		} else if (World::getBlockAt({(int) floorf(x + 0.3f), (int) floorf(y - size), (int) floorf(z + 0.3f)}) ==
		           BlockType::Water
		           || World::getBlockAt({(int) floorf(x - 0.3f), (int) floorf(y - size), (int) floorf(z + 0.3f)}) ==
		              BlockType::Water
		           || World::getBlockAt({(int) floorf(x + 0.3f), (int) floorf(y - size), (int) floorf(z - 0.3f)}) ==
		              BlockType::Water
		           || World::getBlockAt({(int) floorf(x - 0.3f), (int) floorf(y - size), (int) floorf(z - 0.3f)}) ==
		              BlockType::Water) {
			if (!cameraLocked) {
				renderer.camera.pos.y += velocity / 40;
			} else {
				renderer.camera.pos.y += velocity / 10;
				renderer.camera.look.y -= velocity / 10;
			}
		} else {
			renderer.camera.pos.y = (f32) ceilf(y + 0.40005f + velocity / 10 - 1) - 1.1005f;
			Velocity = 0.6f;
		}
		
	} else if (!cameraLocked) {
		renderer.camera.pos.y += velocity / 10;
	} else {
		renderer.camera.pos.y += velocity / 10;
		renderer.camera.look.y -= velocity / 10;
	}
	guVecNormalize(&renderer.camera.look);
}


void Player::goDown(float velocity, bool collision) {
	f32 size = sneak ? 1.3 : 1.6;
	f32 x = renderer.camera.pos.x + 1;
	f32 y = renderer.camera.pos.y + 1;
	f32 z = renderer.camera.pos.z + 1;
	if (collision) {
		if ((World::getBlockAt({(int) floorf(x + 0.3), (int) floorf(y - size - velocity / 10), (int) floorf(z + 0.3)}) <=
		     BlockType::Air
		     &&
		     World::getBlockAt({(int) floorf(x - 0.3), (int) floorf(y - size - velocity / 10), (int) floorf(z + 0.3)}) <=
		     BlockType::Air
		     &&
		     World::getBlockAt({(int) floorf(x + 0.3), (int) floorf(y - size - velocity / 10), (int) floorf(z - 0.3)}) <=
		     BlockType::Air
		     &&
		     World::getBlockAt({(int) floorf(x - 0.3), (int) floorf(y - size - velocity / 10), (int) floorf(z - 0.3)}) <=
		     BlockType::Air)
		    ||
		    (World::getBlockAt({(int) floorf(x + 0.3), (int) floorf(y - size - velocity / 10), (int) floorf(z + 0.3)}) ==
		     BlockType::Water
		     &&
		     World::getBlockAt({(int) floorf(x - 0.3), (int) floorf(y - size - velocity / 10), (int) floorf(z + 0.3)}) ==
		     BlockType::Water
		     &&
		     World::getBlockAt({(int) floorf(x + 0.3), (int) floorf(y - size - velocity / 10), (int) floorf(z - 0.3)}) ==
		     BlockType::Water
		     &&
		     World::getBlockAt({(int) floorf(x - 0.3), (int) floorf(y - size - velocity / 10), (int) floorf(z - 0.3)}) ==
		     BlockType::Water)) {
			if (!cameraLocked) {
				renderer.camera.pos.y -= velocity / 10;
			} else {
				renderer.camera.pos.y -= velocity / 10;
				renderer.camera.look.y += velocity / 10;
			}
		} else {
			renderer.camera.pos.y = floorf(y - size - velocity / 10) + size + 0.00001f;
		}
	} else {
		if (!cameraLocked) {
			renderer.camera.pos.y -= velocity / 10;
		} else {
			renderer.camera.pos.y -= velocity / 10;
			renderer.camera.look.y += velocity / 10;
		}
	}
	guVecNormalize(&renderer.camera.look);
}

void Player::Jump() {
	if (!isJumping) {
		isJumping = true;
		Velocity = -1.65;
	}
}


void Player::setPos(f32 x, f32 y, f32 z) {
	renderer.camera.pos.x = x;
	renderer.camera.pos.y = y;
	renderer.camera.pos.z = z;
}


/**
 * @brief get the rotation horizontal speed of the camera in rad/frame unit
 * @param dx the distance in x axis from center of the screen in [-1, 1]
 * @param dy the distance in y axis from center of the screen in [-1, 1]
 * */
static f32 rotationHSpeed(f32 dx, f32 dy) {
	f32 x = std::abs(dx), y = std::abs(dy);
	/// speed datamined from MP2 (MPT version)
	f32 speed = 2.5f * (f32) std::pow(x, 2.9); // 0.04363319 radians / 2.5 degrees coeff
	f32 factor = 0.757f * (f32) std::pow(x, 0.0267); // reduce speed when looking up or down
	speed *= std::pow(factor, y); // vertical attenuation of speed
	return dx > 0 ? -speed : speed;
}


void Player::handleRotation() {
	if (wiimote.wd->ir.valid) {
		renderer.camera.rotateH(rotationHSpeed(wiimote.x, -wiimote.y));
		
		/// auto re-center for Y axis, from MP2 (MPT version)
		f32 targetV = -wiimote.y * Camera::limitV;
		f32 deltaV = targetV - renderer.camera.angleV;
		if (deltaV < 0) renderer.camera.rotateV(std::min(deltaV / (60.f), std::max(deltaV, -0.05f)));
		else renderer.camera.rotateV(std::max(deltaV / (60.f), std::min(deltaV, 0.05f)));
	}
}


void Player::destroyBlock() {
	if (!(focusedBlockPos == previousFocusedBlockPos))
		breakingState = 0;
	
	destroying = false;
	
	if (breakingState < 50) {
		destroying = true;
	} else {
		BlockType breaked = World::getBlockAt(focusedBlockPos);
		World::setBlockAt(focusedBlockPos, BlockType::Air);
		if (!creative)
			inventory.addItem(breaked, 1);
		if (focusedBlockPos == lockedBlockPos) {
			if ((focusing = getFocusedBlock())) lockedBlockPos = focusedBlockPos;
			else cameraLocked = false;
		}
		breakingState = 0;
	}
	breakingState++;
}


void Player::placeBlock() {
	//if (inventory.inventory[0][inventory.selectedSlot].item.type )  // IF NOT SOLID BLOCK
	//    return;
	if (placeDelay < 10) return;
	
	BlockCoord pos = focusedBlockPos;
	
	if (focusedBlockType > BlockType::Air) {
		switch (focusedFace) {
			case BLOC_FACE_LEFT: pos.x--;
				break;
			case BLOC_FACE_RIGHT: pos.x++;
				break;
			case BLOC_FACE_BOTTOM: pos.y--;
				break;
			case BLOC_FACE_TOP: pos.y++;
				break;
			case BLOC_FACE_BACK: pos.z--;
				break;
			case BLOC_FACE_FRONT: pos.z++;
				break;
		}
		
		if (World::getBlockAt(pos) <= BlockType::Air
		    && (
				    (pos.x != (int) floorf(renderer.camera.pos.x + 1.3f) &&
				     pos.x != (int) floorf(renderer.camera.pos.x + 0.7f))
				    || (pos.y != (int) floorf(renderer.camera.pos.y + 1.0f) && pos.y != (int) floorf(renderer.camera.pos.y))
				    || (pos.z != (int) floorf(renderer.camera.pos.z + 1.3f) &&
				        pos.z != (int) floorf(renderer.camera.pos.z + 0.7f)))) {
			World::setBlockAt(pos, inventory.inventory[0][inventory.selectedSlot].item.type);
			if (!creative) {
				inventory.inventory[0][inventory.selectedSlot].quantity--;
				if (inventory.inventory[0][inventory.selectedSlot].quantity == 0)
					inventory.inventory[0][inventory.selectedSlot].item = Item(BlockType::Air);
			}
		}
	}
	placeDelay = 0;
}

int Player::getFocusedFace() const {
	if (focusedBlockType != BlockType::Air) {
		f32 deltaX = (f32) std::fabs((focusedBlockLook.x - round(focusedBlockLook.x)));
		f32 deltaY = (f32) std::fabs((focusedBlockLook.y - round(focusedBlockLook.y)));
		f32 deltaZ = (f32) std::fabs((focusedBlockLook.z - round(focusedBlockLook.z)));
		
		std::vector<f32> f = {deltaX, deltaY, deltaZ};
		auto min = std::min_element(std::begin(f), std::end(f));
		if (*min == f[0]) {
			if (renderer.camera.pos.x + 1 <= round(focusedBlockLook.x)) return BLOC_FACE_LEFT;
			else return BLOC_FACE_RIGHT;
		} else if (*min == f[1]) {
			if (renderer.camera.pos.y + 1 <= round(focusedBlockLook.y)) return BLOC_FACE_BOTTOM;
			else return BLOC_FACE_TOP;
		} else {
			if (renderer.camera.pos.z + 1 <= round(focusedBlockLook.z)) return BLOC_FACE_BACK;
			else return BLOC_FACE_FRONT;
		}
	}
	return -1;
}


guVector Player::coordToGuVector(BlockCoord coord) {
	return {(f32) coord.x, (f32) coord.y, (f32) coord.z};
}


void Player::move(joystick_t sticks) {
	guVector normalizedLook = renderer.camera.look;
	
	f32 stick_x = (f32) sticks.pos.x - (f32) sticks.center.x;
	f32 stick_y = (f32) sticks.pos.y - (f32) sticks.center.y;
	f32 maxValueX = ((f32) sticks.max.x - (f32) sticks.min.x) / 2;
	f32 maxValueY = ((f32) sticks.max.y - (f32) sticks.min.y) / 2;
	
	guVector directionMove;
	guVector move = {0, 0, 0};
	
	if (stick_x > 5) {
		guVecCross(&normalizedLook, &renderer.camera.up, &directionMove);
		move.x += stick_x * directionMove.x / maxValueX;
		move.z += stick_x * directionMove.z / maxValueX;
		
	} else if (stick_x < -5) {
		guVecCross(&normalizedLook, &renderer.camera.up, &directionMove);
		directionMove = negateVector(directionMove);
		move.x += -stick_x * directionMove.x / maxValueX;
		move.z += -stick_x * directionMove.z / maxValueX;
	}
	
	if (stick_y > 5) {
		move.x += stick_y * normalizedLook.x / maxValueY;
		move.z += stick_y * normalizedLook.z / maxValueY;
		
	} else if (stick_y < -5) {
		directionMove = negateVector(normalizedLook);
		move.x += -stick_y * directionMove.x / maxValueY;
		move.z += -stick_y * directionMove.z / maxValueY;
	}
	
	if (move.x != 0 || move.z != 0)
		guVecNormalize(&move);
	
	if (sneak) {
		move.x = move.x * 2.f / 60;
		move.z = move.z * 2.f / 60;
	} else if (sprint) {
		move.x = move.x * 5.6f / 60;
		move.z = move.z * 5.6f / 60;
	} else {
		move.x = move.x * 4.3f / 60;
		move.z = move.z * 4.3f / 60;
	}
	
	f32 offsetX = move.x < 0 ? -0.3: 0.3,
	    offsetZ = move.z < 0 ? -0.3: 0.3;
	
	f32 camX = renderer.camera.pos.x;
	f32 camY = sneak ? renderer.camera.pos.y + 0.3f : renderer.camera.pos.y;
	f32 camZ = renderer.camera.pos.z;
	
	if ((World::getBlockAt({(int) floorf(offsetX + camX + 1 + move.x), (int) (camY - 0.5f),
	                        (int) floorf(camZ + 1.3)}) <= BlockType::Air
	     && World::getBlockAt({(int) floorf(offsetX + camX + 1 + move.x), (int) (camY - 0.5f),
	                           (int) floorf(camZ + 0.7)}) <= BlockType::Air
	     && World::getBlockAt({(int) floorf(offsetX + camX + 1 + move.x), (int) (camY + 0.5f),
	                           (int) floorf(camZ + 1.3)}) <= BlockType::Air
	     && World::getBlockAt({(int) floorf(offsetX + camX + 1 + move.x), (int) (camY + 0.5f),
	                           (int) floorf(camZ + 0.7)}) <= BlockType::Air) ||
	    (World::getBlockAt({(int) floorf(offsetX + camX + 1 + move.x), (int) (camY - 0.5f),
	                        (int) floorf(camZ + 1.3)}) == BlockType::Water
	     && World::getBlockAt({(int) floorf(offsetX + camX + 1 + move.x), (int) (camY - 0.5f),
	                           (int) floorf(camZ + 0.7)}) == BlockType::Water
	     && World::getBlockAt({(int) floorf(offsetX + camX + 1 + move.x), (int) (camY + 0.5f),
	                           (int) floorf(camZ + 1.3)}) == BlockType::Water
	     && World::getBlockAt({(int) floorf(offsetX + camX + 1 + move.x), (int) (camY + 0.5f),
	                           (int) floorf(camZ + 0.7)}) == BlockType::Water)) {
		if (sneak && !isJumping) {
			if (World::getBlockAt({(int) floorf(camX + 1.3 + move.x), (int) (camY - 1), (int) floorf(camZ + 1.3)}) >
			    BlockType::Air
			    ||
			    World::getBlockAt({(int) floorf(camX + 0.7 + move.x), (int) (camY - 1), (int) floorf(camZ + 1.3)}) >
			    BlockType::Air
			    ||
			    World::getBlockAt({(int) floorf(camX + 1.3 + move.x), (int) (camY - 1), (int) floorf(camZ + 0.7)}) >
			    BlockType::Air
			    ||
			    World::getBlockAt({(int) floorf(camX + 0.7 + move.x), (int) (camY - 1), (int) floorf(camZ + 0.7)}) >
			    BlockType::Air) {
				renderer.camera.pos.x += move.x;
			}
		} else {
			if (!cameraLocked)
				renderer.camera.pos.x += move.x;
			else {
				renderer.camera.pos.x += move.x;
				renderer.camera.look.x -= move.x;
			}
		}
	}
	if ((World::getBlockAt({(int) floorf(camX + 1.3), (int) (camY - 0.5),
	                        (int) floorf(offsetZ + camZ + 1 + move.z)}) <= BlockType::Air
	     && World::getBlockAt({(int) floorf(camX + 0.7), (int) (camY - 0.5),
	                           (int) floorf(offsetZ + camZ + 1 + move.z)}) <= BlockType::Air
	     && World::getBlockAt({(int) floorf(camX + 1.3), (int) (camY + 0.5f),
	                           (int) floorf(offsetZ + camZ + 1 + move.z)}) <= BlockType::Air
	     && World::getBlockAt({(int) floorf(camX + 0.7), (int) (camY + 0.5f),
	                           (int) floorf(offsetZ + camZ + 1 + move.z)}) <= BlockType::Air) ||
	    (World::getBlockAt({(int) floorf(camX + 1.3), (int) (camY - 0.5),
	                        (int) floorf(offsetZ + camZ + 1 + move.z)}) == BlockType::Water
	     && World::getBlockAt({(int) floorf(camX + 0.7), (int) (camY - 0.5),
	                           (int) floorf(offsetZ + camZ + 1 + move.z)}) == BlockType::Water
	     && World::getBlockAt({(int) floorf(camX + 1.3), (int) (camY + 0.5f),
	                           (int) floorf(offsetZ + camZ + 1 + move.z)}) == BlockType::Water
	     && World::getBlockAt({(int) floorf(camX + 0.7), (int) (camY + 0.5f),
	                           (int) floorf(offsetZ + camZ + 1 + move.z)}) == BlockType::Water)
			) {
		if (sneak && !isJumping) {
			if (World::getBlockAt({(int) floorf(camX + 0.7), (int) (camY - 1), (int) floorf(camZ + 1.3 + move.z)}) >
			    BlockType::Air
			    ||
			    World::getBlockAt({(int) floorf(camX + 0.7), (int) (camY - 1), (int) floorf(camZ + 0.7 + move.z)}) >
			    BlockType::Air
			    ||
			    World::getBlockAt({(int) floorf(camX + 1.3), (int) (camY - 1), (int) floorf(camZ + 1.3 + move.z)}) >
			    BlockType::Air
			    ||
			    World::getBlockAt({(int) floorf(camX + 1.3), (int) (camY - 1), (int) floorf(camZ + 0.7 + move.z)}) >
			    BlockType::Air) {
				renderer.camera.pos.z += move.z;
			}
		} else {
			if (!cameraLocked)
				renderer.camera.pos.z += move.z;
			
			else {
				renderer.camera.pos.z += move.z;
				renderer.camera.look.z -= move.z;
			}
		}
	}
	
	guVecNormalize(&renderer.camera.look);
}

void Player::handleGravity(BlockCoord &coord) {
	if (!gravity) return;
	
	f32 size = sneak ? 1.3005 : 1.6005;
	f32 x = renderer.camera.pos.x + 1;
	f32 y = renderer.camera.pos.y + 1;
	f32 z = renderer.camera.pos.z + 1;
	
	bool canGoDown = (
			World::getBlockAt({(int) floorf(x + 0.3f), (int) floorf(y - size), (int) floorf(z + 0.3f)}) <= BlockType::Air
			&& World::getBlockAt({(int) floorf(x - 0.3f), (int) floorf(y - size), (int) floorf(z + 0.3f)}) <= BlockType::Air
			&& World::getBlockAt({(int) floorf(x + 0.3f), (int) floorf(y - size), (int) floorf(z - 0.3f)}) <= BlockType::Air
			&&
			World::getBlockAt({(int) floorf(x - 0.3f), (int) floorf(y - size), (int) floorf(z - 0.3f)}) <= BlockType::Air);
	
	bool inWater = (
			World::getBlockAt({(int) floorf(x + 0.3), (int) floorf(y - size), (int) floorf(z + 0.3)}) == BlockType::Water
			||
			World::getBlockAt({(int) floorf(x - 0.3), (int) floorf(y - size), (int) floorf(z + 0.3)}) == BlockType::Water
			||
			World::getBlockAt({(int) floorf(x + 0.3), (int) floorf(y - size), (int) floorf(z - 0.3)}) == BlockType::Water
			||
			World::getBlockAt({(int) floorf(x - 0.3), (int) floorf(y - size), (int) floorf(z - 0.3)}) == BlockType::Water);
	if (inWater) {
		Velocity += Acceleration;
		if (Velocity > 0.1)	Velocity = 0.5;
		isJumping = false;
	} else if (isJumping || canGoDown) {
		Velocity += Acceleration;
		if (!canGoDown && Velocity > 0)	isJumping = false;
	} else {
		Velocity = 0;
		goDown(0);
	}
	if (Velocity < 0) goUp(-Velocity);
	else if (Velocity > 0) goDown(Velocity);
}

bool Player::isUnderwater() const {
	return (World::getBlockAt({(int) floorf(renderer.camera.pos.x + 1), (int) floorf(renderer.camera.pos.y + 1),
	                       (int) floorf(renderer.camera.pos.z + 1)}) == BlockType::Water
	    || World::getBlockAt({(int) floorf(renderer.camera.pos.x + 1), (int) floorf(renderer.camera.pos.y + 1),
	                          (int) floorf(renderer.camera.pos.z + 1)}) == BlockType::Water);
}

void Player::update() {
	if (!wiimote.connected) return;
    if(inventory.pickedItem.item.type == BlockType::Air)
        inventory.pickedItem.quantity = 0;
	
    if (wiimote.wd->btns_d & WPAD_BUTTON_UP){
        inventory.open = !(inventory.open);
        if (inventory.craftOpen) inventory.craftOpen = false;
        if (!inventory.open) inventory.ClearCraft();
    }
	
    if (wiimote.wd->btns_d & WPAD_BUTTON_1) wiimoteFocus = !wiimoteFocus;
	
	if (inventory.open){
        f32 x = wiimote.x + 1.0f;
        f32 y = wiimote.y - 1.0f;
		
        if (wiimote.wd->btns_d & WPAD_BUTTON_LEFT && inventory.currentPage > 0) inventory.currentPage--;
        if (wiimote.wd->btns_d & WPAD_BUTTON_RIGHT && inventory.currentPage < 2) inventory.currentPage++;

        int l, c, slot;
        bool isValidCursor = false;
        bool craftSlot = false;

        // Inventory
        if (x > 0.32f && x < 1.73f && y > -1.62f && y < -1.0f) {
            l = -(int)floorf((1.0f + y) / 0.158f) - 1;
            c = (int)floorf((0.32f + x) / 0.158f) - 3;
            slot = l * 9 + c - 1;
            isValidCursor = true;
        }
		
		if (inventory.craftOpen) {
			if (x > 0.49f && x < 0.975f && y > -0.925f && y < -0.486f) {
				l = -(int)floorf((0.486f + y) / 0.157f) - 1;
				c =  (int)floorf((0.49f + x) / 0.163f) - 5;
				slot = l * 3 + c - 1;
				isValidCursor = true;
				craftSlot = true;
			} else if (x > 1.3 && x < 1.55 && y > -0.81 && y < -0.59) {
				slot = 9;
				isValidCursor = true;
				craftSlot = true;
			}
		} else {
			// Craft slots
			if (x > 1.11 && x < 1.425 && y > -0.8 && y < -0.5) {
				l = -(int)floorf((0.5f + y) / 0.158f) - 1;
				c =  (int)floorf((1.1f + x) / 0.158f) - 13;
				slot = l * 3 + c - 1;
				isValidCursor = true;
				craftSlot = true;
			}
				// Craft result
			else if (x > 1.6 && x < 1.75 && y > -0.72 && y < -0.58) {
				slot = 9;
				isValidCursor = true;
				craftSlot = true;
			}
		}
		
		if (wiimote.wd->btns_d & WPAD_BUTTON_A && isValidCursor)
			inventory.action(slot, craftSlot, 0, false, false, creative);
		else if (wiimote.wd->btns_d & WPAD_BUTTON_MINUS && isValidCursor)
			inventory.action(slot, craftSlot, 1, false, true, creative);
		else if (wiimote.wd->btns_d & WPAD_BUTTON_PLUS && isValidCursor)
			inventory.action(slot, craftSlot, 0, true, false, creative);
	}
	else {
		if (!cameraLocked) handleRotation();
		
		BlockCoord coord(floorf(renderer.camera.pos.x + 1), floorf(renderer.camera.pos.y),
		                 floorf(renderer.camera.pos.z + 1));
		
		handleGravity(coord);
		
		sprint = (wiimote.wd->btns_h & WPAD_BUTTON_PLUS) > 0;
		
		focusing = getFocusedBlock();
		if (focusing) {
			if (wiimote.wd->btns_h & WPAD_BUTTON_B) {
				if (focusedBlockType == BlockType::CraftingTable && !sneak) inventory.craftOpen = inventory.open = true;
				else placeBlock();
			}
		}
		placeDelay++;
		
		if (wiimote.wd->btns_d & WPAD_BUTTON_LEFT)
			if (inventory.selectedSlot > 0) inventory.selectedSlot--;
		if (wiimote.wd->btns_d & WPAD_BUTTON_RIGHT)
			if (inventory.selectedSlot < 8) inventory.selectedSlot++;
		
		if (wiimote.wd->btns_d & WPAD_BUTTON_DOWN) sneak = !sneak;
		
		if (gravity) {
			if (wiimote.wd->btns_h & WPAD_BUTTON_A) Jump();
		} else {
			if (wiimote.wd->btns_h & WPAD_BUTTON_A) goUp();
			if (wiimote.wd->btns_h & WPAD_BUTTON_B) goDown();
		}
		
		if (cameraLocked) {
			if (getFocusedBlockDistance() > 7)
				cameraLocked = false;
			else {
				renderer.camera.look.x = (f32) lockedBlockPos.x - renderer.camera.pos.x - 0.5f;
				renderer.camera.look.y = (f32) lockedBlockPos.y - renderer.camera.pos.y - 0.5f;
				renderer.camera.look.z = (f32) lockedBlockPos.z - renderer.camera.pos.z - 0.5f;
				guVecNormalize(&renderer.camera.look);
			}
		}
		
		if (wiimote.wd->exp.type == WPAD_EXP_NUNCHUK) {
			joystick_t sticks = wiimote.wd->exp.nunchuk.js;
			move(sticks);
			destroying = false;
			if (wiimote.accel > 5 && focusing) destroyBlock();
			else {
				if (frame_cntr < 60) frame_cntr++;
				else {
					frame_cntr = 0;
					breakingState = 0;
				}
			}
			
			if (IS_PRESSED((&(wiimote.wd->exp.nunchuk)), NUNCHUK_BUTTON_Z)) {
				cameraLocked = true;
				lockedBlockPos = focusedBlockPos;
			} else if (IS_RELEASED((&(wiimote.wd->exp.nunchuk)), NUNCHUK_BUTTON_Z)) cameraLocked = false;
		}
	}
}

void Player::renderFocus() {
	if (focusing) renderer.renderFocus((f32)focusedBlockPos.x, (f32)focusedBlockPos.y, (f32)focusedBlockPos.z);
}

void Player::renderDestroy() {
	if (destroying) {
		auto breakBlock = (BlockType) ((breakingState - 1) / 5 + BlockType::BlockBreaking0);
		renderer.renderBlock(coordToGuVector(focusedBlockPos), breakBlock);
	}
}






