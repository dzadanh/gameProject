#include "game.h"
#include<bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init("Asteroid Drift", 800, 600)) {
        cout << "Không thể khởi tạo game!\n";
        return -1;
    }

    game.run();
    game.clean();

    return 0;
}
