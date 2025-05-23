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
        PLAYING,
        TUTORIAL,
        SKIN_SELECTION,
        GAME_OVER,
        WINNER
    };

    GameState currentState;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
    Player* player;
    std::vector<Star*> stars;
    std::vector<Enemy*> enemies;
    std::vector<RoundBullet*> roundBullets;
    int score;
    int stage;
    Uint32 lastStarResetTime;
    const Uint32 STAR_RESET_INTERVAL;
    Uint32 lastEnemyShootTime;
    const Uint32 ENEMY_SHOOT_INTERVAL;
    SDL_Texture* backgroundTexture;
    SDL_Rect backgroundRect;

    // Score display
    SDL_Texture* scoreTextTexture;
    SDL_Rect scoreTextRect;
    SDL_Texture* gameOverScoreTextTexture;
    SDL_Rect gameOverScoreTextRect;

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
    Mix_Music* gameMusic;
    Mix_Chunk* clickSound;
    bool musicOn;

    // Skin selection
    int selectedSkin;
    std::vector<Button> skinButtons;
    SDL_Texture* currentSkinTexture;
    SDL_Rect skinPreviewRect;
    void initSkinSelection();
    void handleSkinSelectionEvents(SDL_Event& event);
    void renderSkinSelection();

    // Game Over
    std::vector<Button> gameOverButtons;
    SDL_Texture* gameOverTextTexture;
    SDL_Rect gameOverTextRect;
    void initGameOver();
    void handleGameOverEvents(SDL_Event& event);
    void renderGameOver();

    // Winner
    std::vector<Button> winnerButtons;
    SDL_Texture* winnerTextTexture;
    SDL_Rect winnerTextRect;
    void initWinner();
    void handleWinnerEvents(SDL_Event& event);
    void renderWinner();

    void initMenu();
    void handleMenuEvents(SDL_Event& event);
    void renderMenu();
    void renderTutorial();
    void resetGame();
};

#endif
