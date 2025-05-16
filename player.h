#ifndef PLAYER_H
#define PLAYER_H

#include<SDL.h>
#include<SDL_image.h>
#include<bits/stdc++.h>

class Player{
public:
    Player(SDL_Renderer* renderer);
    ~Player();

    void handleInput(const Uint8* keystates);
    void update();
    void render();

    void setPosition(int x, int y);
    SDL_Rect getRect();
    SDL_Rect getHitbox(); // Thêm hàm lấy hitbox

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    SDL_Rect hitbox; // Thêm hitbox
    int speed;
};

#endif
