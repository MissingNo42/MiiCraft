//
// Created by Natha on 13/02/2024.
//
#define deadzone 20
#define deadzoneTop 80

#include <cmath>
#include <algorithm>
#include "player.h"

Player::Player() : placeDelay(0), focusedBlockPos(0,0,0), lockedBlockPos(0,0,0), previousFocusedBlockPos(0,0,0),  sprint(false), cameraLocked(false){}

Player::Player(f32 x, f32 y, f32 z) : placeDelay(0), focusedBlockPos(0,0,0), lockedBlockPos(0,0,0), previousFocusedBlockPos(0,0,0), sprint(false), cameraLocked(false) {
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
bool Player::getFocusedBlock(World &w) {
    BlockType type = BlockType::Air;
    t_coord pos = t_coord(0,0,0);
    f32 distance = 0,
            x = renderer.camera.pos.x + 1,
            y = renderer.camera.pos.y + 1,
            z = renderer.camera.pos.z + 1;
    while(type <= BlockType::Air && distance <= 5){
        x += renderer.camera.look.x/100;
        y += renderer.camera.look.y/100;
        z += renderer.camera.look.z/100;
        distance += 0.01;
        pos = t_coord((int)floor(x), (int)floor(y), (int)floor(z));
        type = w.getBlockAt(pos).type;
    }
    if(type > BlockType::Air) {
        renderer.drawFocus(w.getBlockAt(pos), (f32) pos.x, (f32) pos.y, (f32) pos.z);
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


void Player::goUp(t_coord coord, World &w, float velocity, bool collision) {
    if ( collision ){
        coord.y += 1.6;
        if (w.getBlockAt(coord).type <= BlockType::Air)
            if(!cameraLocked) {
                renderer.camera.pos.y += velocity /10;
            }
            else {
                renderer.camera.pos.y += velocity /10;
                renderer.camera.look.y -= velocity /10;
            }
        else{
            //renderer.camera.look.y -= (f32) renderer.camera.pos.y - (f32) coord.y -1.1f;
            renderer.camera.pos.y = (f32) coord.y -1.1f;
            Velocity = 1.0f;
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


void Player::goDown(t_coord coord, World &w, float velocity, bool collision ) {
    if ( collision ){
        coord.y -= 1;
        if (w.getBlockAt(coord).type <= BlockType::Air)
            if(!cameraLocked) {
                renderer.camera.pos.y -= velocity /10;
            }
            else {
                renderer.camera.pos.y -= velocity /10;
                renderer.camera.look.y += velocity /10;
            }

        else{
            if (sneak) {renderer.camera.pos.y = (f32) floor (coord.y )+ 1.3f;}
            else {renderer.camera.pos.y = (f32) floor (coord.y )+ 1.6f;}
        }
    }
    else
    if(!cameraLocked) {
        renderer.camera.pos.y -= velocity /10;
    }
    else {
        renderer.camera.pos.y -= velocity /10;
        renderer.camera.look.y += velocity /10;
    }
}

void Player::Jump() {
    if (Velocity == 0.0 && !isJumping){
        isJumping = true;
        Velocity = -2.0;
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


void Player::destroyBlock(World& w){
    if(!focusedBlockPos.equals(previousFocusedBlockPos))
        breakingState = 0;

    if (breakingState < 50)
    {
        BlockType breakBlock = (BlockType)(breakingState / 5 + BlockType::BlockBreaking0);
        renderer.renderBloc(coordToGuVector(focusedBlockPos),breakBlock , true, true, true, true, true, true);
    }
    else
    {
        w.setBlockAt(focusedBlockPos, BlockType::Air);
        if (focusedBlockPos.equals(lockedBlockPos)) {
            if(getFocusedBlock(w)) {
                lockedBlockPos = focusedBlockPos;
            }
            else
                cameraLocked = false;
        }
        breakingState = 0;
    }
    breakingState++;
}

void Player::placeBlock(World& w){
    if (inventory.inventory[3][inventory.selectedSlot].item.type > BlockType::Blocks)
        return;
    if(placeDelay < 10){
        return;}
    t_coord pos = focusedBlockPos;
    if(focusedBlockType > BlockType::Air) {
        switch (getFocusedFace(w)) {
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
        //printf("newBlockX: %d, newBlockY: %d, newBlockZ: %d\r", pos.x, pos.y, pos.z);
        //printf("playerX: %d, playerY: %d, playerZ: %d\r", (int)floor(renderer.camera.pos.x + 1), (int)floor(renderer.camera.pos.y + 1), (int)floor(renderer.camera.pos.z + 1));
        if(pos.x != (int)floor(renderer.camera.pos.x + 1)
           || (pos.y != (int)floor(renderer.camera.pos.y + 1)
               && pos.y != (int)floor(renderer.camera.pos.y))
           || pos.z != (int)floor(renderer.camera.pos.z + 1))
        w.setBlockAt(pos, inventory.inventory[3][inventory.selectedSlot].item.type);
    }
    placeDelay = 0;
}

int Player::getFocusedFace(World& w) const {
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

t_coord Player::guVectorToCoord(guVector v) {
    t_coord coord = t_coord((int)v.x, (int)v.y, (int)v.z);
    return coord;
}

guVector Player::coordToGuVector(t_coord coord) {
    auto v = guVector((f32)coord.x, (f32)coord.y, (f32)coord.z);
    return v;
}

void Player::move(World &w, joystick_t sticks) {
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
        if (w.getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY- 0.5),
                          (int) floor(camZ + 1)}).type <= BlockType::Air
            && w.getBlockAt({(int) floor(offsetX + camX + 1 + move.x), (int) (camY + 0.5),
                         (int) floor(camZ + 1)}).type <= BlockType::Air){
            if (sneak && !isJumping ) {
                if (w.getBlockAt({(int) floor(camX +1 + move.x), (int)(camY - 1), (int) floor(camZ + 1)}).type > BlockType::Air) {
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
        if ( w.getBlockAt({(int) floor(camX + 1), (int) (camY - 0.5),
                                (int) floor(offsetZ +camZ + 1 + move.z)}).type <= BlockType::Air
            && w.getBlockAt({(int) floor(camX + 1), (int) (camY + 0.5),
                             (int) floor(offsetZ +camZ + 1 + move.z)}).type <= BlockType::Air) {
            if (sneak && !isJumping ) {
                if (w.getBlockAt({(int) floor(camX +1), (int)(camY - 1), (int) floor(camZ + 1 + move.z)}).type > BlockType::Air) {
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

void Player::handleGravity(World &world, t_coord& coord) {
    if (!gravity)
        return;

    Block blockBelow = world.getBlockAt({coord.x, (int)floor(coord.y - 0.8), coord.z});

    if (isJumping || blockBelow.type <= BlockType::Air) {
        Velocity += Acceleration;
        //printf("%f %f\r", Acceleration, Velocity);
        if (blockBelow.type > BlockType::Air && Velocity > 0)
            isJumping = false;
    } else {
        Velocity = 0;
        goDown(coord, world, 0);
    }

    if (Velocity < 0) {
        goUp(coord, world, -Velocity);
    } else if (Velocity > 0) {
        goDown(coord, world, Velocity);
    }
}






