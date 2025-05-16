#include "round_bullet.h"
#include<iostream>
using namespace std;

SDL_Texture* RoundBullet::sharedTexture = nullptr;

void RoundBullet::loadTexture(SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load("assets/round_bullet.png");
    if (!surface) {
        std::cout << "Lỗi load đạn: " << IMG_GetError() << std::endl;
        return;
    }
    sharedTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void RoundBullet::unloadTexture() {
    if (sharedTexture) {
        SDL_DestroyTexture(sharedTexture);
        sharedTexture = nullptr;
    }
}


RoundBullet::RoundBullet(SDL_Renderer* renderer, float x, float y, float angle, float speed)
    : renderer(renderer), x(x), y(y)
{
    width = 10;
    height = 10;
    vx = cos(angle) * speed;
    vy = sin(angle) * speed;
}


void RoundBullet::update() {
    x += vx;
    y += vy;
}

void RoundBullet::render() {
    SDL_Rect dstRect = {(int)x, (int)y, width, height};
    SDL_RenderCopy(renderer, sharedTexture, NULL, &dstRect);
}


bool RoundBullet::isOffScreen() {
    return (x < -width || x > 800 || y < -height || y > 600);
}

SDL_Rect RoundBullet::getRect() {
    return {(int)x, (int)y, width, height};
}
