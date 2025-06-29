// Manager.cpp
#include "Manager.h"
#include "DataLoader.h"  // 提供 LoadMerchantData
#include <algorithm>
#include <cstdlib>
#include <ctime>


static int randInt(int a, int b) { return a + std::rand() % (b - a + 1); }
std::set<std::pair<int, int>> Manager::allsearchPos;
std::set<std::pair<int, int>> EnemyInMap::allPos;
std::set<std::pair<int, int>> MerchantInMap::allPos;
std::set<std::pair<int, int>> TreasureBoxInMap::allPos;
const int dirs[4][2] = {//wasd
    { -1,0  }, 
    { 0, -1 },  
    { 1, 0 }, 
    { 0, 1 } 
};

inline void showTreasureBox(TreasureBoxInMap& it) {
    system("cls");
    bool hasLoot = false;

    for (auto& mat : it.treasureBox.ecMaterial) {
        std::cout << "獲得了: Material"
            << mat.getName() << "(" << mat.getDesc() << ")" << "\n";
        hasLoot = true;
    }
    for (auto& eq : it.treasureBox.ecEquip) {
        std::cout << "獲得了: Equipment"
            << eq.getName() << "(" << eq.getDesc() << ")" << "\n";
        hasLoot = true;
    }
    for (auto& mi : it.treasureBox.ecMiseryItem) {
        std::cout << "獲得了: MiseryItem"
            << mi.getName() << "(" << mi.getDesc() << ")" << "\n";
        hasLoot = true;
    }

    if (!hasLoot) {
        std::cout << "寶箱是空的，什麼都沒找到...\n";
    }
    system("pause");
}



Manager::Manager(int w, int h)
    : maps(w, h), player("Hero") {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    playerPos = pickRandomFloor();
    allsearchPos.clear();
    // 初始化探索範圍 3x3
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int x = playerPos.first + dx;
            int y = playerPos.second + dy;
            if (x >= 0 && x < w && y >= 0 && y < h)
                allsearchPos.insert({ x, y });
        }
    }
}

Manager::Manager(int w, int h, const Player& p)
    : maps(w, h), player(p) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    playerPos = pickRandomFloor();
    allsearchPos.clear();
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int x = playerPos.first + dx;
            int y = playerPos.second + dy;
            if (x >= 0 && x < w && y >= 0 && y < h)
                allsearchPos.insert({ x, y });
        }
    }
}

std::pair<int, int> Manager::pickRandomFloor() const {
    const auto& grid = maps.getMap();
    int H = grid.size();
    int W = grid[0].size();
    std::vector<std::pair<int, int>> floors;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            if (grid[y][x] == FLOOR)
                floors.emplace_back(x, y);
        }
    }
    if (floors.empty()) return { 0, 0 };
    return floors[randInt(0, floors.size() - 1)];
}


void Manager::spawnEnemies(const std::vector<Enemy>& prototypes, int maxNum, int floor) {
    const auto& grid = maps.getMap();
    int H = grid.size(), W = grid[0].size();
    int tries = 0;

    // 建立樓層依賴的抽取池
    std::vector<const Enemy*> weightedPool;
    for (const auto& e : prototypes) {
        // 樓層越高，rank 高的敵人機率就越大
        int weight = std::max(0, (floor + 2) - e.getrank() * 2);
        for (int i = 0; i < weight; ++i) {
            weightedPool.push_back(&e);
        }
    }
    if (weightedPool.empty()) return;

    while ((int)enemies.size() < maxNum && ++tries < 10000) {
        int x = std::rand() % W;
        int y = std::rand() % H;
        auto pos = std::make_pair(x, y);
        if (grid[y][x] != FLOOR || EnemyInMap::allPos.count(pos) || pos == playerPos)
            continue;

        const Enemy& proto = *weightedPool[std::rand() % weightedPool.size()];
        Enemy scaled = proto;
        scaled.upgrageByFloor(floor);  // 這是重點：樓層加強
        enemies.push_back({ scaled, pos });
        EnemyInMap::allPos.insert(pos);
    }
}


void Manager::spawnMerchants(int count, const std::string& merchantDataPath) {
    const auto& grid = maps.getMap();
    int H = grid.size(), W = grid[0].size();
    int tries = 0, spawned = 0;
    while (spawned < count && ++tries < 10000) {
        int x = std::rand() % W;
        int y = std::rand() % H;
        auto pos = std::make_pair(x, y);
        if (grid[y][x] != FLOOR || EnemyInMap::allPos.count(pos) || pos == playerPos || MerchantInMap::allPos.count(pos))
            continue;
        // 三種類型的商人
        MerchantType type = static_cast<MerchantType>(randInt(0, 2));
        Merchant m(type, player.getlv());
        LoadMerchantData(m, merchantDataPath);
        merchants.push_back({ m, pos });
		MerchantInMap::allPos.insert(pos);
        spawned++;
    }
}

void Manager::battleIfNeeded() {
    for (auto& e : enemies) {
        if (!e.enemy.Died() && e.pos == playerPos) {
            bool win = Battle(player, e.enemy);
            if (win) {
                EnemyInMap::allPos.erase(e.pos);
                player.earnedexp(e.enemy.Giveexp());
                auto items = e.enemy.getFallBackpack();
                for(auto & item : items) {
                    player.getMaterial(item);
				}
            }
            else {
                gameOver = true;
            }
            break;
        }
    }
}

void Manager::interactIfMerchant() {
    for (auto& m : merchants) {
        if (m.pos == playerPos) {
            m.merchant.Interact(player);
            break;
        }
    }
}

void Manager::moveMainCharacter(int dir) {
    int nx = playerPos.first + dirs[dir][0];
    int ny = playerPos.second + dirs[dir][1];
    const auto& grid = maps.getMap();
    if (ny >= 0 && ny < (int)grid.size() &&
        nx >= 0 && nx < (int)grid[0].size() &&
        grid[ny][nx] != WALL) {
        playerPos = { nx, ny };
        battleIfNeeded();
        // 更新探索範圍 3x3
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                int x = nx + dx, y = ny + dy;
                if (x >= 0 && x < (int)grid[0].size() && y >= 0 && y < (int)grid.size()) {
                    allsearchPos.insert({ x, y });
                }
            }
        }
        interactIfMerchant();
        IsOpenTreasureBox();
        atStairs = (playerPos == maps.getStairs());
    }
}

void Manager::operateSkill()
{
	std::system("cls");
    std::cout << "已裝備技能" << std::endl;
    for (auto &it : player.allskillsDesc()) {
		std::cout << it.first << " : " << it.second << std::endl;
    }
    std::cout << "已學會技能" << std::endl;
    for (auto &it : player.allLearnskillsDesc()) {
		std::cout << it.first << " : " << it.second << std::endl;
    }
    while (true) {
        std::cout << "是否需要裝備或替換技能?" << std::endl;
		std::cout << "1.裝備技能 2.替換技能 3.返回" << std::endl;
		std::string choice;
        std::cin >> choice;
        if (choice == "1") {
            std::cout << "請輸入技能編號:" << std::endl;
            std::string skillName;
            std::cin >> skillName;
			int skillIndex = stoi(skillName);
            if (player.setbattleskill(skillIndex)) {
                std::cout << "技能裝備成功!" << std::endl;
            }
            else {
                std::cout << "技能裝備失敗，請檢查技能編號或是否已經裝備。" << std::endl;
            }
        }
        else if (choice == "2") {
            std::cout << "請輸入要替換的技能編號:" << std::endl;
            std::string oldSkillName, newSkillName;
            std::cin >> oldSkillName;
            std::cout << "請輸入新的技能編號:" << std::endl;
            std::cin >> newSkillName;
			int oldSkillIndex = stoi(oldSkillName);
			int newSkillIndex = stoi(newSkillName);
            if (player.changebattleskill(oldSkillIndex,newSkillIndex)) {
                std::cout << "技能替換成功!" << std::endl;
            }
            else {
                std::cout << "技能替換失敗，請檢查技能編號。" << std::endl;
            }
        }
        else if (choice == "3") {
            return;
        }
    }
}

void Manager::operateEquip()
{
    system("cls");
    std::cout << "已裝備物品" << std::endl;
    for (const auto& eq : player.allEquipsDesc()) {
        std::cout << eq.first << " : " << eq.second << std::endl;
    }
    while (true) {
        std::cout << "是否需要裝備或替換物品?" << std::endl;
        std::cout << "1.裝備物品 2.替換物品 3.返回" << std::endl;
        std::string choice;
        std::cin >> choice;
        if (choice == "1") {
            if (!player.canEquip()) {
                std::cout << "無法裝備更多物品，請先替換或卸下物品。" << std::endl;
	            continue;
            }
            std::cout << "請輸入物品編號:" << std::endl;
            std::string itemName;
            std::cin >> itemName;
            int itemIndex = stoi(itemName);
            if(itemIndex<0|| itemIndex >= player.getEquipSize()) {
                std::cout << "無效的物品編號，請重新輸入。" << std::endl;
                continue;
			}
			player.wearEquip(itemIndex);
        }
        else if (choice == "2") {
            std::cout << "請輸入要替換的物品編號:" << std::endl;
            std::string oldItemName, newItemName;
            std::cin >> oldItemName;
            std::cout << "請輸入新的物品編號:" << std::endl;
            std::cin >> newItemName;
            int oldItemIndex = stoi(oldItemName);
            if (oldItemIndex < 0 || oldItemIndex >= player.getEquipSize()) {
                std::cout << "無效的物品編號，請重新輸入。" << std::endl;
                continue;
            }
            int newItemIndex = stoi(newItemName);
            if(oldItemIndex == newItemIndex || newItemIndex < 0 || newItemIndex >= player.getEquipSize()) {
                std::cout << "無效的物品編號，請重新輸入。" << std::endl;
                continue;
			}
            player.throwEquip(oldItemIndex);
			player.wearEquip(newItemIndex);
        }
        else if (choice == "3") {
            return;
        }
	}
}

void Manager::putTextureBox(int numbers)
{
    const auto& grid = maps.getMap();
    int H = grid.size(), W = grid[0].size();
    int tries = 0, spawned = 0;
    while (spawned <numbers && ++tries < 10000) {
        int x = std::rand() % W;
        int y = std::rand() % H;
        auto pos = std::make_pair(x, y);
        if (grid[y][x] != FLOOR || EnemyInMap::allPos.count(pos) || pos == playerPos || MerchantInMap::allPos.count(pos))
            continue;
        TreasureBox box;
        if (!box.randomset(numbers))
            continue;
		treasureBoxes.push_back({ box, pos });
		TreasureBoxInMap::allPos.insert(pos);
        spawned++;
    }
}

void Manager::IsOpenTreasureBox() {
    for (auto it = treasureBoxes.begin(); it != treasureBoxes.end(); ++it) {
        if (it->pos == playerPos) {
            
            for (auto& mat : it->treasureBox.ecMaterial)
                player.getMaterial(mat);
            for (auto& eq : it->treasureBox.ecEquip)
                player.getEquip(eq);
            for (auto& mi : it->treasureBox.ecMiseryItem)
                player.getMyseryItem(mi);

            
            std::cout << "[Debug] 玩家當前裝備數量: "
                << player.getEquipSize() << "\n";
            system("pause");

            
            showTreasureBox(*it);

            
            treasureBoxes.erase(it);
            TreasureBoxInMap::allPos.erase(playerPos);
            break;
        }
    }
}



void Manager::nextMap() {
    int H = maps.getMap().size();
    int W = maps.getMap()[0].size();
    maps = MapGenerator(W, H);
    allsearchPos.clear();
    enemies.clear();
	treasureBoxes.clear();
    EnemyInMap::allPos.clear();
	MerchantInMap::allPos.clear();
    merchants.clear();
    playerPos = pickRandomFloor();
    gameOver = false; atStairs = false;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            int x = playerPos.first + dx;
            int y = playerPos.second + dy;
            if (x >= 0 && x < W && y >= 0 && y < H)
                allsearchPos.insert({ x, y });
        }
    }
}

bool Manager::isGameOver() const { return gameOver; }
bool Manager::reachedStairs() const { return atStairs; }

void Manager::printMap() const {
    const auto& grid = maps.getMap();
    int H = grid.size(), W = grid[0].size();
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            auto pos = std::make_pair(x, y);
            if (pos == playerPos) {
                std::cout << 'P';
            }
            else if (std::any_of(merchants.begin(), merchants.end(),
                [&](auto& mm) { return mm.pos == pos; }) && allsearchPos.count({x,y})) {
                std::cout << 'M';
            }
            else if (!allsearchPos.count(pos)) {
                std::cout << ' ';
            }
            else if (EnemyInMap::allPos.count(pos)) {
                std::cout << 'E';
            }
            else if (TreasureBoxInMap::allPos.count(pos)) {
                std::cout << '!';
            }
            else {
                switch (grid[y][x]) {
                case WALL:       std::cout << '#'; break;
                case STAIR_DOWN: std::cout << '>'; break;
                default:         std::cout << '.'; break;
                }
            }
        }
        std::cout << '\n';
    }
}

Player& Manager::getPlayer() { return player; }
const Player& Manager::getPlayer() const { return player; }


