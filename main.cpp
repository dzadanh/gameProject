#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // 1. Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Không thể khởi tạo SDL! Lỗi: " << SDL_GetError() << "\n";
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cout << "Lỗi khi khởi tạo SDL_image: " << IMG_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // 2. Tạo cửa sổ game
    SDL_Window* window = SDL_CreateWindow(
        "Asteroid Drift",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (!window) {
        cout << "Không tạo được cửa sổ! Lỗi: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // 3. Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Không tạo được renderer! Lỗi: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // 4. Load texture background
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Surface* bgSurface = IMG_Load("background.png");
    if (!bgSurface) {
        cout << "Không load được ảnh nền: " << IMG_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);

    // 5. Load texture phi thuyền
    SDL_Texture* spaceshipTexture = nullptr;
    SDL_Rect spaceshipRect = {0, 0, 64, 64}; // Kích thước hiển thị
    float spaceshipPosX = SCREEN_WIDTH / 2.0f;
    float spaceshipPosY = SCREEN_HEIGHT / 2.0f;

    SDL_Surface* tempSurface = IMG_Load("spaceship.png");
    if (!tempSurface) {
        cout << "Không load được ảnh phi thuyền: " << IMG_GetError() << endl;
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }
    spaceshipTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // 6. Vòng lặp game
    bool isRunning = true;
    SDL_Event event;
    const float speed = 0.05;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        // Điều khiển phi thuyền
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_UP])    spaceshipPosY -= speed;
        if (keystates[SDL_SCANCODE_DOWN])  spaceshipPosY += speed;
        if (keystates[SDL_SCANCODE_LEFT])  spaceshipPosX -= speed;
        if (keystates[SDL_SCANCODE_RIGHT]) spaceshipPosX += speed;

        // Giới hạn không ra khỏi màn hình
        if (spaceshipPosX < 0) spaceshipPosX = 0;
        if (spaceshipPosY < 0) spaceshipPosY = 0;
        if (spaceshipPosX + spaceshipRect.w > SCREEN_WIDTH)
            spaceshipPosX = SCREEN_WIDTH - spaceshipRect.w;
        if (spaceshipPosY + spaceshipRect.h > SCREEN_HEIGHT)
            spaceshipPosY = SCREEN_HEIGHT - spaceshipRect.h;

        // Cập nhật vị trí phi thuyền
        spaceshipRect.x = (int)spaceshipPosX;
        spaceshipRect.y = (int)spaceshipPosY;

        // Render
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);          // Vẽ background full màn hình
        SDL_RenderCopy(renderer, spaceshipTexture, NULL, &spaceshipRect); // Vẽ phi thuyền
        SDL_RenderPresent(renderer);
    }

    // 7. Giải phóng tài nguyên
    SDL_DestroyTexture(spaceshipTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
