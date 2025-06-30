// DungeonGame.cpp
#include "DungeonGame.h"

DungeonGame::DungeonGame(int width, int height, int maxFloor)
    : WIDTH(width), HEIGHT(height), MAX_FLOOR(maxFloor),
    floor_(1), victory_(false),
    hero_("Hero"), mgr_(width, height, hero_)
{
    if (maxFloor > 6) {
        mgr_.Shouldadjust();
    }
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
    boss_ = loadEnemies(kBossData);
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
            srand(time(NULL));
            if (floor_ == MAX_FLOOR) {
                system("cls");
                std::cout << "=====================警告!!!出現強烈敵對反應!!!=====================" << std::endl;
                system("pause");
                if (MAX_FLOOR>6) {
                    for (auto& boss : boss_) {
                        boss.CritizeByPlayerLv(mgr_.getPlayer().getlv());
                    }
                }
                victory_ = Battle(mgr_.getPlayer(), boss_[rand() % boss_.size()]);
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

