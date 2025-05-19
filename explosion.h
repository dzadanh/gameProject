#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SDL.h>
#include <SDL_image.h>

class Explosion {
public:
    Explosion(SDL_Renderer* renderer, int x, int y);
    ~Explosion();
    void update();
    void render();
    bool isFinished() const;

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    int frame;          // Đếm frame để kiểm soát thời gian
    int maxFrames;      // Tổng số frame trước khi vụ nổ kết thúc
    float scale;        // Tỷ lệ kích thước (giảm dần)
};

#endif
