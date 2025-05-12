#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include <SDL_image.h>

class Enemy {
public:
    Enemy(SDL_Renderer* renderer);
    ~Enemy();

    void update();            // Cập nhật vị trí
    void render();            // Vẽ enemy
    SDL_Rect getRect();       // Trả về hình chữ nhật va chạm

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    int speed;
    bool hasStopped;
};

#endif
