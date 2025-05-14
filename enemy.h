#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
class RoundBullet;

class Enemy {
public:
    Enemy(SDL_Renderer* renderer);
    ~Enemy();

    void update();
    void render();
    void shootSpiralBullets(std::vector<RoundBullet*>& bullets);
    void shootSpreadBullets(std::vector<RoundBullet*>& bullets);
    bool hasStoppedMoving();
    SDL_Rect getRect();

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    int speed;
    bool hasStopped;
};

#endif
