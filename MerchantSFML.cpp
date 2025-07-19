#include "MerchantSFML.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream> // Added for std::cerr

MerchantSFMLUI::MerchantSFMLUI(sf::RenderWindow& win, sf::Font& font, Player& player)
    : window(win), font(font), player(player) {}

void MerchantSFMLUI::showTitle(const std::string& t) { title = t; }
void MerchantSFMLUI::showGoods(const std::vector<std::string>& n, const std::vector<std::string>& d, const std::vector<std::string>& p) { names = n; descs = d; prices = p; }
void MerchantSFMLUI::showMoney(const Money& money) { moneyStr = "金幣:" + std::to_string(money.Gold) + " 銀幣:" + std::to_string(money.Sliver) + " 銅幣:" + std::to_string(money.Cooper); }
int MerchantSFMLUI::selectGoods(int maxIdx) {
    selectedIdx = -1;
    scrollOffset = 0;
    const int maxShow = 6; // 一頁最多顯示6個
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mx = event.mouseButton.x, my = event.mouseButton.y;
                for (int i = 0; i < (int)names.size(); ++i) {
                    int drawIdx = i - scrollOffset;
                    if (drawIdx < 0 || drawIdx >= maxShow) continue;
                    sf::FloatRect rect(60, 80 + drawIdx * 60, 480, 50);
                    if (rect.contains(mx, my)) {
                        selectedIdx = i; return i;
                    }
                }
                sf::FloatRect leaveRect(500, 20, 80, 30);
                if (leaveRect.contains(mx, my)) return -1;
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta < 0 && scrollOffset + maxShow < (int)names.size()) scrollOffset++;
                if (event.mouseWheelScroll.delta > 0 && scrollOffset > 0) scrollOffset--;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Down && scrollOffset + maxShow < (int)names.size()) scrollOffset++;
                if (event.key.code == sf::Keyboard::Up && scrollOffset > 0) scrollOffset--;
            }
        }
        drawUI(-1, false, -1, {});
    }
    return -1;
}
std::string MerchantSFMLUI::selectAction(const std::vector<std::string>& actions) {
    selectedAction = "";
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mx = event.mouseButton.x, my = event.mouseButton.y;
                sf::FloatRect buyRect(180, 300, 100, 40), sellRect(320, 300, 100, 40), closeRect(500, 20, 80, 30);
                if (buyRect.contains(mx, my)) return actions[0];
                if (sellRect.contains(mx, my) && actions.size() > 1) return actions[1];
                if (closeRect.contains(mx, my)) return "";
            }
        }
        drawUI(selectedIdx, true, (int)actions.size(), actions);
    }
    return "";
}
void MerchantSFMLUI::showTip(const std::string& msg) { tipMsg = msg; tipShow = 1; tipClock.restart(); }
void MerchantSFMLUI::wait() {
    while (window.isOpen()) {
        if (tipShow && tipClock.getElapsedTime().asSeconds() > 1.2f) break;
        drawUI(selectedIdx, false, -1, {});
    }
    tipShow = 0;
}
void MerchantSFMLUI::drawUI(int hover, bool showOp, int opCount, const std::vector<std::string>& actions) {
    window.clear(sf::Color(30,30,30));
    sf::Text ttitle(sf::String::fromUtf8(title.begin(), title.end()), font, 24); ttitle.setFillColor(sf::Color::White); ttitle.setPosition(60, 20); window.draw(ttitle);
    sf::Text money(sf::String::fromUtf8(moneyStr.begin(), moneyStr.end()), font, 20); money.setFillColor(sf::Color::Yellow); money.setPosition(60, 50); window.draw(money);
    const int maxShow = 6;
    for (int i = scrollOffset; i < (int)names.size() && i < scrollOffset + maxShow; ++i) {
        int drawIdx = i - scrollOffset;
        sf::RectangleShape btn(sf::Vector2f(480, 50)); btn.setPosition(60, 80 + drawIdx * 60);
        btn.setFillColor(i == hover ? sf::Color(180,220,255) : sf::Color(220,220,220)); window.draw(btn);
        std::string itemStr = names[i] + " - " + descs[i] + " 價格:" + prices[i];
        sf::Text t(sf::String::fromUtf8(itemStr.begin(), itemStr.end()), font, 20); t.setFillColor(sf::Color::Black); t.setPosition(70, 90 + drawIdx * 60); window.draw(t);
    }
    sf::RectangleShape leaveBtn(sf::Vector2f(80, 30)); leaveBtn.setPosition(500, 20); leaveBtn.setFillColor(sf::Color(200,200,200)); window.draw(leaveBtn);
    sf::Text leaveText(sf::String::fromUtf8(u8"離開", u8"離開"+strlen(u8"離開")), font, 18); leaveText.setFillColor(sf::Color::Black); leaveText.setPosition(520, 25); window.draw(leaveText);
    if (showOp && selectedIdx >= 0 && selectedIdx < (int)names.size() && opCount > 0) {
        sf::RectangleShape opWin(sf::Vector2f(300, 120)); opWin.setPosition(150, 250); opWin.setFillColor(sf::Color(240,240,240)); window.draw(opWin);
        std::string opStr = u8"請選擇操作：";
        sf::Text t1(sf::String::fromUtf8(opStr.begin(), opStr.end()), font, 22); t1.setFillColor(sf::Color::Black); t1.setPosition(180, 260); window.draw(t1);
        if (opCount > 0) {
            sf::RectangleShape buyBtn(sf::Vector2f(100, 40)); buyBtn.setPosition(180, 300); buyBtn.setFillColor(sf::Color(200,255,200)); window.draw(buyBtn);
            std::string buyStr = actions[0];
            sf::Text buyText(sf::String::fromUtf8(buyStr.begin(), buyStr.end()), font, 22); buyText.setFillColor(sf::Color::Black); buyText.setPosition(210, 308); window.draw(buyText);
        }
        if (opCount > 1) {
            sf::RectangleShape sellBtn(sf::Vector2f(100, 40)); sellBtn.setPosition(320, 300); sellBtn.setFillColor(sf::Color(255,220,200)); window.draw(sellBtn);
            std::string sellStr = actions[1];
            sf::Text sellText(sf::String::fromUtf8(sellStr.begin(), sellStr.end()), font, 22); sellText.setFillColor(sf::Color::Black); sellText.setPosition(350, 308); window.draw(sellText);
        }
        sf::RectangleShape closeBtn(sf::Vector2f(80, 30)); closeBtn.setPosition(500, 20); closeBtn.setFillColor(sf::Color(200,200,200)); window.draw(closeBtn);
        std::string closeStr = u8"返回";
        sf::Text closeText(sf::String::fromUtf8(closeStr.begin(), closeStr.end()), font, 18); closeText.setFillColor(sf::Color::Black); closeText.setPosition(520, 25); window.draw(closeText);
    }
    if (tipShow && tipClock.getElapsedTime().asSeconds() < 1.2f) {
        std::string tipStr = tipMsg;
        sf::Text tip(sf::String::fromUtf8(tipStr.begin(), tipStr.end()), font, 22); tip.setFillColor(sf::Color::Red); tip.setPosition(180, 420); window.draw(tip);
    }
    window.display();
}

void MerchantInteractSFML(Merchant& merchant, Player& player) {
    sf::RenderWindow window(sf::VideoMode(600, 480), "商店");
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/msjh.ttc")) {
        std::cerr << "[錯誤] 字型載入失敗：C:/Windows/Fonts/msjh.ttc\n";
    }
    MerchantSFMLUI ui(window, font, player);
    merchant.Interact(player, ui);
} 