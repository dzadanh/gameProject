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

    int bulletCount = 14 + stage * 2;
    float speed = 1.4f + stage * 0.2f;

    for (int i = 0; i < bulletCount; i++) {
        float angle = spiralAngle + (2 * M_PI / bulletCount) * i;
        bullets.push_back(new RoundBullet(renderer, cx, cy, angle, speed));
    }

    spiralAngle += 0.1f;
}

void Enemy::shootSpreadBullets(std::vector<RoundBullet*>& bullets) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h;

    int bulletCount = 10 + stage * 2;
    float spreadAngle = M_PI;
    float startAngle = M_PI / 2 - spreadAngle / 2;
    float speed = 1.4f + stage * 0.2f;

    for (int i = 0; i < bulletCount; i++) {
        float angle = startAngle + i * (spreadAngle / (bulletCount - 1));
        bullets.push_back(new RoundBullet(renderer, cx, cy, angle, speed));
    }
}

void Enemy::shootRandomBullets(std::vector<RoundBullet*>& bullets) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h;

    int bulletCount = 5;
    float speed = 1.4f + stage * 0.2f;
    float angle = (rand() % 360) * M_PI / 180.0f; // Chọn một góc ngẫu nhiên
    float spacing = 10.0f; // Khoảng cách giữa các viên đạn

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

void Enemy::shootSpiralWaveBullets(std::vector<RoundBullet*>& bullets) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h / 2.0f;
    static float waveAngle = 0.0f; // Góc ban đầu

    int bulletCount = 20 + stage * 5; // Tăng số lượng đạn để tạo hiệu ứng xoắn ốc rõ hơn
    float speed = 1.4f + stage * 0.2f;
    const float phi = (1.0f + sqrt(5.0f)) / 2.0f; // Tỉ lệ vàng
    const float scaleFactor = 0.1f; // Hằng số điều chỉnh kích thước xoắn ốc
    const float angleStep = M_PI / 8.0f; // Bước góc để tạo các vòng

    for (int i = 0; i < bulletCount; i++) {
        // Tính góc dựa trên waveAngle
        float theta = waveAngle + i * angleStep;
        // Tính bán kính theo xoắn ốc vàng: r = a * e^(b * theta), với b liên quan đến phi
        float radius = scaleFactor * exp((log(phi) / (2.0f * M_PI)) * theta);

        // Tính tọa độ ban đầu dựa trên xoắn ốc
        float bulletX = cx + radius * cos(theta);
        float bulletY = cy + radius * sin(theta);

        // Tạo viên đạn với hướng di chuyển dựa trên góc theta
        bullets.push_back(new RoundBullet(renderer, bulletX, bulletY, theta, speed));
    }

    waveAngle += angleStep / 10.0f; // Tăng góc chậm để tạo hiệu ứng xoắn ốc liên tục
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
