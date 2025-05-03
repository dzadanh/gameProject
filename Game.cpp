#include "game.h"
#include "player.h"
#include "star.h"
#include <bits/stdc++.h>
using namespace std;

Player* player = nullptr;

vector<Star*> stars;
int score = 0;

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

    for (int i = 0; i < 5; ++i) {
        stars.push_back(new Star(renderer));
    }


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
    // Tạm thời chưa có gì
    player->update();

    for (auto star : stars) {
        star->update();

        // Kiểm tra va chạm
        SDL_Rect playerRect = player->getRect();
        SDL_Rect starRect = star->getRect();
        if (SDL_HasIntersection(&playerRect, &starRect)) {
            score++;
            star->reset();
        }


        // Nếu sao rơi quá màn thì reset
        if (star->isOffScreen()) {
            star->reset();
        }
    }

}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Xoá nền
    SDL_RenderClear(renderer);

    player->render();

    for (auto star : stars) {
        star->render();
    }


    // Tạm thời chưa có gì để vẽ

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    delete player;

    for (auto star : stars){
        delete star;
    }
    stars.clear();
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
