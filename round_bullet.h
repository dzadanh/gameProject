#ifndef ROUND_BULLET_H
#define ROUND_BULLET_H

#include <SDL.h>
#include <SDL_image.h>
#include <cmath>

class RoundBullet {
public:
    static void loadTexture(SDL_Renderer* renderer);  // thêm hàm static
    static void unloadTexture();                      // giải phóng khi clean
    static SDL_Texture* sharedTexture;                // texture dùng chung

    RoundBullet(SDL_Renderer* renderer, float x, float y, float angle, float speed);
    void update();
    void render();
    bool isOffScreen();
    SDL_Rect getRect();

private:
    SDL_Renderer* renderer;
    float x, y;
    float vx, vy;
    int width, height;
};

#endif
