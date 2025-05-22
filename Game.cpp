#include "game.h"
#include "player.h"
#include "star.h"
#include "enemy.h"
#include "round_bullet.h"
#include <bits/stdc++.h>
using namespace std;

Game::Game()
    : currentState(MENU), window(nullptr), renderer(nullptr), isRunning(false),
      player(nullptr), score(0), stage(1), lastStarResetTime(0),
      STAR_RESET_INTERVAL(5000), lastEnemyShootTime(0), ENEMY_SHOOT_INTERVAL(1000),
      backgroundTexture(nullptr), menuBackgroundTexture(nullptr), menuMusic(nullptr),
      gameMusic(nullptr), clickSound(nullptr), musicOn(true), selectedSkin(1),
      currentSkinTexture(nullptr), gameOverTextTexture(nullptr), winnerTextTexture(nullptr),
      scoreTextTexture(nullptr), gameOverScoreTextTexture(nullptr) {}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cout << "SDL không thể khởi tạo! Lỗi: " << SDL_GetError() << endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "SDL_image lỗi: " << IMG_GetError() << endl;
        return false;
    }

    if (TTF_Init() == -1) {
        cout << "SDL_ttf lỗi: " << TTF_GetError() << endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "SDL_mixer lỗi: " << Mix_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        cout << "Không tạo được cửa sổ! Lỗi: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Không tạo được renderer! Lỗi: " << SDL_GetError() << endl;
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    player = new Player(renderer, selectedSkin);
    stars.push_back(new Star(renderer));
    lastStarResetTime = SDL_GetTicks();

    enemies.push_back(new Enemy(renderer, 1));

    RoundBullet::loadTexture(renderer);

    SDL_Surface* backgroundSurface = IMG_Load("assets/background.png");
    if (!backgroundSurface) {
        cout << "Lỗi load background: " << IMG_GetError() << endl;
        return false;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    SDL_FreeSurface(backgroundSurface);

    backgroundRect.x = 0;
    backgroundRect.y = 0;
    backgroundRect.w = width;
    backgroundRect.h = height;

    SDL_Surface* menuBackgroundSurface = IMG_Load("assets/menu_background.png");
    if (!menuBackgroundSurface) {
        cout << "Lỗi load menu background: " << IMG_GetError() << endl;
        return false;
    }
    menuBackgroundTexture = SDL_CreateTextureFromSurface(renderer, menuBackgroundSurface);
    SDL_FreeSurface(menuBackgroundSurface);

    menuBackgroundRect.x = 0;
    menuBackgroundRect.y = 0;
    menuBackgroundRect.w = width;
    menuBackgroundRect.h = height;

    menuMusic = Mix_LoadMUS("assets/menu_music.mp3");
    if (!menuMusic) {
        cout << "Lỗi load menu music: " << Mix_GetError() << endl;
        return false;
    }

    gameMusic = Mix_LoadMUS("assets/game_music.mp3");
    if (!gameMusic) {
        cout << "Lỗi load game music: " << Mix_GetError() << endl;
        return false;
    }

    clickSound = Mix_LoadWAV("assets/click_button.wav");
    if (!clickSound) {
        cout << "Lỗi load click sound: " << Mix_GetError() << endl;
        return false;
    }

    // Initialize score text
    TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 24);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return false;
    }
    SDL_Color textColor = {255, 255, 255, 255};
    string scoreText = "Score: " + to_string(score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!scoreSurface) {
        cout << "Lỗi tạo score surface: " << TTF_GetError() << endl;
        TTF_CloseFont(font);
        return false;
    }
    scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_QueryTexture(scoreTextTexture, nullptr, nullptr, &scoreTextRect.w, &scoreTextRect.h);
    scoreTextRect.x = 10;
    scoreTextRect.y = 10;
    TTF_CloseFont(font);

    if (musicOn) {
        Mix_PlayMusic(menuMusic, -1);
    }

    initMenu();
    initGameOver();
    initWinner();

    isRunning = true;
    return true;
}

void Game::initMenu() {
    vector<string> buttonLabels = {"Play", "Music", "Tutorial", "Skin"};
    int buttonWidth = 200;
    int buttonHeight = 60;
    int startY = 200;
    int spacing = 20;

    SDL_Color textColor = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 32);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return;
    }

    for (size_t i = 0; i < buttonLabels.size(); ++i) {
        Button button;
        button.rect.w = buttonWidth;
        button.rect.h = buttonHeight;
        button.rect.x = (800 - buttonWidth) / 2;
        button.rect.y = startY + i * (buttonHeight + spacing);
        button.label = buttonLabels[i];

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, button.label.c_str(), textColor);
        button.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        buttons.push_back(button);
    }
    TTF_CloseFont(font);
}

void Game::initGameOver() {
    SDL_Color textColor = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 48);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Game Over", textColor);
    gameOverTextTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_QueryTexture(gameOverTextTexture, nullptr, nullptr, &gameOverTextRect.w, &gameOverTextRect.h);
    gameOverTextRect.x = (800 - gameOverTextRect.w) / 2;
    gameOverTextRect.y = 150;

    // Initialize game over score text
    font = TTF_OpenFont("assets/spaceranger.ttf", 32);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return;
    }
    string scoreText = "Score: " + to_string(score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!scoreSurface) {
        cout << "Lỗi tạo game over score surface: " << TTF_GetError() << endl;
        TTF_CloseFont(font);
        return;
    }
    gameOverScoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_FreeSurface(scoreSurface);
    SDL_QueryTexture(gameOverScoreTextTexture, nullptr, nullptr, &gameOverScoreTextRect.w, &gameOverScoreTextRect.h);
    gameOverScoreTextRect.x = (800 - gameOverScoreTextRect.w) / 2;
    gameOverScoreTextRect.y = 220; // Below "Game Over"

    vector<string> buttonLabels = {"Restart", "Back to Menu"};
    int buttonWidth = 250;
    int buttonHeight = 60;
    int startY = 300;
    int spacing = 20;

    font = TTF_OpenFont("assets/spaceranger.ttf", 32);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return;
    }

    gameOverButtons.clear();
    for (size_t i = 0; i < buttonLabels.size(); ++i) {
        Button button;
        button.rect.w = buttonWidth;
        button.rect.h = buttonHeight;
        button.rect.x = (800 - buttonWidth) / 2;
        button.rect.y = startY + i * (buttonHeight + spacing);
        button.label = buttonLabels[i];

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, button.label.c_str(), textColor);
        button.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        gameOverButtons.push_back(button);
    }
    TTF_CloseFont(font);
}

void Game::initWinner() {
    SDL_Color textColor = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 48);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Winner", textColor);
    winnerTextTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_QueryTexture(winnerTextTexture, nullptr, nullptr, &winnerTextRect.w, &winnerTextRect.h);
    winnerTextRect.x = (800 - winnerTextRect.w) / 2;
    winnerTextRect.y = 150;

    vector<string> buttonLabels = {"Back to Menu"};
    int buttonWidth = 260;
    int buttonHeight = 60;
    int startY = 300;

    font = TTF_OpenFont("assets/spaceranger.ttf", 32);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return;
    }

    winnerButtons.clear();
    for (size_t i = 0; i < buttonLabels.size(); ++i) {
        Button button;
        button.rect.w = buttonWidth;
        button.rect.h = buttonHeight;
        button.rect.x = (800 - buttonWidth) / 2;
        button.rect.y = startY + i * (buttonHeight + 20);
        button.label = buttonLabels[i];

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, button.label.c_str(), textColor);
        button.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        winnerButtons.push_back(button);
    }
    TTF_CloseFont(font);
}

void Game::initSkinSelection() {
    string imgPath = "assets/player" + to_string(selectedSkin) + ".png";
    SDL_Surface* skinSurface = IMG_Load(imgPath.c_str());
    if (skinSurface) {
        currentSkinTexture = SDL_CreateTextureFromSurface(renderer, skinSurface);
        SDL_FreeSurface(skinSurface);
    }

    skinPreviewRect.w = 40;
    skinPreviewRect.h = 40;
    skinPreviewRect.x = (800 - skinPreviewRect.w) / 2;
    skinPreviewRect.y = 250;

    vector<string> skinButtonLabels = {"<", ">", "Confirm"};
    int buttonWidth = 50;
    int buttonHeight = 40;
    int confirmButtonWidth = 100;
    int confirmButtonHeight = 50;
    int startY = 300;
    int totalButtonWidth = 2 * buttonWidth + 100;
    int leftX = (800 - totalButtonWidth) / 2;
    int rightX = leftX + buttonWidth + 100;

    SDL_Color textColor = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 24);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return;
    }

    skinButtons.clear();
    for (size_t i = 0; i < skinButtonLabels.size(); ++i) {
        Button button;
        if (skinButtonLabels[i] == "Confirm") {
            button.rect.w = confirmButtonWidth;
            button.rect.h = confirmButtonHeight;
            button.rect.x = (800 - confirmButtonWidth) / 2;
            button.rect.y = startY + buttonHeight + 20;
        } else {
            button.rect.w = buttonWidth;
            button.rect.h = buttonHeight;
            button.rect.x = (skinButtonLabels[i] == "<") ? leftX : rightX;
            button.rect.y = startY;
        }
        button.label = skinButtonLabels[i];

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, button.label.c_str(), textColor);
        button.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        skinButtons.push_back(button);
    }
    TTF_CloseFont(font);
}

void Game::resetGame() {
    score = 0;
    stage = 1;
    lastStarResetTime = SDL_GetTicks();
    lastEnemyShootTime = 0;

    // Update score text to 0
    TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 24);
    if (font) {
        SDL_Color textColor = {255, 255, 255, 255};
        string scoreText = "Score: " + to_string(score);
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        if (scoreSurface) {
            SDL_DestroyTexture(scoreTextTexture);
            scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
            SDL_FreeSurface(scoreSurface);
            SDL_QueryTexture(scoreTextTexture, nullptr, nullptr, &scoreTextRect.w, &scoreTextRect.h);
            scoreTextRect.x = 10;
            scoreTextRect.y = 10;
        }
        TTF_CloseFont(font);
    }

    delete player;
    player = new Player(renderer, selectedSkin);

    for (auto star : stars) {
        delete star;
    }
    stars.clear();
    stars.push_back(new Star(renderer));

    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    enemies.push_back(new Enemy(renderer, 1));

    for (auto bullet : roundBullets) {
        delete bullet;
    }
    roundBullets.clear();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        switch (currentState) {
            case MENU:
                handleMenuEvents(event);
                break;
            case PLAYING:
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    currentState = MENU;
                    if (musicOn && !Mix_PlayingMusic()) {
                        Mix_PlayMusic(menuMusic, -1);
                    }
                }
                break;
            case TUTORIAL:
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    currentState = MENU;
                }
                break;
            case SKIN_SELECTION:
                handleSkinSelectionEvents(event);
                break;
            case GAME_OVER:
                handleGameOverEvents(event);
                break;
            case WINNER:
                handleWinnerEvents(event);
                break;
        }
    }
    if (currentState == PLAYING) {
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        player->handleInput(keystates);
    }
}

void Game::handleMenuEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        for (size_t i = 0; i < buttons.size(); ++i) {
            SDL_Rect buttonRect = buttons[i].rect;
            if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
                mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
                if (musicOn) {
                    Mix_PlayChannel(-1, clickSound, 0);
                }
                if (buttons[i].label == "Play") {
                    currentState = PLAYING;
                    Mix_HaltMusic();
                    if (musicOn) Mix_PlayMusic(gameMusic, -1);
                } else if (buttons[i].label == "Music") {
                    musicOn = !musicOn;
                    if (!musicOn) {
                        Mix_HaltMusic();
                    } else {
                        if (currentState == MENU && !Mix_PlayingMusic()) {
                            Mix_PlayMusic(menuMusic, -1);
                        }
                    }
                } else if (buttons[i].label == "Tutorial") {
                    currentState = TUTORIAL;
                } else if (buttons[i].label == "Skin") {
                    currentState = SKIN_SELECTION;
                    initSkinSelection();
                }
            }
        }
    }
}

void Game::handleSkinSelectionEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        for (size_t i = 0; i < skinButtons.size(); ++i) {
            SDL_Rect buttonRect = skinButtons[i].rect;
            if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
                mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
                if (musicOn) {
                    Mix_PlayChannel(-1, clickSound, 0);
                }
                if (skinButtons[i].label == "<") {
                    selectedSkin = (selectedSkin == 1) ? 4 : selectedSkin - 1;
                    SDL_DestroyTexture(currentSkinTexture);
                    string imgPath = "assets/player" + to_string(selectedSkin) + ".png";
                    SDL_Surface* skinSurface = IMG_Load(imgPath.c_str());
                    if (skinSurface) {
                        currentSkinTexture = SDL_CreateTextureFromSurface(renderer, skinSurface);
                        SDL_FreeSurface(skinSurface);
                    }
                } else if (skinButtons[i].label == ">") {
                    selectedSkin = (selectedSkin == 4) ? 1 : selectedSkin + 1;
                    SDL_DestroyTexture(currentSkinTexture);
                    string imgPath = "assets/player" + to_string(selectedSkin) + ".png";
                    SDL_Surface* skinSurface = IMG_Load(imgPath.c_str());
                    if (skinSurface) {
                        currentSkinTexture = SDL_CreateTextureFromSurface(renderer, skinSurface);
                        SDL_FreeSurface(skinSurface);
                    }
                } else if (skinButtons[i].label == "Confirm") {
                    delete player;
                    player = new Player(renderer, selectedSkin);
                    currentState = MENU;
                    if (musicOn && !Mix_PlayingMusic()) {
                        Mix_PlayMusic(menuMusic, -1);
                    }
                }
            }
        }
    }
}

void Game::handleGameOverEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        for (size_t i = 0; i < gameOverButtons.size(); ++i) {
            SDL_Rect buttonRect = gameOverButtons[i].rect;
            if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
                mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
                if (musicOn) {
                    Mix_PlayChannel(-1, clickSound, 0);
                }
                if (gameOverButtons[i].label == "Restart") {
                    resetGame();
                    currentState = PLAYING;
                    Mix_HaltMusic();
                    if (musicOn) Mix_PlayMusic(gameMusic, -1);
                } else if (gameOverButtons[i].label == "Back to Menu") {
                    resetGame();
                    currentState = MENU;
                    Mix_HaltMusic();
                    if (musicOn) Mix_PlayMusic(menuMusic, -1);
                }
            }
        }
    }
}

void Game::handleWinnerEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        int mouseX = event.button.x;
        int mouseY = event.button.y;

        for (size_t i = 0; i < winnerButtons.size(); ++i) {
            SDL_Rect buttonRect = winnerButtons[i].rect;
            if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
                mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
                if (musicOn) {
                    Mix_PlayChannel(-1, clickSound, 0);
                }
                if (winnerButtons[i].label == "Back to Menu") {
                    resetGame();
                    currentState = MENU;
                    Mix_HaltMusic();
                    if (musicOn) Mix_PlayMusic(menuMusic, -1);
                }
            }
        }
    }
}

void Game::update() {
    if (currentState != PLAYING) return;

    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastStarResetTime >= STAR_RESET_INTERVAL) {
        if (!stars.empty()) {
            stars[0]->reset();
            lastStarResetTime = currentTime;
        }
    }

    player->update();

    for (auto star : stars) {
        star->update();

        SDL_Rect playerRect = player->getRect();
        SDL_Rect starRect = star->getRect();
        if (SDL_HasIntersection(&playerRect, &starRect)) {
            score++;
            star->reset();
            lastStarResetTime = currentTime;

            // Update score text
            TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 24);
            if (font) {
                SDL_Color textColor = {255, 255, 255, 255};
                string scoreText = "Score: " + to_string(score);
                SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
                if (scoreSurface) {
                    SDL_DestroyTexture(scoreTextTexture);
                    scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                    SDL_FreeSurface(scoreSurface);
                    SDL_QueryTexture(scoreTextTexture, nullptr, nullptr, &scoreTextRect.w, &scoreTextRect.h);
                    scoreTextRect.x = 10;
                    scoreTextRect.y = 10;
                }
                TTF_CloseFont(font);
            }

            if (score >= 4 && stage == 1) {
                stage = 2;
                if (!enemies.empty()) {
                    enemies[0]->flyUp();
                }
                enemies.push_back(new Enemy(renderer, 2));
            } else if (score >= 8 && stage == 2) {
                stage = 3;
                for (auto enemy : enemies) {
                    enemy->flyUp();
                }
                enemies.push_back(new Enemy(renderer, 3));
            } else if (score >= 14 && stage == 3) {
                if (!enemies.empty()) {
                    delete enemies.back();
                    enemies.pop_back();
                }
                currentState = WINNER;
                Mix_HaltMusic();
                if (musicOn) Mix_PlayMusic(gameMusic, -1);
            }
        }

        if (star->isOffScreen()) {
            // Không reset ngay
        }
    }

    for (auto enemy : enemies) {
        enemy->update();

        if (enemy->hasStoppedMoving() && currentTime - lastEnemyShootTime >= ENEMY_SHOOT_INTERVAL) {
            int shootType;
            if (stage == 3) {
                shootType = rand() % 3;
            } else if (stage == 2) {
                shootType = rand() % 3;
            } else {
                shootType = rand() % 2;
            }
            if (shootType == 0) {
                enemy->shootSpiralBullets(roundBullets);
            } else if (shootType == 1) {
                enemy->shootSpreadBullets(roundBullets);
            } else if (shootType == 2 && stage >= 2) {
                enemy->shootRandomBullets(roundBullets);
            }
            lastEnemyShootTime = currentTime;
        }
    }

    enemies.erase(
        remove_if(enemies.begin(), enemies.end(),
            [](Enemy* e) {
                if (e->isOffScreen()) {
                    delete e;
                    return true;
                }
                return false;
            }),
        enemies.end());

    for (auto bullet : roundBullets) {
        bullet->update();

        SDL_Rect bulletRect = bullet->getRect();
        SDL_Rect playerHitbox = player->getHitbox();
        if (SDL_HasIntersection(&bulletRect, &playerHitbox)) {
            cout << "Game Over! Bạn đã bị trúng đạn!" << endl;
            // Update game over score text
            TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 32);
            if (font) {
                SDL_Color textColor = {255, 255, 255, 255};
                string scoreText = "Score: " + to_string(score);
                SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
                if (scoreSurface) {
                    SDL_DestroyTexture(gameOverScoreTextTexture);
                    gameOverScoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
                    SDL_FreeSurface(scoreSurface);
                    SDL_QueryTexture(gameOverScoreTextTexture, nullptr, nullptr, &gameOverScoreTextRect.w, &gameOverScoreTextRect.h);
                    gameOverScoreTextRect.x = (800 - gameOverScoreTextRect.w) / 2;
                    gameOverScoreTextRect.y = 220;
                }
                TTF_CloseFont(font);
            }
            currentState = GAME_OVER;
            Mix_HaltMusic();
            if (musicOn) Mix_PlayMusic(menuMusic, -1);
        }
    }

    roundBullets.erase(
        remove_if(roundBullets.begin(), roundBullets.end(),
            [](RoundBullet* b) {
                if (b->isOffScreen()) {
                    delete b;
                    return true;
                }
                return false;
            }),
        roundBullets.end());
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    switch (currentState) {
        case MENU:
            renderMenu();
            break;
        case PLAYING:
            if (backgroundTexture) {
                SDL_RenderCopy(renderer, backgroundTexture, nullptr, &backgroundRect);
            }
            player->render();
            for (auto star : stars) {
                star->render();
            }
            for (auto enemy : enemies) {
                enemy->render();
            }
            for (auto bullet : roundBullets) {
                bullet->render();
            }
            // Render score text
            if (scoreTextTexture) {
                SDL_RenderCopy(renderer, scoreTextTexture, nullptr, &scoreTextRect);
            }
            break;
        case TUTORIAL:
            renderTutorial();
            break;
        case SKIN_SELECTION:
            renderSkinSelection();
            break;
        case GAME_OVER:
            renderGameOver();
            break;
        case WINNER:
            renderWinner();
            break;
    }

    SDL_RenderPresent(renderer);
}

void Game::renderMenu() {
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, &menuBackgroundRect);
    }

    for (const auto& button : buttons) {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &button.rect);
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &button.rect);

        int textW, textH;
        SDL_QueryTexture(button.texture, nullptr, nullptr, &textW, &textH);
        SDL_Rect textRect = {button.rect.x + (button.rect.w - textW) / 2, button.rect.y + (button.rect.h - textH) / 2, textW, textH};
        SDL_RenderCopy(renderer, button.texture, nullptr, &textRect);
    }
}

void Game::renderTutorial() {
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, &menuBackgroundRect);
    }

    SDL_Color textColor = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("assets/spaceranger.ttf", 32);
    if (!font) {
        cout << "Lỗi load font: " << TTF_GetError() << endl;
        return;
    }

    string tutorialText = "Game rules: Players use the arrow keys or WASD to steer their spaceship and dodge bullets fired from enemy ships and try to collect stars and win.";
    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, tutorialText.c_str(), textColor, 600);
    SDL_Texture* tutorialTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    int textW, textH;
    SDL_QueryTexture(tutorialTexture, nullptr, nullptr, &textW, &textH);
    SDL_Rect textRect = {(800 - textW) / 2, (600 - textH) / 2, textW, textH};
    SDL_RenderCopy(renderer, tutorialTexture, nullptr, &textRect);

    SDL_DestroyTexture(tutorialTexture);
    TTF_CloseFont(font);
}

void Game::renderSkinSelection() {
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, &menuBackgroundRect);
    }

    if (currentSkinTexture) {
        SDL_RenderCopy(renderer, currentSkinTexture, nullptr, &skinPreviewRect);
    }

    for (const auto& button : skinButtons) {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &button.rect);
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &button.rect);

        int textW, textH;
        SDL_QueryTexture(button.texture, nullptr, nullptr, &textW, &textH);
        SDL_Rect textRect = {button.rect.x + (button.rect.w - textW) / 2, button.rect.y + (button.rect.h - textH) / 2, textW, textH};
        SDL_RenderCopy(renderer, button.texture, nullptr, &textRect);
    }
}

void Game::renderGameOver() {
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, &menuBackgroundRect);
    }

    if (gameOverTextTexture) {
        SDL_RenderCopy(renderer, gameOverTextTexture, nullptr, &gameOverTextRect);
    }

    if (gameOverScoreTextTexture) {
        SDL_RenderCopy(renderer, gameOverScoreTextTexture, nullptr, &gameOverScoreTextRect);
    }

    for (const auto& button : gameOverButtons) {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &button.rect);
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &button.rect);

        int textW, textH;
        SDL_QueryTexture(button.texture, nullptr, nullptr, &textW, &textH);
        SDL_Rect textRect = {button.rect.x + (button.rect.w - textW) / 2, button.rect.y + (button.rect.h - textH) / 2, textW, textH};
        SDL_RenderCopy(renderer, button.texture, nullptr, &textRect);
    }
}

void Game::renderWinner() {
    if (menuBackgroundTexture) {
        SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, &menuBackgroundRect);
    }

    if (winnerTextTexture) {
        SDL_RenderCopy(renderer, winnerTextTexture, nullptr, &winnerTextRect);
    }

    for (const auto& button : winnerButtons) {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &button.rect);
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &button.rect);

        int textW, textH;
        SDL_QueryTexture(button.texture, nullptr, nullptr, &textW, &textH);
        SDL_Rect textRect = {button.rect.x + (button.rect.w - textW) / 2, button.rect.y + (button.rect.h - textH) / 2, textW, textH};
        SDL_RenderCopy(renderer, button.texture, nullptr, &textRect);
    }
}

void Game::clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    for (auto& button : buttons) {
        SDL_DestroyTexture(button.texture);
    }
    buttons.clear();
    for (auto& button : skinButtons) {
        SDL_DestroyTexture(button.texture);
    }
    skinButtons.clear();
    for (auto& button : gameOverButtons) {
        SDL_DestroyTexture(button.texture);
    }
    gameOverButtons.clear();
    for (auto& button : winnerButtons) {
        SDL_DestroyTexture(button.texture);
    }
    winnerButtons.clear();
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
    }
    if (menuBackgroundTexture) {
        SDL_DestroyTexture(menuBackgroundTexture);
    }
    if (currentSkinTexture) {
        SDL_DestroyTexture(currentSkinTexture);
    }
    if (gameOverTextTexture) {
        SDL_DestroyTexture(gameOverTextTexture);
    }
    if (winnerTextTexture) {
        SDL_DestroyTexture(winnerTextTexture);
    }
    if (scoreTextTexture) {
        SDL_DestroyTexture(scoreTextTexture);
    }
    if (gameOverScoreTextTexture) {
        SDL_DestroyTexture(gameOverScoreTextTexture);
    }
    if (menuMusic) {
        Mix_FreeMusic(menuMusic);
    }
    if (gameMusic) {
        Mix_FreeMusic(gameMusic);
    }
    if (clickSound) {
        Mix_FreeChunk(clickSound);
    }
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    delete player;
    for (auto star : stars) {
        delete star;
    }
    stars.clear();

    for (auto enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    for (auto bullet : roundBullets) {
        delete bullet;
    }
    roundBullets.clear();

    RoundBullet::unloadTexture();
}

bool Game::running() {
    return isRunning;
}

void Game::run() {
    while (running()) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}
