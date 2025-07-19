// DungeonGame.h
#pragma once

#include "Manager.h"
#include "DataLoader.h"
#include "Battle.h"

#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>

class DungeonGame {
public:
    DungeonGame(int width, int height, int maxFloor = 3);
    void run();
    void runSFML();

private:
    void initData();
    void saveProgress() const;

    const int WIDTH, HEIGHT, MAX_FLOOR;
    int floor_;
    bool victory_;

    Player hero_;
    std::vector<Enemy> pool_;
    std::vector<Enemy> boss_;

    Manager mgr_;

    static constexpr const char* kPlayerSave = "player_save.txt";
    static constexpr const char* kEnemyData = "enemies.txt";
    static constexpr const char* kBossData = "boss.txt";
	static constexpr const char* kSkillData = "Skill.txt";
    static constexpr const char* KMerchantData = "merchant_data.txt";
	static constexpr const char* kTreasureBoxData = "treasure_data.txt";
};


