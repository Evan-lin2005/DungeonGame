#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
#include "Battle.h"
#include <set>

// SFML 圖像化戰鬥介面
struct SfmlBattleUI : public BattleUI {
    bool* pResult = nullptr; // 用於外部取得勝負
    void showState(const Player&, const Enemy&, const std::string& info) override;
    int getPlayerAction(const Player&, const Enemy&) override;
    void showResult(const std::string& result) override;
    void wait() override;
    // 新增：滑鼠選技能
    int selectSkill(const Player& player);
    int selectItem(Player& player, Enemy& enemy); // 新增：滑鼠選道具
    // 內部狀態
    std::string lastInfo;
    int lastAction = 1;
    sf::Font font; // 用於技能選單中文字
    // 在SfmlBattleUI struct內新增：
    struct FloatingNumber {
        int value;
        sf::Vector2f pos;
        sf::Color color;
        float lifetime; // 秒
    };
    std::vector<FloatingNumber> floatingNumbers;
};

class SFMLMANAGER {
private:
    sf::Texture playerTexture, wallTexture, groundTexture, enemyTexture, merchantTexture, stairTexture, treasureBoxTexture;
    sf::Sprite playerSprite, wallSprite, groundSprite, enemySprite, merchantSprite, stairSprite, treasureBoxSprite;
    bool loaded = false;
    // 集中快取敵人圖檔
    std::map<std::string, sf::Texture> enemyTextureCache;
public:
    bool loadTextures();
    void drawAll(sf::RenderWindow& window, const std::vector<std::vector<int>>& map,
        std::pair<int, int> playerPos,
        const std::vector<std::pair<int, int>>& enemies,
        const std::vector<std::pair<int, int>>& merchants,
        const std::vector<std::pair<int, int>>& treasureBoxes,
        std::pair<int, int> stairsPos,
        const std::set<std::pair<int,int>>& visible,
        int tileSize = 24);
    // 取得敵人圖檔快取指標
    const sf::Texture* getEnemyTexture(const std::string& race) const {
        auto it = enemyTextureCache.find(race);
        if (it != enemyTextureCache.end()) return &it->second;
        return nullptr;
    }
    // 新增：取得預設敵人圖
    const sf::Texture* getDefaultEnemyTexture() const { return &enemyTexture; }
};

// 提供全域 sfmlMgr 供多個檔案共用
extern SFMLMANAGER sfmlMgr;

// 圖像化戰鬥：顯示雙方圖像、血量，玩家用鍵盤選擇攻擊，直到分出勝負
bool BattleSFML(sf::RenderWindow& window, Player& player, Enemy& enemy);
