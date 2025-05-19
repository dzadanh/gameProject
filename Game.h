#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

class Player;
class Star;
class Enemy;
class RoundBullet;
class Explosion;

class Game {
public:
    Game();
    ~Game();
    bool init(const char* title, int width, int height);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running();
    void run();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    Player* player;
    std::vector<Star*> stars;
    std::vector<Enemy*> enemies;
    std::vector<RoundBullet*> roundBullets;
    std::vector<Explosion*> explosions;
    int score;
    int stage;
    Uint32 lastStarResetTime;
    const Uint32 STAR_RESET_INTERVAL;
    Uint32 lastEnemyShootTime;
    const Uint32 ENEMY_SHOOT_INTERVAL;
    SDL_Texture* backgroundTexture; // Thêm texture cho background
    SDL_Rect backgroundRect;        // Thêm rect cho background
};

#endif
