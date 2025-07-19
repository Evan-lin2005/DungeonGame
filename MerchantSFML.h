#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Character.h" // for Player
#include "Item.h"      // for Money
#include "Merchant.h"  // for Merchant

class MerchantSFMLUI : public MerchantUI {
public:
    MerchantSFMLUI(sf::RenderWindow& win, sf::Font& font, Player& player);
    void showTitle(const std::string& t) override;
    void showGoods(const std::vector<std::string>& n, const std::vector<std::string>& d, const std::vector<std::string>& p) override;
    void showMoney(const Money& money) override;
    int selectGoods(int maxIdx) override;
    std::string selectAction(const std::vector<std::string>& actions) override;
    void showTip(const std::string& msg) override;
    void wait() override;
private:
    void drawUI(int hover, bool showOp, int opCount, const std::vector<std::string>& actions);
    sf::RenderWindow& window;
    sf::Font& font;
    Player& player;
    std::string title;
    std::vector<std::string> names, descs, prices;
    std::string moneyStr;
    int selectedIdx = -1;
    std::string selectedAction;
    std::string tipMsg;
    int tipShow = 0;
    sf::Clock tipClock;
    int scrollOffset = 0;
};

void MerchantInteractSFML(Merchant& merchant, Player& player); 