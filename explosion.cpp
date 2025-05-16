#include "explosion.h"
#include <bits/stdc++.h>
using namespace std;

Explosion::Explosion(SDL_Renderer* renderer, int x, int y)
    : renderer(renderer), texture(nullptr), frame(0), maxFrames(30) {
    SDL_Surface* surface = IMG_Load("assets/explosion.png");
    if (!surface) {
        cout << "Không thể load ảnh explosion: " << IMG_GetError() << endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    rect.w = 100;
    rect.h = 100;
    rect.x = x - rect.w / 2;
    rect.y = y - rect.h / 2;
}

Explosion::~Explosion() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Explosion::update() {
    frame++;
}

void Explosion::render() {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

bool Explosion::isFinished() {
    return frame >= maxFrames;
}
