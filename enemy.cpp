#include "enemy.h"
#include "round_bullet.h"
#include <bits/stdc++.h>
using namespace std;

Enemy::Enemy(SDL_Renderer* renderer)
    : renderer(renderer), texture(nullptr), speed(2), hasStopped(false) {
    SDL_Surface* surface = IMG_Load("assets/enemy1.png");
    if (!surface) {
        std::cout << "Không thể load ảnh enemy: " << IMG_GetError() << std::endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    rect.w = 90;
    rect.h = 90;
    rect.x = 368;  // Giữa màn hình 800x600
    rect.y = -rect.h; // Xuất hiện từ trên trời rơi xuống
}

Enemy::~Enemy() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Enemy::update() {
    if (!hasStopped) {
        rect.y += speed;
        if (rect.y >= 100) {  // Cho đứng yên tại vị trí này
            rect.y = 100;
            hasStopped = true;
        }
    }
}

void Enemy::render() {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

// Spiral shots
void Enemy::shootSpiralBullets(vector<RoundBullet*>& bullets) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h / 2.0f;
    static float spiralAngle = 0;

    const int bulletCount = 14;
    const float speed = 1.4f;

    for (int i = 0; i < bulletCount; i++) {
        float angle = spiralAngle + (2 * M_PI / bulletCount) * i;
        bullets.push_back(new RoundBullet(renderer, cx, cy, angle, speed));
    }

    spiralAngle += 0.1f;  // xoay dần từng đợt
}

// Spread shots
void Enemy::shootSpreadBullets(std::vector<RoundBullet*>& bullets) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h;

    const int bulletCount = 10;
    const float spreadAngle = M_PI;  // góc 180 độ
    const float startAngle = M_PI / 2 - spreadAngle / 2;
    const float speed = 1.4f;

    for (int i = 0; i < bulletCount; i++) {
        float angle = startAngle + i * (spreadAngle / (bulletCount - 1));
        bullets.push_back(new RoundBullet(renderer, cx, cy, angle, speed));
    }
}


SDL_Rect Enemy::getRect() {
    return rect;
}

bool Enemy::hasStoppedMoving() {
    return hasStopped;
}
