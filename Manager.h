// Manager.h
#pragma once
#include "Character.h"
#include "Battle.h"
#include "MapGenerator.h"
#include "Merchant.h"
#include "treasurebox.h"
#include <set>
#include <utility>
#include <vector>

struct EnemyInMap {
    Enemy enemy;
    std::pair<int, int> pos;
    static std::set<std::pair<int, int>> allPos;
    EnemyInMap(const Enemy& e, std::pair<int, int> p) : enemy(e), pos(p) {}
};


struct MerchantInMap {
    Merchant merchant;
    std::pair<int, int> pos;
    static std::set<std::pair<int, int>> allPos;
};

struct TreasureBoxInMap {
    TreasureBox treasureBox;
    std::pair<int, int> pos;
    static std::set<std::pair<int, int>> allPos;
};

class Manager {
public:
    Manager(int width, int height, const Player& p);
    Manager(int width, int height);

    void spawnEnemies(const std::vector<Enemy>& prototypes, int maxNum,int floor);
    void spawnMerchants(int count, const std::string& merchantDataPath);
    void nextMap();
    void moveMainCharacter(int dir);
    void operateSkill();
    void operateEquip();
    void putTextureBox(int numbers);
    void IsOpenTreasureBox();

    bool isGameOver() const;
    bool reachedStairs() const;
    void printMap() const;
    Player& getPlayer();
    const Player& getPlayer() const;
    void Shouldadjust();

private:
    void battleIfNeeded();
    void interactIfMerchant();
    std::pair<int, int> pickRandomFloor() const;
    bool addjust;

    MapGenerator maps;
    std::vector<EnemyInMap> enemies;
    std::vector<MerchantInMap> merchants;
	std::vector<TreasureBoxInMap> treasureBoxes;
    Player player;
    std::pair<int, int> playerPos;
    bool gameOver = false;
    bool atStairs = false;
    static std::set<std::pair<int, int>> allsearchPos;
};




