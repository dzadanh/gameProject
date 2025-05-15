#include "player.h"
#include<bits/stdc++.h>
using namespace std;

Player::Player(SDL_Renderer* renderer) : renderer(renderer), texture(nullptr), speed(2.5){
    SDL_Surface* surface = IMG_Load("assets/player1.png");
    if(!surface){
        cout<<"không load được ảnh player!"<<IMG_GetError()<<"\n";
    }
    else{
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }
    rect.w = 40;
    rect.h = 40;
    rect.x = 368;// Giữa màn hình 800x600
    rect.y = 500;
}

Player::~Player(){
    if(texture){
        SDL_DestroyTexture(texture);
    }
}

void Player::handleInput(const Uint8* keystates){
    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) rect.x -= speed;
    if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) rect.x += speed;
    if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) rect.y -= speed;
    if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) rect.y += speed;
}

void Player::update(){
    // Giới hạn không cho ra ngoài màn hình
    if (rect.x < 0) rect.x = 0;
    if (rect.x + rect.w > 800) rect.x = 800 - rect.w;
    if (rect.y < 0) rect.y = 0;
    if (rect.y + rect.h > 600) rect.y = 600 - rect.h;
}

void Player::render(){
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

void Player::setPosition(int x, int y){
    rect.x = x;
    rect.y = y;
}

SDL_Rect Player::getRect(){
    return rect;
}










