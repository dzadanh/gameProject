#include "star.h"
#include<bits/stdc++.h>
using namespace std;

Star::Star(SDL_Renderer* renderer) : renderer(renderer), texture(nullptr), speed(2.5) {
    SDL_Surface* surface = IMG_Load("assets/star.png");
    if (!surface) {
        SDL_Log("Không thể load ảnh star: %s", IMG_GetError());
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    srand(time(nullptr)); // Khởi tạo random

    rect.w = 20;
    rect.h = 20;
    rect.x = rand() % (800 - rect.w);
    rect.y = -(rand() % 600); // Rơi ngẫu nhiên từ ngoài màn hình
}

Star::~Star() {
    if (texture) SDL_DestroyTexture(texture);
}

void Star::update() {
    rect.y += speed;
}

void Star::render() {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

SDL_Rect Star::getRect() {
    return rect;
}

bool Star::isOffScreen() {
    return rect.y > 600;
}

void Star::reset() {
    rect.x = rand() % (800 - rect.w);
    rect.y = -rect.h;
}
