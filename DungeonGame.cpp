// DungeonGame.cpp
#include "DungeonGame.h"

DungeonGame::DungeonGame(int width, int height, int maxFloor)
    : WIDTH(width), HEIGHT(height), MAX_FLOOR(maxFloor),
    floor_(1), victory_(false),
    hero_("Hero"), mgr_(width, height, hero_), boss_("Boss", "Dark", 1000000, 10000, 12400, 18000, 9999999, 10,7)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    initData();
	mgr_.spawnEnemies(pool_, rand()%(20),floor_);
}

void DungeonGame::initData() {
    // 載入／建立玩家
    if (std::ifstream(kPlayerSave)) {
        hero_ = loadPlayer(kPlayerSave);
    }
    else {
        std::cerr << "Error(Not Fond Characeter File)" << std::endl;
        return;
    }
    // 教玩家技能
    learnPlayerSkill(hero_, kSkillData);
    hero_.setbattleskill(0); hero_.setbattleskill(1);
    hero_.setbattleskill(2); hero_.setbattleskill(3);

    mgr_ = Manager(WIDTH, HEIGHT, hero_);

    // 載入敵人資料池
	pool_ = loadEnemies(kEnemyData);
    if (pool_.empty()) {
        std::cerr << "[ERROR] 無法讀取 " << kEnemyData << "\n";
        std::exit(1);
    }
    // 載入 Boss
    boss_ = Enemy("Boss","Dark(weak)", 500, 200, 40, 20, 0, 0,7);
    auto tmp = loadEnemies(kBossData);
    if (!tmp.empty()) boss_ = tmp.front();
    boss_.learnskill(Skill("魔神化","短暫使自身化身魔神",150.f,3,150,2));
	boss_.learnskill(Skill("魔神之怒", "對敵人造成大量傷害", 5.0f, 0, 300, 2));
	loadTreasureBox(kTreasureBoxData);

    mgr_.spawnMerchants(rand() % 5 + 1, KMerchantData);

}

void DungeonGame::saveProgress() const {
    savePlayer(mgr_.getPlayer(), kPlayerSave);
}

void DungeonGame::run() {
    while (true) {
        clearScreen();
        mgr_.printMap();
        std::cout << "[w a s d 移動],[e 檢視技能],[f 檢視裝備] ,[q 離開遊戲] "
           <<"地牢層數" << floor_ << " / " << MAX_FLOOR << ")\n";
        char c; std::cin >> c;
		c = std::tolower(c);
        if (c == 'q') { saveProgress(); break; }
        else if (c == 'e') {
            mgr_.operateSkill();
        }
        else if (c == 'f') {
            mgr_.operateEquip();
        }
        int dir = (c == 'a' ? 0 : c == 'w' ? 1 : c == 'd' ? 2 : c == 's' ? 3 : -1);
        if (dir != -1) mgr_.moveMainCharacter(dir);

        if (mgr_.isGameOver()) {
            std::cout << "Game Over\n";
            saveProgress();
            break;
        }

        if (mgr_.reachedStairs()) {
            if (floor_ == MAX_FLOOR) {
                victory_ = Battle(mgr_.getPlayer(), boss_);
                saveProgress();
                break;
            }
            ++floor_;
            mgr_.nextMap();
            // 每層敵人數量可依 floor_ 調整
            mgr_.spawnEnemies(pool_, 5 + floor_,floor_);
            mgr_.spawnMerchants(rand() % 5 + 1, KMerchantData);
			mgr_.putTextureBox(rand() % 5 + 1);
        }
    }

    if (victory_) {
        std::cout << "\n恭喜通關！\n";
    }
}

