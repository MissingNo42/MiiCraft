//
// Created by Natha on 13/02/2024.
//
#define deadzone 20
#define deadzoneTop 80

#include <cmath>
#include <algorithm>
#include "player.h"

Player::Player() : placeDelay(0), focusedBlockPos(0,0,0), lockedBlockPos(0,0,0), previousFocusedBlockPos(0,0,0),  sprint(false), cameraLocked(false), creative(false){}

Player::Player(f32 x, f32 y, f32 z) : placeDelay(0), focusedBlockPos(0,0,0), lockedBlockPos(0,0,0), previousFocusedBlockPos(0,0,0), sprint(false), cameraLocked(false), creative(false) {
    renderer.camera.pos.x = x;
    renderer.camera.pos.y = y;
    renderer.camera.pos.z = z;
}

f32 Player::getFocusedBlockDistance() const {
    f32 dx = (f32)focusedBlockPos.x - renderer.camera.pos.x;
    f32 dy = (f32)focusedBlockPos.y - renderer.camera.pos.y;
    f32 dz = (f32)focusedBlockPos.z - renderer.camera.pos.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}
bool Player::getFocusedBlock() {
    BlockType type = BlockType::Air;
    BlockCoord pos = BlockCoord(0, 0, 0);
    f32 distance = 0,
            x = renderer.camera.pos.x + 1,
            y = renderer.camera.pos.y + 1,
            z = renderer.camera.pos.z + 1;
    while((type <= BlockType::Air || type == Water) && distance <= 5){
        x += renderer.camera.look.x/200;
        y += renderer.camera.look.y/200;
        z += renderer.camera.look.z/200;
        distance += 0.005;
        pos = BlockCoord((int)floor(x), (int)floor(y), (int)floor(z));
        type = World::getBlockAt(pos);
    }
    if(type > BlockType::Air && type != Water) {
        renderer.renderFocus(World::getBlockAt(pos), (f32) pos.x, (f32) pos.y, (f32) pos.z);
        previousFocusedBlockPos = focusedBlockPos;
        focusedBlockLook = {x,y,z};
        focusedBlockPos = pos;
        focusedBlockType = type;
        return true;
    }
    return false;
}

guVector Player::InverseVector(const guVector& v){
    guVector vtemp;
    vtemp.x = - v.x;
    vtemp.y = - v.y;
    vtemp.z = - v.z;

    return vtemp;
}


void Player::goUp(BlockCoord coord, float velocity, bool collision) {
    f32 size = sneak ? 0.70005 : 0.40005;
    f32 x = renderer.camera.pos.x + 1;
    f32 y = renderer.camera.pos.y + 1;
    f32 z = renderer.camera.pos.z + 1;
    if ( collision ){
        if (World::getBlockAt({(int)floor(x + 0.3), (int)ceil(y + size + velocity /10 - 1), (int)floor(z + 0.3)}) <= BlockType::Air
            && World::getBlockAt({(int)floor(x - 0.3), (int)ceil(y + size +velocity /10 - 1), (int)floor(z + 0.3)}) <= BlockType::Air
            && World::getBlockAt({(int)floor(x + 0.3), (int)ceil(y + size + velocity /10 - 1), (int)floor(z - 0.3)}) <= BlockType::Air
            && World::getBlockAt({(int)floor(x - 0.3), (int)ceil(y + size + velocity /10 - 1), (int)floor(z - 0.3)}) <= BlockType::Air)
            {
                if (!cameraLocked) {
                    renderer.camera.pos.y += velocity / 10;
                } else {
                    renderer.camera.pos.y += velocity / 10;
                    renderer.camera.look.y -= velocity / 10;
                }
            }
        else if (World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size), (int)floor(z + 0.3)}) == BlockType::Water
                 || World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size), (int)floor(z + 0.3)}) == BlockType::Water
                 || World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size), (int)floor(z - 0.3)}) == BlockType::Water
                 || World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size), (int)floor(z - 0.3)}) == BlockType::Water) {
            if (!cameraLocked) {
                renderer.camera.pos.y += velocity / 40;
            } else {
                renderer.camera.pos.y += velocity / 10;
                renderer.camera.look.y -= velocity / 10;
            }
        }
        else{
            renderer.camera.pos.y = (f32) ceil(y + 0.40005 + velocity /10 - 1) - 1.1005f;
            Velocity = 0.6;
        }

    }
    else
        if(!cameraLocked) {
            renderer.camera.pos.y += velocity /10;
        }
        else {
            renderer.camera.pos.y += velocity /10;
            renderer.camera.look.y -= velocity /10;
        }
}


void Player::goDown(BlockCoord coord, float velocity, bool collision ) {
    f32 size = sneak ? 1.3 : 1.6;
    f32 x = renderer.camera.pos.x + 1;
    f32 y = renderer.camera.pos.y + 1;
    f32 z = renderer.camera.pos.z + 1;
    if (collision){
        if ((World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size - velocity / 10), (int)floor(z + 0.3)}) <= BlockType::Air
        && World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size - velocity / 10), (int)floor(z + 0.3)}) <= BlockType::Air
           && World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size - velocity / 10), (int)floor(z - 0.3)}) <= BlockType::Air
              && World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size - velocity / 10), (int)floor(z - 0.3)}) <= BlockType::Air)
                || ( World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size - velocity / 10), (int)floor(z + 0.3)}) == BlockType::Water
                        && World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size - velocity / 10), (int)floor(z + 0.3)}) == BlockType::Water
                        && World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size - velocity / 10), (int)floor(z - 0.3)}) == BlockType::Water
                        && World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size - velocity / 10), (int)floor(z - 0.3)}) == BlockType::Water)) {
            if (!cameraLocked) {
                renderer.camera.pos.y -= velocity / 10;
            } else {
                renderer.camera.pos.y -= velocity / 10;
                renderer.camera.look.y += velocity / 10;
            }
        }
        else{
            renderer.camera.pos.y = floor(y - size - velocity / 10) + size + 0.00001f;
        }
    }
    else {
        if(!cameraLocked) {
            renderer.camera.pos.y -= velocity /10;
        }
        else {
            renderer.camera.pos.y -= velocity /10;
            renderer.camera.look.y += velocity /10;
        }
    }
}

void Player::Jump() {
    if (!isJumping){
        isJumping = true;
        Velocity = -1.65;
    }
}


void Player::setPos(f32 x, f32 y, f32 z) {
    renderer.camera.pos.x = x;
    renderer.camera.pos.y = y;
    renderer.camera.pos.z = z;
}

void Player::handleRotation(WPADData * wd) {
    if (wd->ir.valid) {
        if(wd->ir.x <  (f32) Renderer::rmode->fbWidth/2 - deadzone)
        {
            f32 angle = wd->ir.x / (f32) ((f32) Renderer::rmode->fbWidth/2 - deadzone) * 6;
            renderer.camera.rotateH( M_PI /(2 + angle));
        }
        if(wd->ir.x >  (f32) Renderer::rmode->fbWidth/2 + deadzone)
        {
            f32 angle = (wd->ir.x - (f32) Renderer::rmode->fbWidth/2 + deadzone) / (f32) ((f32) Renderer::rmode->fbWidth/2 - deadzone) * 6;
            renderer.camera.rotateH( - (f32) M_PI /(8 - angle));
        }
        if(wd->ir.y <  (f32) Renderer::rmode->xfbHeight/2 - deadzoneTop)
        {
            f32 angle = wd->ir.y / (f32) ((f32) Renderer::rmode->xfbHeight/2 - deadzoneTop) * 6;
            renderer.camera.rotateV( - (f32) M_PI /(2 +angle));
        }
        if(wd->ir.y >  (f32) Renderer::rmode->xfbHeight/2 + deadzone)
        {
            f32 angle = (wd->ir.y - (f32) Renderer::rmode->xfbHeight/2 + deadzone) / ((f32) Renderer::rmode->xfbHeight/2 - deadzone) * 6;
            renderer.camera.rotateV( M_PI /(8 - angle));
        }
    }
}


void Player::destroyBlock(){
    if(!(focusedBlockPos == previousFocusedBlockPos))
        breakingState = 0;

    if (breakingState < 50)
    {
        BlockType breakBlock = (BlockType)(breakingState / 5 + BlockType::BlockBreaking0);
	    renderer.renderBlock(coordToGuVector(focusedBlockPos), breakBlock);
    }
    else
    {
        BlockType breaked = World::getBlockAt(focusedBlockPos);
        World::setBlockAt(focusedBlockPos, BlockType::Air);
        if (!creative)
            inventory.addItem(breaked, 1);
        if (focusedBlockPos == lockedBlockPos) {
            if(getFocusedBlock()) {
                lockedBlockPos = focusedBlockPos;
            }
            else
                cameraLocked = false;
        }
        breakingState = 0;
    }
    breakingState++;
}

void Player::placeBlock(){
    //if (inventory.inventory[0][inventory.selectedSlot].item.type )  // IF NOT SOLID BLOCK
    //    return;
    if(placeDelay < 10){
        return;}
    BlockCoord pos = focusedBlockPos;
    if(focusedBlockType > BlockType::Air) {
        f32 offsetX, offsetZ;
        if(std::signbit(focusedBlockLook.x - renderer.camera.pos.x))
            offsetX = - 0.3f;
        else
            offsetX = 0.3f;
        if(std::signbit(focusedBlockLook.z - renderer.camera.pos.z))
            offsetZ = - 0.3f;
        else
            offsetZ = 0.3f;
        switch (getFocusedFace()) {
            case 0:
                pos.x--;
                break;
            case 1:
                pos.x++;
                break;
            case 2:
                pos.y--;
                break;
            case 3:
                pos.y++;
                break;
            case 4:
                pos.z--;
                break;
            case 5:
                pos.z++;
                break;
        }
        //printf("offsetX: %f, offsetZ: %f\r", offsetX, offsetZ);
        //printf("newBlockX: %d, newBlockY: %d, newBlockZ: %d\r", pos.x, pos.y, pos.z);
        //printf("focusedBlockX: %d, focusedBlockY: %d, focusedBlockZ: %d\r", focusedBlockPos.x, focusedBlockPos.y, focusedBlockPos.z);
        //printf("playerX: %d, playerY: %d, playerZ: %d\r", (int)floor(renderer.camera.pos.x + 1), (int)floor(renderer.camera.pos.y + 1), (int)floor(renderer.camera.pos.z + 1));
        if(World::getBlockAt(pos) <=BlockType::Air
            && (
                    (pos.x != (int)floor(renderer.camera.pos.x + 1.3) && pos.x != (int)floor(renderer.camera.pos.x + 0.7))
                    || (pos.y != (int)floor(renderer.camera.pos.y + 1) && pos.y != (int)floor(renderer.camera.pos.y))
                    || (pos.z != (int)floor(renderer.camera.pos.z + 1.3) && pos.z != (int)floor(renderer.camera.pos.z + 0.7))))
        {
            World::setBlockAt(pos, inventory.inventory[0][inventory.selectedSlot].item.type);
            if (!creative){
                inventory.inventory[0][inventory.selectedSlot].quantity--;
                if (inventory.inventory[0][inventory.selectedSlot].quantity == 0)
                    inventory.inventory[0][inventory.selectedSlot].item = Item(BlockType::Air);
            }
        }
    }
    placeDelay = 0;
}

int Player::getFocusedFace() const {
    if(focusedBlockType != BlockType::Air) {
        f32 deltaX = (f32) std::fabs((focusedBlockLook.x- round(focusedBlockLook.x)));
        f32 deltaY = (f32) std::fabs((focusedBlockLook.y- round(focusedBlockLook.y)));
        f32 deltaZ = (f32) std::fabs((focusedBlockLook.z- round(focusedBlockLook.z)));

        std::vector<f32> f = {deltaX, deltaY,deltaZ};
        auto min = std::min_element(std::begin(f), std::end(f));
        if (*min == f[0]) {
            if (renderer.camera.pos.x + 1 <= round(focusedBlockLook.x)) {
                return 0;
            } else {
                return 1;
            }
        }
        else if(*min == f[1]){
            if (renderer.camera.pos.y + 1 <= round(focusedBlockLook.y)) {
                return 2;
            } else {
                return 3;
            }
        }
        else{
            if (renderer.camera.pos.z + 1 <= round(focusedBlockLook.z)) {
                return 4;
            } else {
                return 5;
            }
        }
    }
    return -1;
}

BlockCoord Player::guVectorToCoord(guVector v) {
    BlockCoord coord = BlockCoord((int)v.x, (int)v.y, (int)v.z);
    return coord;
}

guVector Player::coordToGuVector(BlockCoord coord) {
    auto v = guVector((f32)coord.x, (f32)coord.y, (f32)coord.z);
    return v;
}

void Player::move(joystick_t sticks) {
    guVector normalizedLook = renderer.camera.look;
    guVecNormalize(&normalizedLook);
    f32 stick_x = (f32)sticks.pos.x - (f32)sticks.center.x;
    f32 stick_y = (f32)sticks.pos.y - (f32)sticks.center.y;
    f32 maxValueX = ((f32) sticks.max.x - (f32) sticks.min.x) / 2;
    f32 maxValueY = ((f32) sticks.max.y - (f32) sticks.min.y) / 2;

    guVector directionMove = {0,0,0};
    guVector move = {0,0,0};
    if(stick_x > 5) {
        directionMove = {0, 0, 0};
        guVecCross(&normalizedLook, &renderer.camera.up, &directionMove);
        move.x += stick_x * directionMove.x / maxValueX;
        move.z += stick_x * directionMove.z / maxValueX;
    }
    else if(stick_x < -5){
        directionMove = {0,0,0};
        guVecCross(&normalizedLook, &renderer.camera.up, &directionMove);
        directionMove = InverseVector(directionMove);
        move.x += - stick_x * directionMove.x / maxValueX;
        move.z += - stick_x * directionMove.z / maxValueX;
    }
    if(stick_y > 5){
        move.x += stick_y * normalizedLook.x / maxValueY;
        move.z += stick_y * normalizedLook.z / maxValueY;
    }
    else if(stick_y < -5){
        directionMove = InverseVector(normalizedLook);
        move.x += - stick_y * directionMove.x / maxValueY;
        move.z += - stick_y * directionMove.z / maxValueY;
    }
    if(move.x != 0 || move.z!=0)
        guVecNormalize(&move);

    if (sneak){
        move.x = move.x * 2.f / 60;
        move.z = move.z * 2.f / 60;
    }
    else if(sprint){
        move.x = move.x * 5.6f / 60;
        move.z = move.z * 5.6f / 60;
    }
    else{
        move.x = move.x * 4.3f / 60;
        move.z = move.z * 4.3f / 60;
    }

    if (true) {
        f32 offsetX, offsetZ;
        if(std::signbit(move.x))
            offsetX = - 0.3;
        else
            offsetX = 0.3;
        if(std::signbit(move.z))
            offsetZ = - 0.3;
        else
            offsetZ = 0.3;
        float camX = renderer.camera.pos.x;
        float camY = sneak ? renderer.camera.pos.y +0.3 :renderer.camera.pos.y;
        float camZ = renderer.camera.pos.z;
        if ((World::getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY- 0.5),
                          (int) floor(camZ + 1.3)}) <= BlockType::Air
                          && World::getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY- 0.5),
                                           (int) floor(camZ + 0.7)}) <= BlockType::Air
                                           && World::getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY + 0.5),
                                                            (int) floor(camZ + 1.3)}) <= BlockType::Air
                                              && World::getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY + 0.5),
                                                               (int) floor(camZ + 0.7)}) <= BlockType::Air) ||
                (World::getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY- 0.5),
                              (int) floor(camZ + 1.3)}) == BlockType::Water
                 && World::getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY- 0.5),
                                 (int) floor(camZ + 0.7)}) == BlockType::Water
                 && World::getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY + 0.5),
                                 (int) floor(camZ + 1.3)}) == BlockType::Water
                 && World::getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY + 0.5),
                                 (int) floor(camZ + 0.7)}) == BlockType::Water)) {
            if (sneak && !isJumping ) {
                if (World::getBlockAt({(int) floor(camX +1.3 + move.x), (int)(camY - 1), (int) floor(camZ + 1.3)}) > BlockType::Air
                || World::getBlockAt({(int) floor(camX +0.7 + move.x), (int)(camY - 1), (int) floor(camZ + 1.3)}) > BlockType::Air
                   || World::getBlockAt({(int) floor(camX +1.3 + move.x), (int)(camY - 1), (int) floor(camZ + 0.7)}) > BlockType::Air
                      || World::getBlockAt({(int) floor(camX +0.7 + move.x), (int)(camY - 1), (int) floor(camZ + 0.7)}) > BlockType::Air) {
                    renderer.camera.pos.x += move.x;
                }
            }
            else {
                if (!cameraLocked)
                    renderer.camera.pos.x += move.x;
                else {
                    renderer.camera.pos.x += move.x;
                    renderer.camera.look.x -= move.x;
                }
            }
        }
        if (( World::getBlockAt({(int) floor(camX + 1.3), (int) (camY - 0.5),
                                (int) floor(offsetZ +camZ + 1 + move.z)}) <= BlockType::Air
             && World::getBlockAt({(int) floor(camX + 0.7), (int) (camY - 0.5),
                              (int) floor(offsetZ +camZ + 1 + move.z)}) <= BlockType::Air
            && World::getBlockAt({(int) floor(camX + 1.3), (int) (camY + 0.5),
                             (int) floor(offsetZ +camZ + 1 + move.z)}) <= BlockType::Air
               && World::getBlockAt({(int) floor(camX + 0.7), (int) (camY + 0.5),
                                (int) floor(offsetZ +camZ + 1 + move.z)}) <= BlockType::Air) ||
                (World::getBlockAt({(int) floor(camX + 1.3), (int) (camY - 0.5),
                              (int) floor(offsetZ +camZ + 1 + move.z)}) == BlockType::Water
                 && World::getBlockAt({(int) floor(camX + 0.7), (int) (camY - 0.5),
                                 (int) floor(offsetZ +camZ + 1 + move.z)}) == BlockType::Water
                 && World::getBlockAt({(int) floor(camX + 1.3), (int) (camY + 0.5),
                                 (int) floor(offsetZ +camZ + 1 + move.z)}) == BlockType::Water
                 && World::getBlockAt({(int) floor(camX + 0.7), (int) (camY + 0.5),
                                 (int) floor(offsetZ +camZ + 1 + move.z)}) == BlockType::Water)
                ){
            if (sneak && !isJumping ) {
                if (World::getBlockAt({(int) floor(camX +0.7), (int)(camY - 1), (int) floor(camZ + 1.3 + move.z)}) > BlockType::Air
                || World::getBlockAt({(int) floor(camX +0.7), (int)(camY - 1), (int) floor(camZ + 0.7 + move.z)}) > BlockType::Air
                   || World::getBlockAt({(int) floor(camX +1.3), (int)(camY - 1), (int) floor(camZ + 1.3 + move.z)}) > BlockType::Air
                      || World::getBlockAt({(int) floor(camX +1.3), (int)(camY - 1), (int) floor(camZ + 0.7 + move.z)}) > BlockType::Air) {
                    renderer.camera.pos.z += move.z;
                }
            }
            else {
                if (!cameraLocked)
                    renderer.camera.pos.z += move.z;

                else {
                    renderer.camera.pos.z += move.z;
                    renderer.camera.look.z -= move.z;
                }
            }
        }
    }
}

void Player::handleGravity(BlockCoord& coord) {
    if (!gravity)
        return;

    f32 size = sneak ? 1.3005 : 1.6005;
    f32 x = renderer.camera.pos.x + 1;
    f32 y = renderer.camera.pos.y + 1;
    f32 z = renderer.camera.pos.z + 1;

    bool canGoDown = (World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size), (int)floor(z + 0.3)}) <= BlockType::Air
                      && World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size), (int)floor(z + 0.3)}) <= BlockType::Air
                      && World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size), (int)floor(z - 0.3)}) <= BlockType::Air
                      && World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size), (int)floor(z - 0.3)}) <= BlockType::Air);

    bool inWater = (World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size), (int)floor(z + 0.3)}) == BlockType::Water
                    || World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size), (int)floor(z + 0.3)}) == BlockType::Water
                    || World::getBlockAt({(int)floor(x + 0.3), (int)floor(y - size), (int)floor(z - 0.3)}) == BlockType::Water
                    || World::getBlockAt({(int)floor(x - 0.3), (int)floor(y - size), (int)floor(z - 0.3)}) == BlockType::Water);
    if (inWater){
        Velocity += Acceleration;
        if (Velocity > 0.1)
            Velocity = 0.5;
        isJumping = false;
    }
    else if (isJumping || canGoDown){
        Velocity += Acceleration;
        //printf("%f %f %f\r", floor(x), floor(y - 1.8), floor(z));
        if (!canGoDown && Velocity > 0)
            isJumping = false;
    } else {
        Velocity = 0;
        //printf("%f %f %f\r", floor(x), floor(y - 1.8), floor(z));
        goDown(coord, 0);
    }
    if (Velocity < 0) {
        goUp(coord, -Velocity);
    } else if (Velocity > 0) {
        goDown(coord, Velocity);
    }
}

bool Player::isUnderwater() const{
    if (World::getBlockAt({(int)floor(renderer.camera.pos.x + 1), (int)floor(renderer.camera.pos.y + 1), (int)floor(renderer.camera.pos.z + 1)}) == BlockType::Water
        || World::getBlockAt({(int)floor(renderer.camera.pos.x + 1), (int)floor(renderer.camera.pos.y + 1), (int)floor(renderer.camera.pos.z + 1)}) == BlockType::Water)
        return true;
    else
        return false;
}






