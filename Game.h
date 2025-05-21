#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>

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
    enum GameState {
        MENU,
        PLAYING
    };

    GameState currentState;
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
    SDL_Texture* backgroundTexture;
    SDL_Rect backgroundRect;

    // Menu
    struct Button {
        SDL_Rect rect;
        std::string label;
        SDL_Texture* texture;
    };
    std::vector<Button> buttons;
    SDL_Texture* menuBackgroundTexture;
    SDL_Rect menuBackgroundRect;
    Mix_Music* menuMusic;

    void initMenu();
    void handleMenuEvents(SDL_Event& event);
    void renderMenu();
};

#endif
