// Game.cpp
#include "DungeonGame.h"
#include "GameAsk.h"
int main() {
    int h; int w; int floor;
    GameAsk(h, w, floor);
    std::cout << "¶i¤J¹CÀ¸" << std::endl;
    system("paused");
    DungeonGame game(h,w,floor);
    game.run();
    return 0;
}
