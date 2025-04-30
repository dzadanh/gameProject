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

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    int speed;
};

#endif // PL
