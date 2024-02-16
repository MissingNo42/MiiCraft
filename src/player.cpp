//
// Created by Natha on 13/02/2024.
//
#define deadzone 20
#define deadzoneTop 80

#include <cmath>
#include <algorithm>
#include "player.h"

Player::Player() : focusedBlockPos(0,0,0), previousFocusedBlockPos(0,0,0), isTargeting(false), sprint(false){}

Player::Player(f32 x, f32 y, f32 z) : focusedBlockPos(0,0,0), previousFocusedBlockPos(0,0,0), isTargeting(false), sprint(false) {
    renderer.camera.pos.x = x;
    renderer.camera.pos.y = y;
    renderer.camera.pos.z = z;
}

void Player::getFocusedBlock(World &w) {
    BlockType type = BlockType::Air;
    t_coord pos = t_coord(0,0,0);
    f32 distance = 0,
            x = renderer.camera.pos.x + 1,
            y = renderer.camera.pos.y + 1,
            z = renderer.camera.pos.z + 1;
    while(type == BlockType::Air && distance <= 5){
        x += renderer.camera.look.x/20;
        y += renderer.camera.look.y/20;
        z += renderer.camera.look.z/20;
        distance += 0.05;
        pos = t_coord((int)floor(x), (int)floor(y), (int)floor(z));
        type = w.getBlockAt(pos).type;
    }
    if(type != BlockType::Air) {
        renderer.drawFocus(w.getBlockAt(pos), (f32) pos.x, (f32) pos.y, (f32) pos.z);
        isTargeting = true;
        previousFocusedBlockPos = focusedBlockPos;
        focusedBlockPos = pos;
        return;
    }
    isTargeting = false;
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
        coord.y += 2;
        if (w.getBlockAt(coord).type == BlockType::Air)
            renderer.camera.pos.y += velocity /10;
        else
            renderer.camera.pos.y = coord.y -1.1;
    }
    else
        renderer.camera.pos.y += velocity / 10;
}


void Player::goDown(t_coord coord, World &w, float velocity, bool collision ) {
    if ( collision ){
        coord.y -= 1;
        if (w.getBlockAt(coord).type == BlockType::Air)
            renderer.camera.pos.y -= velocity /10;
        else
            renderer.camera.pos.y = (f32) floor (coord.y )+ 1.8;
    }
    else
        renderer.camera.pos.y -= velocity /10;
}

void Player::Jump() {
    printf("%f %f\r", Acceleration, Velocity);
    isJumping = true;
    if (Velocity == 0)
        Acceleration = -0.2;
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
            f32 angle = wd->ir.y / (f32) ((f32) Renderer::rmode->xfbHeight/2 - deadzone) * 6;
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
    if (breakingState < 10) {
        breakingState++;
        renderer.renderBloc(coordToGuVector(focusedBlockPos), 1, true, true, true, true, true, true);
    }
    else if (breakingState < 20) {
        breakingState++;
        renderer.renderBloc(coordToGuVector(focusedBlockPos), 2, true, true, true, true, true, true);
    }
    else if (breakingState < 30) {
        breakingState++;
        renderer.renderBloc(coordToGuVector(focusedBlockPos), 3, true, true, true, true, true, true);
    }
    else if (breakingState < 40) {
        breakingState++;
        renderer.renderBloc(coordToGuVector(focusedBlockPos), 5, true, true, true, true, true, true);
    }
    else{
        w.setBlockAt(focusedBlockPos, BlockType::Air);
        breakingState = 0;
    }
}

void Player::placeBlock(World& w){
    if(placeDelay < 10)
        return;
    BlockType type = BlockType::Air;
    t_coord pos = t_coord(0,0,0);
    f32 distance = 0,
            x = renderer.camera.pos.x + 1,
            y = renderer.camera.pos.y + 1,
            z = renderer.camera.pos.z + 1;
    while(type == BlockType::Air && distance <= 5){
        x += renderer.camera.look.x/20;
        y += renderer.camera.look.y/20;
        z += renderer.camera.look.z/20;
        distance += 0.05;
        pos = t_coord((int)floor(x), (int)floor(y), (int)floor(z));
        type = w.getBlockAt(pos).type;
    }
    if(type != BlockType::Air) {
        switch (getFocusedFace(w)) {
            case 0:
                pos.x--;
                if(pos.x != (int)floor(renderer.camera.pos.x + 1)
                    || pos.y != (int)floor(renderer.camera.pos.y + 1)
                    || pos.z != (int)floor(renderer.camera.pos.z + 1))
                    w.setBlockAt(pos, BlockType::Bedrock);
                break;
            case 1:
                pos.x++;
                if(pos.x != (int)floor(renderer.camera.pos.x + 1)
                   || pos.y != (int)floor(renderer.camera.pos.y + 1)
                   || pos.z != (int)floor(renderer.camera.pos.z + 1))
                    w.setBlockAt(pos, BlockType::Bedrock);
                break;
            case 2:
                pos.y--;
                if(pos.x != (int)floor(renderer.camera.pos.x + 1)
                   || pos.y != (int)floor(renderer.camera.pos.y + 1)
                   || pos.z != (int)floor(renderer.camera.pos.z + 1))
                    w.setBlockAt(pos, BlockType::Bedrock);
                break;
            case 3:
                pos.y++;
                if(pos.x != (int)floor(renderer.camera.pos.x + 1)
                   || pos.y != (int)floor(renderer.camera.pos.y + 1)
                   || pos.z != (int)floor(renderer.camera.pos.z + 1))
                    w.setBlockAt(pos, BlockType::Bedrock);
                break;
            case 4:
                pos.z--;
                if(pos.x != (int)floor(renderer.camera.pos.x + 1)
                   || pos.y != (int)floor(renderer.camera.pos.y + 1)
                   || pos.z != (int)floor(renderer.camera.pos.z + 1))
                    w.setBlockAt(pos, BlockType::Bedrock);
                break;
            case 5:
                pos.z++;
                if(pos.x != (int)floor(renderer.camera.pos.x + 1)
                   || pos.y != (int)floor(renderer.camera.pos.y + 1)
                   || pos.z != (int)floor(renderer.camera.pos.z + 1))
                    w.setBlockAt(pos, BlockType::Bedrock);
                break;
        }
    }
    placeDelay = 0;
}

int Player::getFocusedFace(World& w) const {
    BlockType type = BlockType::Air;
    t_coord pos = t_coord(0,0,0);
    f32 distance = 0,
            x = renderer.camera.pos.x + 1,
            y = renderer.camera.pos.y + 1,
            z = renderer.camera.pos.z + 1;
    while(type == BlockType::Air && distance <= 5){
        x += renderer.camera.look.x/20;
        y += renderer.camera.look.y/20;
        z += renderer.camera.look.z/20;
        distance += 0.05;
        pos = t_coord((int)floor(x), (int)floor(y), (int)floor(z));
        type = w.getBlockAt(pos).type;
    }
    if(type != BlockType::Air) {
        f32 deltaX = (f32) std::fabs((x- round(x)));
        f32 deltaY = (f32) std::fabs((y- round(y)));
        f32 deltaZ = (f32) std::fabs((z- round(z)));

        std::vector<f32> f = {deltaX, deltaY,deltaZ};
        auto min = std::min_element(std::begin(f), std::end(f));
        if (*min == f[0]) {
            if (renderer.camera.pos.x + 1 <= round(x)) {
                return 0;
            } else {
                return 1;
            }
        }
        else if(*min == f[1]){
            if (renderer.camera.pos.y + 1 <= round(y)) {
                return 2;
            } else {
                return 3;
            }
        }
        else{
            if (renderer.camera.pos.z + 1 <= round(z)) {
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
        c_guVecNormalize(&move);

    if(sprint){
        move.x = move.x * 5.6f / 30;
        move.z = move.z * 5.6f / 30;
    }
    else{
        move.x = move.x * 4.3f / 30;
        move.z = move.z * 4.3f / 30;
    }

    if (true) {
        f32 offsetX, offsetZ;
        if(std::signbit(move.x))
            offsetX = - 0.2;
        else
            offsetX = 0.2;
        if(std::signbit(move.z))
            offsetZ = - 0.2;
        else
            offsetZ = 0.2;
        if (w.getBlockAt({(int) floor(offsetX + renderer.camera.pos.x + 1 + move.x), (int) renderer.camera.pos.y,
                          (int) floor(renderer.camera.pos.z + 1)}).type == BlockType::Air
            && w.getBlockAt({(int) floor(renderer.camera.pos.x + 1), (int) renderer.camera.pos.y,
                             (int) floor(offsetZ +renderer.camera.pos.z + 1 + move.z)}).type == BlockType::Air
            && w.getBlockAt({(int) floor(offsetX + renderer.camera.pos.x + 1 + move.x), (int) renderer.camera.pos.y + 1,
                         (int) floor(renderer.camera.pos.z + 1)}).type == BlockType::Air
            && w.getBlockAt({(int) floor(renderer.camera.pos.x + 1), (int) renderer.camera.pos.y + 1,
                             (int) floor(offsetZ +renderer.camera.pos.z + 1 + move.z)}).type == BlockType::Air) {
            renderer.camera.pos.x += move.x;
            renderer.camera.pos.z += move.z;
        }
    }
}

void Player::handleGravity(World &w, t_coord& coord) {
    if (gravity){
        if (Acceleration < 0.49)
            Acceleration += 0.01;

        if (isJumping || w.getBlockAt({coord.x, (int) floor (coord.y - 0.8), coord.z}).type == BlockType::Air) {
            Velocity += Acceleration;
            printf("%f %f\r", Acceleration, Velocity);
            if (w.getBlockAt({coord.x, (int) floor (coord.y - 0.8), coord.z}).type == BlockType::Air)
                isJumping = false;
        }
        else{
            Velocity = 0;
        }

        if ( Velocity < 0){
            goUp(coord,w, -Velocity);
        }
        if (Velocity > 0){
            goDown(coord, w, Velocity);
        }
    }
}



