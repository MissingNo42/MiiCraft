//
// Created by Natha on 13/02/2024.
//
#define deadzone 20
#define deadzoneTop 80
#include <cmath>
#include "player.h"

Player::Player() {}

t_coord Player::getFocusedBlock(World &w) {
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
        return pos;
    }
    throw std::invalid_argument("No blocks are targeted");
}

guVector Player::InverseVector(const guVector& v){
    guVector vtemp;
    vtemp.x = - v.x;
    vtemp.y = - v.y;
    vtemp.z = - v.z;

    return vtemp;
}

void Player::goLeft(guVector& normalizedLook, int speed, bool collision, World& w) {
    guVector move = {0,0,0};
    guVecCross(&normalizedLook, &renderer.camera.up, &move);
    move = InverseVector(move);
    if (collision) {
        if (w.getBlockAt({(int)ceil(renderer.camera.pos.x + move.x), (int) renderer.camera.pos.y, (int)ceil(renderer.camera.pos.z + move.z)}).type == BlockType::Air
            && w.getBlockAt({(int)ceil(renderer.camera.pos.x + move.x), (int) renderer.camera.pos.y+1, (int)ceil(renderer.camera.pos.z + move.z)}).type == BlockType::Air){
            renderer.camera.pos.x += move.x / 10 * (f32) speed;
            renderer.camera.pos.z += move.z / 10 * (f32) speed;
        }
    }
    else {
        renderer.camera.pos.x += move.x / 10 * (f32) speed;
        renderer.camera.pos.z += move.z / 10 * (f32) speed;
    }
}

void Player::goRight(guVector& normalizedLook, int speed, bool collision, World& w) {
    guVector move = {0,0,0};
    guVecCross(&normalizedLook, &renderer.camera.up, &move);
    if (collision) {
        if (w.getBlockAt({(int)ceil( renderer.camera.pos.x + move.x), (int) renderer.camera.pos.y, (int)ceil(renderer.camera.pos.z+move.z)}).type == BlockType::Air
            && w.getBlockAt({(int)ceil( renderer.camera.pos.x + move.x), (int) renderer.camera.pos.y+1, (int)ceil(renderer.camera.pos.z+move.z)}).type == BlockType::Air){
            renderer.camera.pos.x += move.x / 10 * (f32) speed;
            renderer.camera.pos.z += move.z / 10 * (f32) speed;
        }
    }
    else {
        renderer.camera.pos.x += move.x / 10 * (f32) speed;
        renderer.camera.pos.z += move.z / 10 * (f32) speed;
    }
}

void Player::goForward(guVector& normalizedLook, int speed, bool collision, World& w) {
    if (collision) {
        if (w.getBlockAt({(int)ceil(renderer.camera.pos.x + renderer.camera.look.x), (int) renderer.camera.pos.y, (int)ceil(renderer.camera.pos.z+renderer.camera.look.z)}).type == BlockType::Air
            && w.getBlockAt({(int)ceil(renderer.camera.pos.x + renderer.camera.look.x), (int) renderer.camera.pos.y+1, (int)ceil(renderer.camera.pos.z+renderer.camera.look.z)}).type == BlockType::Air){
            renderer.camera.pos.x += normalizedLook.x/10 * (f32) speed;
            renderer.camera.pos.z += normalizedLook.z/10 * (f32) speed;
        }
    }
    else {
        renderer.camera.pos.x += normalizedLook.x / 10 * (f32) speed;
        renderer.camera.pos.z += normalizedLook.z / 10 * (f32) speed;
    }
}

void Player::goBackward(guVector& normalizedLook, int speed, bool collision, World& w) {
    guVector move = InverseVector(normalizedLook);
    if (collision) {
        if (w.getBlockAt({(int)ceil(renderer.camera.pos.x+1 + move.x), (int) renderer.camera.pos.y, (int) ceil(renderer.camera.pos.z+move.z)}).type == BlockType::Air
            && w.getBlockAt({(int)ceil(renderer.camera.pos.x+1 + move.x), (int) renderer.camera.pos.y+1, (int) ceil(renderer.camera.pos.z+move.z)}).type == BlockType::Air){
            renderer.camera.pos.x += move.x/10 * (f32) speed;
            renderer.camera.pos.z += move.z/10 * (f32) speed;
        }
    }
    else {
        renderer.camera.pos.x += move.x/10 * (f32) speed;
        renderer.camera.pos.z += move.z/10 * (f32) speed;
    }
}

void Player::goUp(t_coord coord, bool collision, World &w) {
    if ( collision ){
        coord.y += 1;
        if (w.getBlockAt(coord).type == BlockType::Air)
            renderer.camera.pos.y += 0.1;
    }
    else
        renderer.camera.pos.y += 0.1;
}

void Player::goDown(t_coord coord, bool collision, World &w) {
    if ( collision ){
        coord.y -= 1;
        if (w.getBlockAt(coord).type == BlockType::Air)
            renderer.camera.pos.y -= 0.1;
    }
    else
        renderer.camera.pos.y -= 0.1;
}

void Player::setPos(f32 x, f32 y, f32 z) {
    renderer.camera.pos.x = x;
    renderer.camera.pos.y = y;
    renderer.camera.pos.z = z;
}

Player::Player(f32 x, f32 y, f32 z) : speed(1), renderer(), wiimote() {
    renderer.camera.pos.x = x;
    renderer.camera.pos.y = y;
    renderer.camera.pos.z = z;
}

void Player::handleMovement(World& w, t_coord focusedBlockPos, bool targetable, bool collision) {
    guVector normalizedLook = renderer.camera.look;
    guVecNormalize(&normalizedLook);
    t_coord coord((int)renderer.camera.pos.x+1, (int)renderer.camera.pos.y, (int)renderer.camera.pos.z+1);
    speed = 1;

    if ( wiimote.directions & WPAD_BUTTON_PLUS)
        speed = 3;
    if ( wiimote.directions & WPAD_BUTTON_MINUS && targetable)
        DestroyBlock(focusedBlockPos, w);
    if ( wiimote.directions & WPAD_BUTTON_LEFT )
        goLeft(normalizedLook, speed, collision, w);
    if ( wiimote.directions & WPAD_BUTTON_RIGHT )
        goRight(normalizedLook, speed, collision, w);
    if ( wiimote.directions & WPAD_BUTTON_UP )
        goForward(normalizedLook, speed, collision, w);
    if ( wiimote.directions & WPAD_BUTTON_DOWN )
        goBackward(normalizedLook, speed, collision, w);
    if ( wiimote.directions & WPAD_BUTTON_A )
        goUp(coord, collision, w);
    if ( wiimote.directions & WPAD_BUTTON_B)
        goDown(coord, collision, w);
    wiimote.directions = 0;
}

void Player::handleRotation() {
    if (wiimote.wd->ir.valid) {
        if(wiimote.wd->ir.x <  (f32) Renderer::rmode->fbWidth/2 - deadzone)
        {
            f32 angle = wiimote.wd->ir.x / (f32) ((f32) Renderer::rmode->fbWidth/2 - deadzone) * 6;
            renderer.camera.rotateH( M_PI /(2 + angle));
        }
        if(wiimote.wd->ir.x >  (f32) Renderer::rmode->fbWidth/2 + deadzone)
        {
            f32 angle = (wiimote.wd->ir.x - (f32) Renderer::rmode->fbWidth/2 + deadzone) / (f32) ((f32) Renderer::rmode->fbWidth/2 - deadzone) * 6;
            renderer.camera.rotateH( - (f32) M_PI /(8 - angle));
        }
        if(wiimote.wd->ir.y <  (f32) Renderer::rmode->xfbHeight/2 - deadzoneTop)
        {
            f32 angle = wiimote.wd->ir.y / (f32) ((f32) Renderer::rmode->xfbHeight/2 - deadzoneTop) * 6;
            renderer.camera.rotateV( - (f32) M_PI /(2 +angle));
        }
        if(wiimote.wd->ir.y >  (f32) Renderer::rmode->xfbHeight/2 + deadzone)
        {
            f32 angle = (wiimote.wd->ir.y - (f32) Renderer::rmode->xfbHeight/2 + deadzone) / ((f32) Renderer::rmode->xfbHeight/2 - deadzone) * 6;
            renderer.camera.rotateV( M_PI /(8 - angle));
        }
    }
    wiimote.wd = nullptr;
}

bool Player::handleInput(World& w, t_coord focusedBlockPos, bool targetable){
    if(wiimote.update()){
        handleMovement(w, focusedBlockPos, targetable);
        handleRotation();
        return true;
    }
    return false;
}


void Player::DestroyBlock(t_coord coord, World& w){
    w.setBlockAt(coord, BlockType::Air);
}


