#include<SDL.h>
#include<bits/stdc++.h>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main(int argc, char* argv[]){
    //1.Khởi tạo SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        cout<<"không thể khởi tạo SDL! Lỗi: "<<SDL_GetError()<<"\n";
        return -1;
    }

    //2. Tạo cửa sổ game
    SDL_Window* window = SDL_CreateWindow(
        "Asteroid Drift",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN
    );
    if(!window){
        cout<<"không tạo được cửa sổ! Lỗi: "<<SDL_GetError()<<"\n";
        SDL_Quit();
        return -1;
    }

    //3. Tạo renderer để vẽ
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer){
        cout<<"không tạo được renderer! Lỗi: "<<SDL_GetError()<<"\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    //4. Vòng lặp game
    bool isRunning = true;
    SDL_Event event;
    while(isRunning){
        // Xử lý sự kiện (bấm nút X để thoát)
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                isRunning = 0;
            }
        }

        // Làm sạch renderer với màu đen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // R,G,B,A = 0,0,0,255 = đen
        SDL_RenderClear(renderer);

        // Sau này sẽ vẽ ở đây

        // Hiển thị nội dung renderer ra màn hình
        SDL_RenderPresent(renderer);
    }

    //5. Giải phòng tài nguyên
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
