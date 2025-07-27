#include "SfmlViewer.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include "Battle.h"
#include <algorithm>
#include <map>
#include "Manager.h"
extern Manager* g_mgr_ptr;
SFMLMANAGER sfmlMgr;

// --- SfmlBattleUI ---

namespace {
    sf::RenderWindow* g_battleWindow = nullptr;
    sf::Font g_battleFont;
    bool g_battleFontLoaded = false;
    int g_battleAction = 1;
    bool g_battleWait = false;
    std::string g_battleInfo;
    std::string g_battleResult;
}

// 1. raceToImage map
static const std::map<std::string, std::string> raceToImage = {
    {"哥布林", "Basic Asset Pack/Basic Monster Animations/Blinded Grimlock/BlindedGrimlock.png"},
    {"史萊姆", "Basic Asset Pack/Basic Monster Animations/Death Slime/DeathSlime.png"},
    {"猛獸", "Basic Asset Pack/Basic Monster Animations/Stone Troll/StoneTroll.png"},
    {"不死族", "Basic Asset Pack/Basic Monster Animations/Ochre Jelly/OchreJelly.png"},
    {"鬥士", "Basic Asset Pack/Basic Monster Animations/Brawny Ogre/BrawnyOgre.png"},
    {"鳥族", "Basic Asset Pack/Basic Monster Animations/Bloodshot Eye/BloodshotEye.png"},
    {"元素精靈", "Basic Asset Pack/Basic Monster Animations/Fungal Myconid/FungalMyconid.png"},
    {"暗影刺客", "Basic Asset Pack/Basic Monster Animations/Red Cap/RedCap.png"},
    {"石像鬼", "Basic Asset Pack/Basic Monster Animations/Crushing Cyclops/CrushingCyclops.png"},
    {"巨人", "Basic Asset Pack/Basic Monster Animations/Humongous Ettin/HumongousEttin.png"},
    {"魔像", "Basic Asset Pack/Basic Monster Animations/Ocular Watcher/OcularWatcher.png"},
    {"龍", "Basic Asset Pack/Basic Dragon Animations/Adult Green Dragon/AdultGreenDragon.png"}
};

void SfmlBattleUI::showState(const Player& a, const Enemy& b, const std::string& info) {
    if (!g_battleWindow) return;
    g_battleInfo = info;
    g_battleAction = 0;
    // 畫面
    g_battleWindow->clear(sf::Color(30, 30, 30));
    // 角色圖
    sf::Texture playerTex;
    playerTex.loadFromFile("Basic Asset Pack/knight.png");
    std::string race = b.getRace();
    const sf::Texture* enemyTexPtr = nullptr;
    if (g_mgr_ptr) {
        auto* mgr = g_mgr_ptr;
        if (mgr->getEnemies().size() > 0) {
            extern SFMLMANAGER sfmlMgr;
            enemyTexPtr = sfmlMgr.getEnemyTexture(race);
        }
    }
    if (!enemyTexPtr) {
        extern SFMLMANAGER sfmlMgr;
        enemyTexPtr = sfmlMgr.getDefaultEnemyTexture();
    }
    sf::Sprite playerSprite(playerTex, sf::IntRect(0, 0, 24, 24)); 
    sf::Sprite enemySprite(*enemyTexPtr, sf::IntRect(0, 0, 16, 16)); 
    playerSprite.setPosition(80, 140);
    playerSprite.setScale(80.f / 24, 80.f / 24); 
    enemySprite.setPosition(320, 140);
    enemySprite.setScale(80.f / 16, 80.f / 16); 
    g_battleWindow->draw(playerSprite);
    g_battleWindow->draw(enemySprite);
    // 血量條
    sf::RectangleShape playerBar(sf::Vector2f(120 * (float)a.gethp() / a.getMaxHp(), 15));
    playerBar.setFillColor(sf::Color::Green);
    playerBar.setPosition(80, 120);
    g_battleWindow->draw(playerBar);
    sf::RectangleShape enemyBar(sf::Vector2f(120 * (float)b.gethp() / b.getMaxhp(), 15));
    enemyBar.setFillColor(sf::Color::Red);
    enemyBar.setPosition(320, 120);
    g_battleWindow->draw(enemyBar);
    // MP條
    sf::RectangleShape playerMpBar(sf::Vector2f(120 * (float)a.getmp() / a.getMaxMp(), 10));
    playerMpBar.setFillColor(sf::Color(80, 80, 255));
    playerMpBar.setPosition(80, 140);
    g_battleWindow->draw(playerMpBar);
    sf::RectangleShape enemyMpBar(sf::Vector2f(120 * (float)b.getmp() / b.getMaxmp(), 10));
    enemyMpBar.setFillColor(sf::Color(80, 80, 255));
    enemyMpBar.setPosition(320, 140);
    g_battleWindow->draw(enemyMpBar);
    // 文字
    if (!g_battleFontLoaded) {
        g_battleFontLoaded = g_battleFont.loadFromFile("C:/Windows/Fonts/msjh.ttc");
        if (!g_battleFontLoaded) {
            std::cerr << "[錯誤] 字型載入失敗：C:/Windows/Fonts/msjh.ttc\n";
        }
    }
    std::string pname = a.getname();
    sf::Text pName(sf::String::fromUtf8(pname.begin(), pname.end()), g_battleFont, 18);
    pName.setPosition(80, 100);
    g_battleWindow->draw(pName);
    std::string ename = b.getname();
    sf::Text eName(sf::String::fromUtf8(ename.begin(), ename.end()), g_battleFont, 18);
    eName.setPosition(320, 100);
    g_battleWindow->draw(eName);
    sf::Text infoText(sf::String::fromUtf8(info.begin(), info.end()), g_battleFont, 20);
    infoText.setPosition(120, 60);
    g_battleWindow->draw(infoText);
    // 血量數字
    sf::Text pHp(std::to_string(a.gethp()) + "/" + std::to_string(a.getMaxHp()), g_battleFont, 16);
    pHp.setPosition(80, 220);
    g_battleWindow->draw(pHp);
    sf::Text eHp(std::to_string(b.gethp()) + "/" + std::to_string(b.getMaxhp()), g_battleFont, 16);
    eHp.setPosition(320, 220);
    g_battleWindow->draw(eHp);
    // MP數字
    sf::Text pMp(std::to_string(a.getmp()) + "/" + std::to_string(a.getMaxMp()), g_battleFont, 14);
    pMp.setFillColor(sf::Color(80,80,255));
    pMp.setPosition(80, 155);
    g_battleWindow->draw(pMp);
    sf::Text eMp(std::to_string(b.getmp()) + "/" + std::to_string(b.getMaxmp()), g_battleFont, 14);
    eMp.setFillColor(sf::Color(80,80,255));
    eMp.setPosition(320, 155);
    g_battleWindow->draw(eMp);
    int py = 240;
    for (const auto& d : a.listEffectsDesc()) {
        sf::Text t(sf::String::fromUtf8(d.begin(), d.end()), g_battleFont, 14);
        t.setPosition(60, py);
        g_battleWindow->draw(t);
        py += 20;
    }
    int ey = 240;
    for (const auto& d : b.listEffectsDesc()) {
        sf::Text t(sf::String::fromUtf8(d.begin(), d.end()), g_battleFont, 14);
        t.setPosition(300, ey);
        g_battleWindow->draw(t);
        ey += 20;
    }
    // 操作提示
    sf::Text op1(sf::String::fromUtf8(u8"1.攻擊  2.技能  3.道具", u8"1.攻擊  2.技能  3.道具" + strlen(u8"1.攻擊  2.技能  3.道具")), g_battleFont, 18);
    op1.setPosition(120, 320);
    g_battleWindow->draw(op1);
    // --- 繪製浮動傷害數字 ---
    for (auto& fn : floatingNumbers) {
        sf::Text dmgText(std::to_string(fn.value), g_battleFont, 32);
        dmgText.setFillColor(fn.color);
        dmgText.setPosition(fn.pos);
        dmgText.setStyle(sf::Text::Bold);
        g_battleWindow->draw(dmgText);
    }
    g_battleWindow->display();
    // 更新浮動數字位置與生命週期
    float dt = 0.05f; // 每次showState呼叫間隔
    for (auto& fn : floatingNumbers) {
        fn.pos.y -= 0.5f; // 上升速度變慢
        fn.lifetime -= dt;
        fn.color.a = static_cast<sf::Uint8>(255 * std::max(0.f, fn.lifetime / 2.0f)); // 2秒內淡出
    }
    floatingNumbers.erase(std::remove_if(floatingNumbers.begin(), floatingNumbers.end(), [](const FloatingNumber& fn) { return fn.lifetime <= 0.f; }), floatingNumbers.end());
}

// 改寫 getPlayerAction：滑鼠點擊主選單
int SfmlBattleUI::getPlayerAction(const Player& a, const Enemy& b) {
    int action = 0;
    while (g_battleWindow && g_battleWindow->isOpen() && action == 0) {
        g_battleWindow->clear(sf::Color(30, 30, 30));
        // 角色圖
        sf::Texture playerTex, enemyTex;
        playerTex.loadFromFile("Basic Asset Pack/knight.png");
        // 根據種族載入敵人圖
        std::string race = b.getRace();
        std::string texPath = "Enemy_Animations_Set/enemies-skeleton1_idle.png";
        if (raceToImage.count(race)) texPath = raceToImage.at(race);
        enemyTex.loadFromFile(texPath);
        sf::Sprite playerSprite(playerTex, sf::IntRect(0, 0, 24, 24));
        sf::Sprite enemySprite(enemyTex, sf::IntRect(0, 0, 16, 16));
        playerSprite.setPosition(80, 140);
        playerSprite.setScale(80.f / 24, 80.f / 24);
        enemySprite.setPosition(320, 140);
        enemySprite.setScale(80.f / 16, 80.f / 16);
        g_battleWindow->draw(playerSprite);
        g_battleWindow->draw(enemySprite);
        // 血量條
        sf::RectangleShape playerBar(sf::Vector2f(120 * (float)a.gethp() / a.getMaxHp(), 15));
        playerBar.setFillColor(sf::Color::Green);
        playerBar.setPosition(80, 120);
        g_battleWindow->draw(playerBar);
        sf::RectangleShape enemyBar(sf::Vector2f(120 * (float)b.gethp() / b.getMaxhp(), 15));
        enemyBar.setFillColor(sf::Color::Red);
        enemyBar.setPosition(320, 120);
        g_battleWindow->draw(enemyBar);
        // MP條
        sf::RectangleShape playerMpBar(sf::Vector2f(120 * (float)a.getmp() / a.getMaxMp(), 10));
        playerMpBar.setFillColor(sf::Color(80, 80, 255));
        playerMpBar.setPosition(80, 140);
        g_battleWindow->draw(playerMpBar);
        sf::RectangleShape enemyMpBar(sf::Vector2f(120 * (float)b.getmp() / b.getMaxmp(), 10));
        enemyMpBar.setFillColor(sf::Color(80, 80, 255));
        enemyMpBar.setPosition(320, 140);
        g_battleWindow->draw(enemyMpBar);
        // 文字
        if (!g_battleFontLoaded) g_battleFontLoaded = g_battleFont.loadFromFile("C:/Windows/Fonts/msjh.ttc");
        if (!g_battleFontLoaded) {
            std::cerr << "[錯誤] 字型載入失敗：C:/Windows/Fonts/msjh.ttc\n";
        }
        std::string pname2 = a.getname();
        sf::Text pName(sf::String::fromUtf8(pname2.begin(), pname2.end()), g_battleFont, 18);
        pName.setPosition(80, 100);
        g_battleWindow->draw(pName);
        std::string ename2 = b.getname();
        sf::Text eName(sf::String::fromUtf8(ename2.begin(), ename2.end()), g_battleFont, 18);
        eName.setPosition(320, 100);
        g_battleWindow->draw(eName);
        sf::Text pHp(std::to_string(a.gethp()) + "/" + std::to_string(a.getMaxHp()), g_battleFont, 16);
        pHp.setPosition(80, 220);
        g_battleWindow->draw(pHp);
        sf::Text eHp(std::to_string(b.gethp()) + "/" + std::to_string(b.getMaxhp()), g_battleFont, 16);
        eHp.setPosition(320, 220);
        g_battleWindow->draw(eHp);
        // MP數字
        sf::Text pMp(std::to_string(a.getmp()) + "/" + std::to_string(a.getMaxMp()), g_battleFont, 14);
        pMp.setFillColor(sf::Color(80,80,255));
        pMp.setPosition(80, 155);
        g_battleWindow->draw(pMp);
        sf::Text eMp(std::to_string(b.getmp()) + "/" + std::to_string(b.getMaxmp()), g_battleFont, 14);
        eMp.setFillColor(sf::Color(80,80,255));
        eMp.setPosition(320, 155);
        g_battleWindow->draw(eMp);
        // 畫三個主選單按鈕
        const char* btns[3] = {u8"攻擊", u8"技能", u8"道具"};
        for (int i = 0; i < 3; ++i) {
            sf::RectangleShape btn(sf::Vector2f(120, 50));
            btn.setPosition(100 + i * 140, 340);
            btn.setFillColor(sf::Color(220, 220, 220));
            g_battleWindow->draw(btn);
            sf::Text text(sf::String::fromUtf8(btns[i], btns[i] + strlen(btns[i])), g_battleFont, 24);
            text.setFillColor(sf::Color::Black);
            text.setPosition(120 + i * 140, 350);
            g_battleWindow->draw(text);
        }
        g_battleWindow->display();
        sf::Event event;
        while (g_battleWindow->pollEvent(event)) {
            if (event.type == sf::Event::Closed) g_battleWindow->close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mx = event.mouseButton.x, my = event.mouseButton.y;
                for (int i = 0; i < 3; ++i) {
                    sf::FloatRect rect(100 + i * 140, 340, 120, 50);
                    if (rect.contains(mx, my)) {
                        action = i + 1;
                    }
                }
            }
        }
        sf::sleep(sf::milliseconds(10));
    }
    return action;
}

void SfmlBattleUI::showResult(const std::string& result) {
    g_battleResult = result;
    if (!g_battleWindow) return;
    sf::Text resText(sf::String::fromUtf8(result.begin(), result.end()), g_battleFont, 28);
    resText.setPosition(180, 120);
    g_battleWindow->draw(resText);
    g_battleWindow->display();
}

void SfmlBattleUI::wait() {
    if (!g_battleWindow) return;
    sf::sleep(sf::seconds(1));
}

// 新增：滑鼠選技能
int SfmlBattleUI::selectSkill(const Player& player) {
    auto skills = player.allskillsDesc();
    int skillCount = skills.size();
    if (!g_battleFontLoaded) { // 確保字型已載入
        g_battleFontLoaded = g_battleFont.loadFromFile("C:/Windows/Fonts/msjh.ttc");
        if (!g_battleFontLoaded) {
            std::cerr << "[錯誤] 字型載入失敗：C:/Windows/Fonts/msjh.ttc\n";
        }
    }
    int scrollOffset = 0;
    const int maxShow = 6;
    while (g_battleWindow && g_battleWindow->isOpen()) {
        g_battleWindow->clear(sf::Color(30, 30, 30));
        // 畫出技能按鈕
        for (int i = scrollOffset; i < skillCount && i < scrollOffset + maxShow; ++i) {
            int drawIdx = i - scrollOffset;
            sf::RectangleShape btn(sf::Vector2f(260, 40));
            btn.setPosition(100, 60 + drawIdx * 60);
            btn.setFillColor(sf::Color(220, 220, 220));
            g_battleWindow->draw(btn);
            sf::Text text;
            text.setFont(g_battleFont); // 使用已載入的字型
            std::string skillStr = skills[i].first + " (" + skills[i].second + ")";
            text.setString(sf::String::fromUtf8(skillStr.begin(), skillStr.end()));
            text.setCharacterSize(22);
            text.setFillColor(sf::Color::Black);
            text.setPosition(110, 65 + drawIdx * 60);
            g_battleWindow->draw(text);
        }
        // 提示
        sf::Text tip(sf::String::fromUtf8(u8"請用滑鼠點選技能，或按ESC取消", u8"請用滑鼠點選技能，或按ESC取消" + strlen(u8"請用滑鼠點選技能，或按ESC取消")), g_battleFont, 18);
        tip.setFillColor(sf::Color::White);
        tip.setPosition(100, 60 + maxShow * 60 + 10);
        g_battleWindow->draw(tip);
        g_battleWindow->display();
        sf::Event event;
        while (g_battleWindow->pollEvent(event)) {
            if (event.type == sf::Event::Closed) g_battleWindow->close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) return -1;
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mx = event.mouseButton.x, my = event.mouseButton.y;
                for (int i = scrollOffset; i < skillCount && i < scrollOffset + maxShow; ++i) {
                    int drawIdx = i - scrollOffset;
                    sf::FloatRect rect(100, 60 + drawIdx * 60, 260, 40);
                    if (rect.contains(mx, my)) {
                        return i;
                    }
                }
            }
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (event.mouseWheelScroll.delta < 0 && scrollOffset + maxShow < skillCount) scrollOffset++;
                if (event.mouseWheelScroll.delta > 0 && scrollOffset > 0) scrollOffset--;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Down && scrollOffset + maxShow < skillCount) scrollOffset++;
                if (event.key.code == sf::Keyboard::Up && scrollOffset > 0) scrollOffset--;
            }
        }
        sf::sleep(sf::milliseconds(10));
    }
    return -1;
}

// 新增：滑鼠選道具
int SfmlBattleUI::selectItem(Player& player, Enemy& enemy) {
    auto& backpack = player.getMyseryBackpack();
    if (backpack.empty()) {
        // 顯示沒有道具
        g_battleWindow->clear(sf::Color(30,30,30));
        if (!g_battleFontLoaded) g_battleFontLoaded = g_battleFont.loadFromFile("C:/Windows/Fonts/msjh.ttc");
        if (!g_battleFontLoaded) {
            std::cerr << "[錯誤] 字型載入失敗：C:/Windows/Fonts/msjh.ttc\n";
        }
        sf::Text t(sf::String::fromUtf8(u8"你沒有可以使用的道具。", u8"你沒有可以使用的道具。" + strlen(u8"你沒有可以使用的道具。")), g_battleFont, 22);
        t.setFillColor(sf::Color::White);
        t.setPosition(100, 120);
        g_battleWindow->draw(t);
        g_battleWindow->display();
        sf::sleep(sf::seconds(1.2f));
        return -1;
    }
    std::vector<std::string> keys;
    for (auto& it : backpack) keys.push_back(it.first);
    int hover = -1;
    while (g_battleWindow && g_battleWindow->isOpen()) {
        g_battleWindow->clear(sf::Color(30,30,30));
        sf::Text title(sf::String::fromUtf8(u8"請選擇要使用的道具：", u8"請選擇要使用的道具：" + strlen(u8"請選擇要使用的道具：")), g_battleFont, 22);
        title.setFillColor(sf::Color::White);
        title.setPosition(100, 30);
        g_battleWindow->draw(title);
        int mx = sf::Mouse::getPosition(*g_battleWindow).x, my = sf::Mouse::getPosition(*g_battleWindow).y;
        hover = -1;
        for (int i = 0; i < (int)keys.size(); ++i) {
            sf::FloatRect rect(80, 70 + i * 60, 320, 50);
            if (rect.contains(mx, my)) hover = i;
            sf::RectangleShape btn(sf::Vector2f(320, 50));
            btn.setPosition(80, 70 + i * 60);
            btn.setFillColor(i == hover ? sf::Color(180,220,255) : sf::Color(220,220,220));
            g_battleWindow->draw(btn);
            std::string itemStr = player.getMyseryBackpack().at(keys[i]).getName() + " - " + player.getMyseryBackpack().at(keys[i]).getDesc();
            sf::Text t(sf::String::fromUtf8(itemStr.begin(), itemStr.end()), g_battleFont, 20);
            t.setFillColor(sf::Color::Black);
            t.setPosition(90, 80 + i * 60);
            g_battleWindow->draw(t);
        }
        g_battleWindow->display();
        sf::Event event;
        while (g_battleWindow->pollEvent(event)) {
            if (event.type == sf::Event::Closed) g_battleWindow->close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mx = event.mouseButton.x, my = event.mouseButton.y;
                for (int i = 0; i < (int)keys.size(); ++i) {
                    sf::FloatRect rect(80, 70 + i * 60, 320, 50);
                    if (rect.contains(mx, my)) {
                        return i;
                    }
                }
            }
        }
        sf::sleep(sf::milliseconds(10));
    }
    return -1;
}

// 新版 Battle：可插拔 UI
bool Battle(Player& a, Enemy& b, BattleUI* ui) {
    int round = 0;
    std::string info = "戰鬥開始!";
    while (true) {
        ++round;
        a.Affected();
        b.Affected();
        if (b.Died()) {
            a.earnedexp(b.Giveexp());
            getallItem(a, b.getFallBackpack());
            printItem(b.getFallBackpack());
            if (a.goToNextLevel())
                info = a.getname() + " 升級了!";
            ui->showState(a, b, info);
            ui->showResult("Victory!");
            ui->wait();
            return true;
        }
        if (a.Died()) {
            ui->showState(a, b, info);
            ui->showResult("你被打倒了!");
            ui->wait();
            return false;
        }
        bool usedItemThisTurn = false;
        SkillResult playerRes; // 修正：移到這裡
        while (true) { // 玩家回合可多次操作（道具不消耗回合）
            ui->showState(a, b, info + "\n--- Battle Round " + std::to_string(round) + " ---");
            int action = ui->getPlayerAction(a, b);
            if (action == 1) {
                playerRes.immediateDamage = a.Basicattack();
                info = a.getname() + " 普通攻擊!";
                break; // 結束玩家回合
            } else if (action == 2) {
                int idx = -1;
                if (auto sfmlui = dynamic_cast<SfmlBattleUI*>(ui)) {
                    idx = sfmlui->selectSkill(a);
                } else {
                    printallskill(a);
                    std::cin >> idx;
                }
                if (idx >= 0) {
                    playerRes = a.useSkill(idx);
                    info = a.getname() + " 使用技能!";
                } else {
                    info = "取消技能選擇";
                }
                break; // 結束玩家回合
            } else if (action == 3) {
                if (usedItemThisTurn) {
                    // 已用過道具，顯示提示
                    ui->showState(a, b, u8"本回合只能用一次道具");
                    if (auto sfmlui = dynamic_cast<SfmlBattleUI*>(ui)) {
                        sf::sleep(sf::seconds(1));
                    } else {
                        system("pause");
                    }
                    continue; // 讓玩家重新選擇
                }
                int idx = -1;
                if (auto sfmlui = dynamic_cast<SfmlBattleUI*>(ui)) {
                    idx = sfmlui->selectItem(a, b);
                    if (idx >= 0) {
                        auto& backpack = a.getMyseryBackpack();
                        auto it = backpack.begin();
                        std::advance(it, idx);
                        Effect e = it->second.itemEffect;
                        bool reversed = (rand() % 100 < 30);
                        if (reversed) {
                            e.affectHp = -e.affectHp;
                            e.affectMp = -e.affectMp;
                            e.affectAtk = -e.affectAtk;
                            e.affectDef = -e.affectDef;
                            e.affectMissRate = -e.affectMissRate;
                        }
                        bool isBuff = (e.affectHp > 0 || e.affectMp > 0 || e.affectAtk > 0 || e.affectDef > 0 || e.affectMissRate > 0);
                        if (isBuff) {
                            a.BeEffect(e);
                            info = a.getname() + " 獲得 " + it->second.getName() + " 的效果。";
                        } else {
                            b.BeEffect(e);
                            info = b.getname() + " 承受了 " + it->second.getName() + " 的效果。";
                        }
                        a.throwMyseryItem(it->second);
                        usedItemThisTurn = true;
                    } else {
                        info = "沒有使用道具";
                    }
                } else {
                    playerUseItemDuringBattle(a, b);
                    info = a.getname() + " 使用道具!";
                    usedItemThisTurn = true;
                }
                continue; // 用道具後可再選其他行動，但不能再用道具
            }
        }
        if (playerRes.effect) {
            if (playerRes.target == Target::SELF) {
                a.BeEffect(*playerRes.effect);
                info += " " + a.getname() + " 獲得" + playerRes.effect->Desc + "!";
            } else if (playerRes.target == Target::ENEMY) {
                if (rand() % 100 >= int(b.getMissRate())) {
                    b.BeEffect(*playerRes.effect);
                    info += " " + b.getname() + " 受到" + playerRes.effect->Desc + "!";
                } else {
                    info += " 敵人閃避了效果";
                }
            }
        }
        if (playerRes.immediateDamage > 0) {
            if (rand() % 100 >= int(b.getMissRate())) {
                int dmg = b.BeAttacked(playerRes.immediateDamage);
                if (auto sfmlui = dynamic_cast<SfmlBattleUI*>(ui)) {
                    sfmlui->floatingNumbers.push_back({dmg, sf::Vector2f(340, 120), sf::Color::Red, 2.0f});
                }
                info += " 對敵人造成 " + std::to_string(dmg) + " 傷害!";
            } else {
                info += " 攻擊被閃避!";
            }
        }
        ui->showState(a, b, info);
        if (b.Died()) {
            a.earnedexp(b.Giveexp());
            getallItem(a, b.getFallBackpack());
            printItem(b.getFallBackpack());
            if (a.goToNextLevel())
                info = a.getname() + " 升級了!";
            ui->showResult("Victory!");
            ui->wait();
            return true;
        }
        // 敵人回合
        SkillResult enemyRes = b.Attack(a);
        if (enemyRes.effect) {
            if (enemyRes.target == Target::SELF) {
                b.BeEffect(*enemyRes.effect);
                info = b.getname() + " 獲得" + enemyRes.effect->Desc + "!";
            } else if (enemyRes.target == Target::ENEMY) {
                if (rand() % 100 >= int(a.getMissRate())) {
                    a.BeEffect(*enemyRes.effect);
                    info = a.getname() + " 受到" + enemyRes.effect->Desc + "!";
                } else {
                    info = a.getname() + " 閃避了效果";
                }
            }
        }
        if (enemyRes.immediateDamage > 0) {
            if (rand() % 100 >= int(a.getMissRate())) {
                int dmg = a.Beattacked(enemyRes.immediateDamage);
                if (auto sfmlui = dynamic_cast<SfmlBattleUI*>(ui)) {
                    sfmlui->floatingNumbers.push_back({dmg, sf::Vector2f(100, 120), sf::Color::Yellow, 2.0f});
                }
                info = b.getname() + " 攻擊造成 " + std::to_string(dmg) + " 傷害!";
            } else {
                info = b.getname() + " 的攻擊被閃避!";
            }
        }
        ui->showState(a, b, info);
        if (a.Died()) {
            ui->showResult("你被打倒了!");
            ui->wait();
            return false;
        }
    }
}

// BattleSFML 實作
bool BattleSFML(sf::RenderWindow& window, Player& player, Enemy& enemy) {
    g_battleWindow = &window;
    SfmlBattleUI ui;
    bool result = Battle(player, enemy, &ui);
    g_battleWindow = nullptr;
    return result;
}

bool SFMLMANAGER::loadTextures() {
    // JUدAЮھڹ�گ������|�վ�
    if (!playerTexture.loadFromFile("Basic Asset Pack/knight.png"))
        std::cerr << "主角圖片載入失敗\n";
    if (!wallTexture.loadFromFile("P_P_FREE_RPG_TILESET/Dungeon_24x24.png"))
        std::cerr << "牆壁載入失敗\n";
    if (!groundTexture.loadFromFile("P_P_FREE_RPG_TILESET/Dungeon_24x24.png"))
        std::cerr << "地面載入失敗\n";
    if (!enemyTexture.loadFromFile("Enemy_Animations_Set/enemies-skeleton1_idle.png"))
        std::cerr << "敵人載入失敗\n";
    if (!merchantTexture.loadFromFile("Basic Asset Pack/Basic Monster Animations/Red Cap/RedCap.png"))
        std::cerr << "商人載入失敗\n";
    if (!stairTexture.loadFromFile("P_P_FREE_RPG_TILESET/Dungeon_24x24.png"))
        std::cerr << "樓梯載入失敗\n";
    if (!treasureBoxTexture.loadFromFile("P_P_FREE_RPG_TILESET/decor.png"))
        std::cerr << "寶箱圖片載入失敗\n";
    playerSprite.setTexture(playerTexture);
    playerSprite.setTextureRect(sf::IntRect(0, 0, 24, 24)); // 只顯示第一格
    wallSprite.setTexture(wallTexture);
    wallSprite.setTextureRect(sf::IntRect(24, 0, 24, 24)); 
    groundSprite.setTexture(groundTexture);
    groundSprite.setTextureRect(sf::IntRect(0, 0, 24, 24)); // 
    enemySprite.setTexture(enemyTexture);
    enemySprite.setTextureRect(sf::IntRect(0, 0, 24, 24)); // 只顯示第一格(24x24)
    merchantSprite.setTexture(merchantTexture);
    merchantSprite.setTextureRect(sf::IntRect(0, 0, 16, 16)); // 只顯示第一格(16x16)
    stairSprite.setTexture(stairTexture);
    stairSprite.setTextureRect(sf::IntRect(48, 0, 24, 24)); // ]ӱtile
    treasureBoxSprite.setTexture(treasureBoxTexture);
    treasureBoxSprite.setTextureRect(sf::IntRect(24, 0, 24, 24)); // 第一行第二列tile
    // 集中載入所有敵人種族圖檔
    for (const auto& kv : raceToImage) {
        sf::Texture tex;
        if (tex.loadFromFile(kv.second)) {
            enemyTextureCache[kv.first] = tex;
        } else {
            std::cerr << "[警告] 載入敵人圖檔失敗: " << kv.second << "\n";
        }
    }
    loaded = true;
    return true;
}

void SFMLMANAGER::drawAll(sf::RenderWindow& window, const std::vector<std::vector<int>>& map,
    std::pair<int, int> playerPos,
    const std::vector<std::pair<int, int>>& enemies,
    const std::vector<std::pair<int, int>>& merchants,
    const std::vector<std::pair<int, int>>& treasureBoxes,
    std::pair<int, int> stairsPos,
    const std::set<std::pair<int,int>>& visible,
    int tileSize) {
    if (!loaded) return;
    int H = map.size(), W = map[0].size();
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            auto p = std::make_pair(x, y);
            if (visible.count(p)) {
                sf::Sprite* tile = nullptr;
                if (map[y][x] == 0) tile = &wallSprite;
                else if (map[y][x] == 1) tile = &groundSprite;
                else if (map[y][x] == 2) tile = &stairSprite;
                if (tile) {
                    tile->setPosition(x * tileSize, y * tileSize);
                    window.draw(*tile);
                }
            } else {
                sf::RectangleShape fog(sf::Vector2f((float)tileSize, (float)tileSize));
                fog.setFillColor(sf::Color(0,0,0));
                fog.setPosition(x * tileSize, y * tileSize);
                window.draw(fog);
            }
        }
    }
    for (auto& pos : treasureBoxes) {
        if (visible.count(pos)) {
            treasureBoxSprite.setPosition(pos.first * tileSize, pos.second * tileSize);
            window.draw(treasureBoxSprite);
        }
    }
    for (auto& pos : merchants) {
        if (visible.count(pos)) {
            merchantSprite.setPosition(pos.first * tileSize, pos.second * tileSize);
            merchantSprite.setScale((float)tileSize / 16, (float)tileSize / 16);
            window.draw(merchantSprite);
        }
    }
    // 畫敵人
    if (g_mgr_ptr) {
        const auto& enemyObjs = g_mgr_ptr->getEnemies();
        for (size_t i = 0; i < enemies.size(); ++i) {
            auto pos = enemies[i];
            if (!visible.count(pos)) continue;
            if (i < enemyObjs.size()) {
                std::string race = enemyObjs[i].enemy.getRace();
                const sf::Texture* tex = getEnemyTexture(race);
                if (!tex) tex = &enemyTexture;
                sf::Sprite sprite(*tex, sf::IntRect(0, 0, 24, 24));
                sprite.setPosition(pos.first * tileSize, pos.second * tileSize);
                sprite.setScale((float)tileSize / 24, (float)tileSize / 24);
                window.draw(sprite);
            } else {
                enemySprite.setPosition(pos.first * tileSize, pos.second * tileSize);
                enemySprite.setScale((float)tileSize / 24, (float)tileSize / 24);
                window.draw(enemySprite);
            }
        }
    } else {
        for (auto& pos : enemies) {
            if (!visible.count(pos)) continue;
            enemySprite.setPosition(pos.first * tileSize, pos.second * tileSize);
            enemySprite.setScale((float)tileSize / 24, (float)tileSize / 24);
            window.draw(enemySprite);
        }
    }
    //e
    playerSprite.setPosition(playerPos.first * tileSize, playerPos.second * tileSize);
    window.draw(playerSprite);
}
