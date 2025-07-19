// Game.cpp
#include "DungeonGame.h"
#include "GameAsk.h"
bool g_useSFMLBattle = true;
int main() {
    srand(time(NULL));
    int h; int w; int floor;
	GameAskSFML(h, w, floor);
    std::cout << "Start" << std::endl;
    system("pause");
    DungeonGame game(h,w,floor);
    game.runSFML();
    return 0;
}
