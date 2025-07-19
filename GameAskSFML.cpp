#include "GameAskSFML.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream> // Added for std::cerr

void GameAskSFML(int& h, int& w, int& floor) {
    sf::RenderWindow window(sf::VideoMode(480, 420), "選擇難度");
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/msjh.ttc")) {
        std::cerr << "[錯誤] 字型載入失敗：C:/Windows/Fonts/msjh.ttc\n";
    }
    const std::string titles[5] = {u8"簡易(3層)", u8"普通(6層)", u8"困難(9層)", u8"夢魘(12層)", u8"絕望(15層)"};
    const int params[5][3] = {{20,20,3},{25,25,6},{30,30,9},{35,35,12},{40,40,15}};
    int hover = -1;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mx = event.mouseButton.x, my = event.mouseButton.y;
                for (int i = 0; i < 5; ++i) {
                    sf::FloatRect rect(90, 60 + i * 65, 300, 50);
                    if (rect.contains(mx, my)) {
                        h = params[i][0]; w = params[i][1]; floor = params[i][2];
                        window.close();
                    }
                }
            }
        }
        // hover 效果
        int mx = sf::Mouse::getPosition(window).x, my = sf::Mouse::getPosition(window).y;
        hover = -1;
        for (int i = 0; i < 5; ++i) {
            sf::FloatRect rect(90, 60 + i * 65, 300, 50);
            if (rect.contains(mx, my)) hover = i;
        }
        window.clear(sf::Color(30,30,30));
        sf::Text title(sf::String::fromUtf8(u8"歡迎來到地下城系統 (請選擇難度)", u8"歡迎來到地下城系統 (請選擇難度)" + strlen(u8"歡迎來到地下城系統 (請選擇難度)")), font, 22);
        title.setFillColor(sf::Color::White);
        title.setPosition(30, 10);
        window.draw(title);
        for (int i = 0; i < 5; ++i) {
            sf::RectangleShape btn(sf::Vector2f(300, 50));
            btn.setPosition(90, 60 + i * 65);
            btn.setFillColor(i == hover ? sf::Color(180,220,255) : sf::Color(220,220,220));
            window.draw(btn);
            sf::Text t(sf::String::fromUtf8(titles[i].begin(), titles[i].end()), font, 26);
            t.setFillColor(sf::Color::Black);
            t.setPosition(110, 70 + i * 65);
            window.draw(t);
        }
        window.display();
    }
} 