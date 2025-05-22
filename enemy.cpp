#include "enemy.h"
#include "round_bullet.h"
#include <bits/stdc++.h>
using namespace std;

Enemy::Enemy(SDL_Renderer* renderer, int stage)
    : renderer(renderer), texture(nullptr), speed(2), hasStopped(false), isFlyingUp(false), stage(stage) {
    string imgPath = "assets/enemy" + to_string(stage) + ".png";
    SDL_Surface* surface = IMG_Load(imgPath.c_str());
    if (!surface) {
        std::cout << "Không thể load ảnh enemy: " << IMG_GetError() << std::endl;
    } else {
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    rect.w = 90;
    rect.h = 90;
    rect.x = 368;  // Giữa màn hình 800x600
    rect.y = -rect.h; // Xuất hiện từ trên
}

Enemy::~Enemy() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Enemy::update() {
    if (isFlyingUp) {
        rect.y -= speed * 2; // Bay lên nhanh hơn
    } else if (!hasStopped) {
        rect.y += speed;
        if (rect.y >= 100) {
            rect.y = 100;
            hasStopped = true;
        }
    }
}

void Enemy::shootSpiralBullets(vector<RoundBullet*>& bullets) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h / 2.0f;
    static float spiralAngle = 0;

    int bulletCount = 5 + stage * 5;// Số lượng đạn cho spiral
    float speed = 1.4f + stage * 0.5f; // Tốc độ đạn cho spiral

    for (int i = 0; i < bulletCount; i++) {
        float angle = spiralAngle + (2 * M_PI / bulletCount) * i;
        bullets.push_back(new RoundBullet(renderer, cx, cy, angle, speed));
    }

    spiralAngle += 0.1f;
}

void Enemy::shootSpreadBullets(std::vector<RoundBullet*>& bullets) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h;

    int bulletCount = 4 + stage * 4; // Số lượng đạn cho spread
    float spreadAngle = M_PI;
    float startAngle = M_PI / 2 - spreadAngle / 2;
    float speed = 1.4f + stage * 0.5f; // Tốc độ đạn cho spread

    for (int i = 0; i < bulletCount; i++) {
        float angle = startAngle + i * (spreadAngle / (bulletCount - 1));
        bullets.push_back(new RoundBullet(renderer, cx, cy, angle, speed));
    }
}

void Enemy::shootRandomBullets(std::vector<RoundBullet*>& bullets) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h;

    int bulletCount = 5; // Số lượng đạn cho random
    float speed = 1.7f + stage * 0.5f; // Tốc độ đạn cho random
    float angle = (rand() % 360) * M_PI / 180.0f; // Chọn một góc ngẫu nhiên
    float spacing = 40.0f; // Khoảng cách giữa các viên đạn

    // Góc vuông góc với hướng bắn để xếp đạn
    float perpendicularAngle = angle + M_PI / 2;

    for (int i = 0; i < bulletCount; i++) {
        // Tính vị trí ban đầu, xếp ngang vuông góc với hướng bắn
        float offset = (i - (bulletCount - 1) / 2.0f) * spacing; // Căn giữa
        float bulletX = cx + offset * cos(perpendicularAngle);
        float bulletY = cy + offset * sin(perpendicularAngle);
        bullets.push_back(new RoundBullet(renderer, bulletX, bulletY, angle, speed));
    }
}

void Enemy::render() {
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

SDL_Rect Enemy::getRect() {
    return rect;
}

bool Enemy::hasStoppedMoving() {
    return hasStopped && !isFlyingUp;
}

bool Enemy::isOffScreen() {
    return rect.y < -rect.h;
}

void Enemy::flyUp() {
    isFlyingUp = true;
    hasStopped = false;
}
