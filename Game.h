#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include "player.h"

extern Player* player;

class Game {
public:
    Game() : window(nullptr), renderer(nullptr), isRunning(false) {}

    bool init(const char* title, int width, int height); // Khởi tạo SDL, window, renderer
    void handleEvents();   // Bắt sự kiện (như thoát game)
    void update();         // Cập nhật logic game
    void render();         // Vẽ game
    void clean();          // Giải phóng tài nguyên
    bool running();        // Trả về trạng thái game còn chạy không
    void run();            // Vòng lặp game chính

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool isRunning;
};

#endif
