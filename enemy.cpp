#include "enemy.h"
#include <iostream>
using namespace std;

Enemy::Enemy(SDL_Renderer* renderer)
    : renderer(renderer), texture(nullptr), speed(2), hasStopped(false) {
    SDL_Surface* surface = IMG_Load("assets/enemy.png");
    if (!surface) {
        std::cout << "Không thể load ảnh enemy: " << IMG_GetError() << std::endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    rect.w = 90;
    rect.h = 90;
    rect.x = 368;  // Giữa màn hình 800x600
    rect.y = -rect.h; // Xuất hiện từ trên trời rơi xuống
}

Enemy::~Enemy() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Enemy::update() {
    if (!hasStopped) {
        rect.y += speed;
        if (rect.y >= 100) {  // Cho đứng yên tại vị trí này
            rect.y = 100;
            hasStopped = true;
        }
    }
}

void Enemy::render() {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

SDL_Rect Enemy::getRect() {
    return rect;
}
