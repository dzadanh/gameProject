#ifndef STAR_H
#define STAR_H

#include <SDL.h>
#include <SDL_image.h>

class Star {
public:
    Star(SDL_Renderer* renderer);
    ~Star();

    void update();
    void render();
    SDL_Rect getRect();
    bool isOffScreen();
    void reset();  // Cho rơi lại từ trên xuống

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    int speed;
};

#endif
