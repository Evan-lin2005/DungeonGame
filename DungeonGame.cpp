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
    // ���J���إߪ��a
    if (std::ifstream(kPlayerSave)) {
        hero_ = loadPlayer(kPlayerSave);
    }
    else {
        std::cerr << "Error(Not Fond Characeter File)" << std::endl;
        return;
    }
    // �Ъ��a�ޯ�
    learnPlayerSkill(hero_, kSkillData);
    hero_.setbattleskill(0); hero_.setbattleskill(1);
    hero_.setbattleskill(2); hero_.setbattleskill(3);

    mgr_ = Manager(WIDTH, HEIGHT, hero_);

    // ���J�ĤH��Ʀ�
	pool_ = loadEnemies(kEnemyData);
    if (pool_.empty()) {
        std::cerr << "[ERROR] �L�kŪ�� " << kEnemyData << "\n";
        std::exit(1);
    }
    // ���J Boss
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
        std::cout << "[w a s d ����],[e �˵��ޯ�],[f �˵��˳�] ,[q ���}�C��] "
           <<"�a�c�h��" << floor_ << " / " << MAX_FLOOR << ")\n";
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
                std::cout << "=====================ĵ�i!!!�X�{�j�P�Ĺ����!!!=====================" << std::endl;
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
            // �C�h�ĤH�ƶq�i�� floor_ �վ�
            mgr_.spawnEnemies(pool_, 5 + floor_,floor_);
            mgr_.spawnMerchants(rand() % 5 + 1, KMerchantData);
			mgr_.putTextureBox(rand() % 5 + 1);
        }
    }

    if (victory_) {
        std::cout << "\n���߳q���I\n";
    }
}

