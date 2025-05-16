#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include "player.h"
#include "enemy.h"
#include "explosion.h"
#include <vector>

extern Player* player;
extern std::vector<Enemy*> enemies;
extern int score;
extern int stage;

class Game {
public:
    Game() : window(nullptr), renderer(nullptr), isRunning(false) {}

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
    std::vector<Explosion*> explosions; // Thêm để quản lý hiệu ứng nổ
};

#endif
