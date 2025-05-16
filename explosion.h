#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SDL.h>
#include <SDL_image.h>

class Explosion {
public:
    Explosion(SDL_Renderer* renderer, int x, int y);
    ~Explosion();

    void update();
    void render();
    bool isFinished();

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    int frame;
    int maxFrames;
};

#endif
