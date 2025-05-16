#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
class RoundBullet;

class Enemy {
public:
    Enemy(SDL_Renderer* renderer, int stage = 1);
    ~Enemy();

    void update();
    void render();
    void shootSpiralBullets(std::vector<RoundBullet*>& bullets);
    void shootSpreadBullets(std::vector<RoundBullet*>& bullets);
    void shootRandomBullets(std::vector<RoundBullet*>& bullets); // 5 viên cùng hướng
    void shootSpiralWaveBullets(std::vector<RoundBullet*>& bullets);
    bool hasStoppedMoving();
    bool isOffScreen();
    void flyUp();
    SDL_Rect getRect();

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    int speed;
    bool hasStopped;
    bool isFlyingUp;
    int stage;
};

#endif
