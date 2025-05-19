#include "explosion.h"
#include <iostream>
using namespace std;

Explosion::Explosion(SDL_Renderer* renderer, int x, int y)
    : renderer(renderer), texture(nullptr), frame(0), maxFrames(120), scale(1.0f) {
    SDL_Surface* surface = IMG_Load("assets/explosion.png");
    if (!surface) {
        std::cout << "Lỗi load ảnh explosion: " << IMG_GetError() << std::endl;
        return;
    }
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    rect.w = 60; // Kích thước ban đầu
    rect.h = 60;
    rect.x = x - rect.w / 2; // Căn giữa tại vị trí (x, y)
    rect.y = y - rect.h / 2;
}

Explosion::~Explosion() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Explosion::update() {
    frame++; // Tăng frame mỗi lần update

    // Giảm tỷ lệ kích thước (scale) tuyến tính từ 1.0 xuống 0
    scale = 1.0f - static_cast<float>(frame) / maxFrames;
    if (scale < 0) scale = 0;

    // Cập nhật kích thước
    int newWidth = static_cast<int>(60 * scale); // Kích thước ban đầu là 60
    int newHeight = static_cast<int>(60 * scale);

    // Căn giữa lại bằng cách điều chỉnh x, y dựa trên kích thước mới
    rect.x = (rect.x + (rect.w / 2)) - (newWidth / 2);
    rect.y = (rect.y + (rect.h / 2)) - (newHeight / 2);
    rect.w = newWidth;
    rect.h = newHeight;
}

void Explosion::render() {
    // Render texture với kích thước mới
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

bool Explosion::isFinished() const {
    return frame >= maxFrames;
}
