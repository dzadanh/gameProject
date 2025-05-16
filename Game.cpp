#include "game.h"
#include "player.h"
#include "star.h"
#include "enemy.h"
#include "round_bullet.h"
#include "explosion.h"
#include <bits/stdc++.h>
using namespace std;

Player* player = nullptr;
vector<Star*> stars;
vector<Enemy*> enemies;
int score = 0;
int stage = 1;

Uint32 lastStarResetTime = 0;
const Uint32 STAR_RESET_INTERVAL = 5000;

vector<RoundBullet*> roundBullets;
Uint32 lastEnemyShootTime = 0;
const Uint32 ENEMY_SHOOT_INTERVAL = 1000;

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL không thể khởi tạo! Lỗi: " << SDL_GetError() << endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "SDL_image lỗi: " << IMG_GetError() << endl;
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

    player = new Player(renderer);
    stars.push_back(new Star(renderer));
    lastStarResetTime = SDL_GetTicks();

    enemies.push_back(new Enemy(renderer, 1));

    RoundBullet::loadTexture(renderer);

    isRunning = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
    }
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    player->handleInput(keystates);
}

void Game::update() {
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

            if (score >= 4 && stage == 1) {
                stage = 2;
                if (!enemies.empty()) {
                    enemies[0]->flyUp(); // Enemy1 bay lên
                }
                enemies.push_back(new Enemy(renderer, 2));
            } else if (score >= 10 && stage == 2) {
                stage = 3;
                for (auto enemy : enemies) {
                    enemy->flyUp(); // Tất cả enemy hiện tại bay lên
                }
                enemies.push_back(new Enemy(renderer, 3));
            } else if (score >= 14 && stage == 3) {
                cout << "Chúc mừng! Bạn đã chiến thắng!" << endl;
                if (!enemies.empty()) {
                    SDL_Rect enemyRect = enemies.back()->getRect();
                    explosions.push_back(new Explosion(renderer, enemyRect.x + enemyRect.w / 2, enemyRect.y + enemyRect.h / 2));
                    delete enemies.back();
                    enemies.pop_back();
                }
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
                shootType = rand() % 4; // Enemy3 bắn cả 4 kiểu
            } else if (stage == 2) {
                shootType = rand() % 3; // Enemy2 bắn 3 kiểu
            } else {
                shootType = rand() % 2; // Enemy1 bắn 2 kiểu
            }
            if (shootType == 0) {
                enemy->shootSpiralBullets(roundBullets);
            } else if (shootType == 1) {
                enemy->shootSpreadBullets(roundBullets);
            } else if (shootType == 2 && stage >= 2) {
                enemy->shootRandomBullets(roundBullets);
            } else if (shootType == 3 && stage >= 3) {
                enemy->shootSpiralWaveBullets(roundBullets);
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
        SDL_Rect playerHitbox = player->getHitbox(); // Dùng hitbox thay vì rect
        if (SDL_HasIntersection(&bulletRect, &playerHitbox)) {
            cout << "Game Over! Bạn đã bị trúng đạn!" << endl;
            isRunning = false;
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

    for (auto explosion : explosions) {
        explosion->update();
    }
    explosions.erase(
        remove_if(explosions.begin(), explosions.end(),
            [](Explosion* e) {
                if (e->isFinished()) {
                    delete e;
                    return true;
                }
                return false;
            }),
        explosions.end());
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

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
    for (auto explosion : explosions) {
        explosion->render();
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
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

    for (auto explosion : explosions) {
        delete explosion;
    }
    explosions.clear();

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
