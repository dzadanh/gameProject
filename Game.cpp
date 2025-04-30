#include "game.h"
#include "player.h"
#include <bits/stdc++.h>
using namespace std;

Player* player = nullptr;

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
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Xoá nền
    SDL_RenderClear(renderer);

    player->render();

    // Tạm thời chưa có gì để vẽ

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    delete player;
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
