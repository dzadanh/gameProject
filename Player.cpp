#include "player.h"
#include<bits/stdc++.h>
using namespace std;

Player::Player(SDL_Renderer* renderer, int skinNumber) : renderer(renderer), texture(nullptr), speed(2.5) {
    string imgPath = "assets/player" + to_string(skinNumber) + ".png";
    SDL_Surface* surface = IMG_Load(imgPath.c_str());
    if (!surface) {
        cout << "Không load được ảnh player: " << IMG_GetError() << "\n";
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    rect.w = 40;
    rect.h = 40;
    rect.x = 368; // Giữa màn hình 800x600
    rect.y = 500;

    // Khởi tạo hitbox ở chính giữa
    hitbox.w = 10;
    hitbox.h = 10;
    hitbox.x = rect.x + (rect.w - hitbox.w) / 2;
    hitbox.y = rect.y + (rect.h - hitbox.h) / 2;
}

Player::~Player() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Player::handleInput(const Uint8* keystates) {
    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) rect.x -= speed;
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) rect.x += speed;
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) rect.y -= speed;
    if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) rect.y += speed;
}

void Player::update() {
    // Giới hạn không cho ra ngoài màn hình
    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.w > 800) rect.x = 800 - rect.w;
    if (rect.y < 0) rect.y = 0;
    if (rect.y + rect.h > 600) rect.y = 600 - rect.h;

    // Cập nhật vị trí hitbox
    hitbox.x = rect.x + (rect.w - hitbox.w) / 2;
    hitbox.y = rect.y + (rect.h - hitbox.h) / 2;
}

void Player::render() {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

void Player::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
    // Cập nhật hitbox khi set vị trí
    hitbox.x = rect.x + (rect.w - hitbox.w) / 2;
    hitbox.y = rect.y + (rect.h - hitbox.h) / 2;
}

SDL_Rect Player::getRect() {
    return rect;
}

SDL_Rect Player::getHitbox() {
    return hitbox;
}
