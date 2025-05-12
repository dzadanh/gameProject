#include "game.h"
#include "player.h"
#include "star.h"
#include "enemy.h"
#include <bits/stdc++.h>
using namespace std;

Player* player = nullptr;
vector<Star*> stars;
int score = 0;

Uint32 lastStarResetTime = 0;
const Uint32 STAR_RESET_INTERVAL = 5000; // 5 giây

Enemy* enemy = nullptr;

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

    // Tạo một ngôi sao duy nhất
    stars.push_back(new Star(renderer));
    lastStarResetTime = SDL_GetTicks();

    enemy = new Enemy(renderer);

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

    // Reset sao sau mỗi 3 giây
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
            lastStarResetTime = currentTime; // reset lại thời gian đếm 3s tiếp theo
        }

        if (star->isOffScreen()) {
            // Nếu rơi quá màn hình thì không reset ngay — để giữ thời gian cách nhau 3s
            // hoặc có thể reset để cho rơi lại ngay:
            // star->reset();
        }
    }

    enemy->update();

}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Xoá nền
    SDL_RenderClear(renderer);

    player->render();

    for (auto star : stars) {
        star->render();
    }

    enemy->render();


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

    delete enemy;

}

bool Game::running() {
    return isRunning;
}

void Game::run() {
    while (running()) {
        handleEvents();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }
}
