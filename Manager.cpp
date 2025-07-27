// Manager.cpp
#include "Manager.h"
#include "DataLoader.h"  //  LoadMerchantData
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "SfmlViewer.h"
#include "MerchantSFML.h"
#include <SFML/Graphics.hpp>

extern bool g_useSFMLBattle; // 在 main.cpp 或 DungeonGame.cpp 定義

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
        std::cout << "  o F: Material"
            << mat.getName() << "(" << mat.getDesc() << ")" << "\n";
        hasLoot = true;
    }
    for (auto& eq : it.treasureBox.ecEquip) {
        std::cout << "  o F: Equipment"
            << eq.getName() << "(" << eq.getDesc() << ")" << "\n";
        hasLoot = true;
    }
    for (auto& mi : it.treasureBox.ecMiseryItem) {
        std::cout << "  o F: MiseryItem"
            << mi.getName() << "(" << mi.getDesc() << ")" << "\n";
        hasLoot = true;
    }

    if (!hasLoot) {
        std::cout << " _ c O Ū  A   򳣨S   ...\n";
    }
    system("pause");
}

void showTreasureBoxSFML(const TreasureBox& box, sf::RenderWindow& window, sf::Font& font) {
    bool running = true;
    while (running && window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::KeyPressed)
                running = false;
        }
        window.clear(sf::Color(40, 30, 10));
        sf::Text title(sf::String::fromUtf8(u8"你打開了寶箱！", u8"你打開了寶箱！" + strlen(u8"你打開了寶箱！")), font, 28);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(60, 30);
        window.draw(title);
        int y = 80;
        for (const auto& mat : box.ecMaterial) {
            std::string matStr = "材料: " + mat.getName() + " - " + mat.getDesc();
            sf::Text t(sf::String::fromUtf8(matStr.begin(), matStr.end()), font, 20);
            t.setFillColor(sf::Color::White);
            t.setPosition(60, y); y += 30;
            window.draw(t);
        }
        for (const auto& eq : box.ecEquip) {
            std::string eqStr = "裝備: " + eq.getName() + " - " + eq.getDesc();
            sf::Text t(sf::String::fromUtf8(eqStr.begin(), eqStr.end()), font, 20);
            t.setFillColor(sf::Color::Cyan);
            t.setPosition(60, y); y += 30;
            window.draw(t);
        }
        for (const auto& mi : box.ecMiseryItem) {
            std::string miStr = "道具: " + mi.getName() + " - " + mi.getDesc();
            sf::Text t(sf::String::fromUtf8(miStr.begin(), miStr.end()), font, 20);
            t.setFillColor(sf::Color::Green);
            t.setPosition(60, y); y += 30;
            window.draw(t);
        }
        if (box.ecMaterial.empty() && box.ecEquip.empty() && box.ecMiseryItem.empty()) {
            sf::Text t(sf::String::fromUtf8(u8"寶箱是空的...", u8"寶箱是空的..." + strlen(u8"寶箱是空的...")), font, 22);
            t.setFillColor(sf::Color::Red);
            t.setPosition(60, y);
            window.draw(t);
        }
        sf::Text ok(sf::String::fromUtf8(u8"（點擊或按任意鍵繼續）", u8"（點擊或按任意鍵繼續）" + strlen(u8"（點擊或按任意鍵繼續）")), font, 18);
        ok.setFillColor(sf::Color::White);
        ok.setPosition(60, y + 40);
        window.draw(ok);
        window.display();
    }
}


Manager::Manager(int w, int h)
    : maps(w, h), player("Hero") {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    playerPos = pickRandomFloor();
    allsearchPos.clear();
    //   l Ʊ    d   3x3
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
    : maps(w, h), player(p),addjust(false) {
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

    //  إ߼Ӽh ̿઺     
    std::vector<const Enemy*> weightedPool;
    for (const auto& e : prototypes) {
        //  Ӽh V   Arank      ĤH   v N V j
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
        scaled.upgrageByFloor(floor);  //  o O   I G Ӽh [ j
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
        //  T         ӤH
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
            if (addjust)
            {
                e.enemy.CritizeByPlayerLv(player.getlv());
            }
            bool win;
            if (g_useSFMLBattle) {
                extern sf::RenderWindow* g_sfmlWindow;
                win = BattleSFML(*g_sfmlWindow, player, e.enemy);
            } else {
                win = Battle(player, e.enemy);
            }
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
            if (g_useSFMLBattle) {
                MerchantInteractSFML(m.merchant, player);
            } else {
                m.merchant.Interact(player);
            }
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
        //   s     d   3x3
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
    std::cout << " w ˳Ƨޯ " << std::endl;
    for (auto &it : player.allskillsDesc()) {
		std::cout << it.first << " : " << it.second << std::endl;
    }
    std::cout << " w Ƿ| ޯ " << std::endl;
    for (auto &it : player.allLearnskillsDesc()) {
		std::cout << it.first << " : " << it.second << std::endl;
    }
    while (true) {
        std::cout << " O _ ݭn ˳Ʃδ    ޯ ?" << std::endl;
		std::cout << "1. ˳Ƨޯ  2.     ޯ  3.  ^" << std::endl;
		std::string choice;
        std::cin >> choice;
        if (choice == "1") {
            std::cout << " п J ޯ s  :" << std::endl;
            std::string skillName;
            std::cin >> skillName;
			int skillIndex = stoi(skillName);
            if (player.setbattleskill(skillIndex)) {
                std::cout << " ޯ ˳Ʀ  \!" << std::endl;
            }
            else {
                std::cout << " ޯ ˳ƥ  ѡA   ˬd ޯ s   άO _ w g ˳ơC" << std::endl;
            }
        }
        else if (choice == "2") {
            std::cout << " п J n       ޯ s  :" << std::endl;
            std::string oldSkillName, newSkillName;
            std::cin >> oldSkillName;
            std::cout << " п J s   ޯ s  :" << std::endl;
            std::cin >> newSkillName;
			int oldSkillIndex = stoi(oldSkillName);
			int newSkillIndex = stoi(newSkillName);
            if (player.changebattleskill(oldSkillIndex,newSkillIndex)) {
                std::cout << " ޯ       \!" << std::endl;
            }
            else {
                std::cout << " ޯ       ѡA   ˬd ޯ s   C" << std::endl;
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
    std::cout << " w ˳ƪ  ~" << std::endl;
    for (const auto& eq : player.allEquipsDesc()) {
        std::cout << eq.first << " : " << eq.second << std::endl;
    }
    while (true) {
        std::cout << " O _ ݭn ˳Ʃδ      ~?" << std::endl;
        std::cout << "1. ˳ƪ  ~ 2.       ~ 3.  ^" << std::endl;
        std::string choice;
        std::cin >> choice;
        if (choice == "1") {
            if (!player.canEquip()) {
                std::cout << " L k ˳Ƨ h   ~ A Х      Ψ  U   ~ C" << std::endl;
	            continue;
            }
            std::cout << " п J   ~ s  :" << std::endl;
            std::string itemName;
            std::cin >> itemName;
            int itemIndex = stoi(itemName);
            if(itemIndex<0|| itemIndex >= player.getEquipSize()) {
                std::cout << " L Ī    ~ s   A Э  s  J C" << std::endl;
                continue;
			}
			player.wearEquip(itemIndex);
        }
        else if (choice == "2") {
            std::cout << " п J n         ~ s  :" << std::endl;
            std::string oldItemName, newItemName;
            std::cin >> oldItemName;
            std::cout << " п J s     ~ s  :" << std::endl;
            std::cin >> newItemName;
            int oldItemIndex = stoi(oldItemName);
            if (oldItemIndex < 0 || oldItemIndex >= player.getEquipSize()) {
                std::cout << " L Ī    ~ s   A Э  s  J C" << std::endl;
                continue;
            }
            int newItemIndex = stoi(newItemName);
            if (newItemIndex < 0 || newItemIndex >= player.getEquipSize() || oldItemIndex == newItemIndex) {
                std::cout << " L Ī    ~ s   A Э  s  J C" << std::endl;
                continue;
                        }
            if (newItemIndex > oldItemIndex) newItemIndex--;
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
            if (g_useSFMLBattle) {
                extern sf::RenderWindow* g_sfmlWindow;
                extern sf::Font* g_sfmlFont;
                if (g_sfmlWindow && g_sfmlFont && g_sfmlWindow->isOpen())
                    showTreasureBoxSFML(it->treasureBox, *g_sfmlWindow, *g_sfmlFont);
            } else {
                showTreasureBox(*it);
            }
            TreasureBoxInMap::allPos.erase(playerPos);
            treasureBoxes.erase(it);
            return;
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

const std::set<std::pair<int,int>>& Manager::getVisible() const { return allsearchPos; }

void Manager::Shouldadjust()
{
    addjust= true;
}


