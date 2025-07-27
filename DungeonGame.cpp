// DungeonGame.cpp
#include "DungeonGame.h"
#include "SfmlViewer.h"
#include <SFML/Graphics.hpp>
#include "Character.h"

Manager* g_mgr_ptr = nullptr;
sf::RenderWindow* g_sfmlWindow = nullptr;
sf::Font* g_sfmlFont = nullptr;

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
    if (std::ifstream(kPlayerSave)) {
        hero_ = loadPlayer(kPlayerSave);
    }
    else {
        std::cerr << "Error(Not Fond Characeter File)" << std::endl;
        return;
    }

    learnPlayerSkill(hero_, kSkillData);
    hero_.setbattleskill(0); hero_.setbattleskill(1);
    hero_.setbattleskill(2); hero_.setbattleskill(3);

    mgr_ = Manager(WIDTH, HEIGHT, hero_);

	pool_ = loadEnemies(kEnemyData);
    if (pool_.empty()) {
        std::cerr << "[ERROR] NotFindEnemyData in  " << kEnemyData << "\n";
        std::exit(1);
    }
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
        std::cout << "[w a s d  to move],[e Skill],[f Equip] ,[q exit] "
           <<"Floor : " << floor_ << " / " << MAX_FLOOR << ")\n";
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
                std::cout << "====================Boss!!!=====================" << std::endl;
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
            mgr_.spawnEnemies(pool_, 5 + floor_,floor_);
            mgr_.spawnMerchants(rand() % 5 + 1, KMerchantData);
			mgr_.putTextureBox(rand() % 5 + 1);
        }
    }

    if (victory_) {
        std::cout << "\nWin\n";
    }
}

void DungeonGame::runSFML() {
    const int tileSize = 24;
    auto& mgr = mgr_;
    sfmlMgr.loadTextures();
    g_mgr_ptr = &mgr;
    const auto* mapData = &mgr.getMapGenerator().getMap();
    int H = mapData->size(), W = (*mapData)[0].size();
    sf::RenderWindow window(sf::VideoMode(W * tileSize, H * tileSize),"DungeonGame(SFML)");
    window.setFramerateLimit(60);
    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/msyh.ttc");
    g_sfmlWindow = &window;
    g_sfmlFont = &font;
    while (window.isOpen()) {
        sf::Event event;
        int moveDir = -1;
        bool triggerSkill = false;
        bool triggerEquip = false;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                if (event.key.code == sf::Keyboard::W) moveDir = 1;
                if (event.key.code == sf::Keyboard::A) moveDir = 0;
                if (event.key.code == sf::Keyboard::S) moveDir = 3;
                if (event.key.code == sf::Keyboard::D) moveDir = 2;
                if (event.key.code == sf::Keyboard::E) triggerSkill = true;
                if (event.key.code == sf::Keyboard::F) triggerEquip = true;
            }
        }
        if (triggerSkill) {
            // 圖像化換技能介面
            int scrollOffset = 0;
            const int maxShow = 10;
            while (window.isOpen()) {
                window.clear(sf::Color(30,30,30));
                sf::Text title(sf::String::fromUtf8(u8"技能列表 (ESC 離開)", u8"技能列表 (ESC 離開)" + strlen(u8"技能列表 (ESC 離開)")), font, 22);
                title.setFillColor(sf::Color::White);
                title.setPosition(30, 10);
                window.draw(title);
                auto skills = mgr.getPlayer().allLearnskillsDesc();
                auto battleSkills = mgr.getPlayer().allskillsDesc();
                int y = 60;
                int hover = -1;
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (size_t i = scrollOffset; i < skills.size() && i < scrollOffset + maxShow; ++i) {
                    std::string skillStr = std::to_string(i) + ". " + skills[i].first + " - " + skills[i].second;
                    for (auto& bs : battleSkills) {
                        if (bs.first == skills[i].first) skillStr = u8"★ " + skillStr;
                    }
                    sf::Text t(sf::String::fromUtf8(skillStr.begin(), skillStr.end()), font, 20);
                    t.setFillColor(sf::Color::White);
                    t.setPosition(40, y);
                    sf::FloatRect rect(40, y, 520, 32);
                    if (rect.contains(mousePos.x, mousePos.y)) {
                        t.setFillColor(sf::Color(255,255,0));
                        hover = (int)i;
                    }
                    window.draw(t);
                    y += 36;
                }
                window.display();
                bool leave = false;
                sf::Event e2;
                while (window.pollEvent(e2)) {
                    if (e2.type == sf::Event::Closed) { window.close(); leave = true; }
                    if (e2.type == sf::Event::KeyPressed && e2.key.code == sf::Keyboard::Escape) leave = true;
                    if (e2.type == sf::Event::KeyPressed) {
                        if (e2.key.code == sf::Keyboard::Down && scrollOffset + maxShow < (int)skills.size()) scrollOffset++;
                        if (e2.key.code == sf::Keyboard::Up && scrollOffset > 0) scrollOffset--;
                    }
                    if (e2.type == sf::Event::MouseWheelScrolled) {
                        if (e2.mouseWheelScroll.delta < 0 && scrollOffset + maxShow < (int)skills.size()) scrollOffset++;
                        if (e2.mouseWheelScroll.delta > 0 && scrollOffset > 0) scrollOffset--;
                    }
                    if (e2.type == sf::Event::MouseButtonPressed && e2.mouseButton.button == sf::Mouse::Left && hover >= 0) {
                        // 檢查是否已在戰鬥技能欄
                        bool alreadyInBattle = false;
                        int battleIdx = -1;
                        for (size_t j = 0; j < battleSkills.size(); ++j) {
                            if (battleSkills[j].first == skills[hover].first) {
                                alreadyInBattle = true;
                                battleIdx = (int)j;
                                break;
                            }
                        }
                        if (alreadyInBattle) {
                            // 已在戰鬥技能欄，顯示提示
                            window.clear(sf::Color(30,30,30));
                            std::string msg = u8"此技能已在戰鬥技能欄！";
                            sf::Text msgT(sf::String::fromUtf8(msg.begin(), msg.end()), font, 24); msgT.setFillColor(sf::Color::White); msgT.setPosition(100, 200); window.draw(msgT);
                            window.display();
                            sf::sleep(sf::seconds(1));
                        } else if (battleSkills.size() < MaxBattleSkill) {
                            // 直接加入
                            bool ok = mgr.getPlayer().setbattleskill(hover);
                            window.clear(sf::Color(30,30,30));
                            std::string msg = ok ? u8"設為戰鬥技能成功！" : u8"設為戰鬥技能失敗";
                            sf::Text msgT(sf::String::fromUtf8(msg.begin(), msg.end()), font, 24); msgT.setFillColor(sf::Color::White); msgT.setPosition(100, 200); window.draw(msgT);
                            window.display();
                            sf::sleep(sf::seconds(1));
                        } else {
                            // 已滿，彈出選單讓玩家選要替換哪一個
                            bool ask = true;
                            int replaceIdx = -1;
                            while (ask && window.isOpen()) {
                                window.clear(sf::Color(30,30,30));
                                sf::Text ttitle(sf::String::fromUtf8(u8"戰鬥技能已滿，請選擇要替換哪一個：", u8"戰鬥技能已滿，請選擇要替換哪一個：" + strlen(u8"戰鬥技能已滿，請選擇要替換哪一個：")), font, 20);
                                ttitle.setFillColor(sf::Color::White);
                                ttitle.setPosition(40, 40);
                                window.draw(ttitle);
                                int yy = 100;
                                int hover2 = -1;
                                sf::Vector2i mousePos2 = sf::Mouse::getPosition(window);
                                for (size_t k = 0; k < battleSkills.size(); ++k) {
                                    std::string bstr = std::to_string(k) + ". " + battleSkills[k].first + " - " + battleSkills[k].second;
                                    sf::Text t(sf::String::fromUtf8(bstr.begin(), bstr.end()), font, 20);
                                    t.setFillColor(sf::Color::White);
                                    t.setPosition(60, yy);
                                    sf::FloatRect rect2(60, yy, 520, 32);
                                    if (rect2.contains(mousePos2.x, mousePos2.y)) {
                                        t.setFillColor(sf::Color(255,255,0));
                                        hover2 = (int)k;
                                    }
                                    window.draw(t);
                                    yy += 36;
                                }
                                window.display();
                                sf::Event e3;
                                while (window.pollEvent(e3)) {
                                    if (e3.type == sf::Event::Closed) { window.close(); ask = false; leave = true; }
                                    if (e3.type == sf::Event::KeyPressed && e3.key.code == sf::Keyboard::Escape) ask = false;
                                    if (e3.type == sf::Event::MouseButtonPressed && e3.mouseButton.button == sf::Mouse::Left && hover2 >= 0) {
                                        replaceIdx = hover2;
                                        ask = false;
                                    }
                                }
                                sf::sleep(sf::milliseconds(10));
                            }
                            if (replaceIdx >= 0) {
                                bool ok = mgr.getPlayer().changebattleskill(replaceIdx, hover);
                                window.clear(sf::Color(30,30,30));
                                std::string msg = ok ? u8"技能替換成功！" : u8"技能替換失敗";
                                sf::Text msgT(sf::String::fromUtf8(msg.begin(), msg.end()), font, 24); msgT.setFillColor(sf::Color::White); msgT.setPosition(100, 200); window.draw(msgT);
                                window.display();
                                sf::sleep(sf::seconds(1));
                            }
                        }
                    }
                }
                if (leave) break;
                sf::sleep(sf::milliseconds(10));
            }
            triggerSkill = false;
            continue;
        }
        if (triggerEquip) {
            // 圖像化換裝介面
            while (window.isOpen()) {
                window.clear(sf::Color(30,30,30));
                sf::Text title(sf::String::fromUtf8(u8"裝備列表 (ESC 離開)", u8"裝備列表 (ESC 離開)" + strlen(u8"裝備列表 (ESC 離開)")), font, 22);
                title.setFillColor(sf::Color::White);
                title.setPosition(30, 10);
                window.draw(title);
                auto equips = mgr.getPlayer().allEquipsDesc();
                // 取得 used 狀態
                std::vector<bool> usedVec;
                auto equipsObj = mgr.getPlayer().getEquips();
                for (size_t i = 0; i < equips.size(); ++i) {
                    usedVec.push_back(equipsObj.size() > i ? equipsObj[i].used : false);
                }
                int y = 60;
                int hover = -1;
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                for (size_t i = 0; i < equips.size(); ++i) {
                    std::string equipStr = std::to_string(i) + ". " + equips[i].first + " - " + equips[i].second;
                    if (usedVec[i]) equipStr = u8"★ " + equipStr;
                    sf::Text t(sf::String::fromUtf8(equipStr.begin(), equipStr.end()), font, 20);
                    t.setFillColor(sf::Color::White);
                    t.setPosition(40, y);
                    sf::FloatRect rect(40, y, 520, 32);
                    if (rect.contains(mousePos.x, mousePos.y)) {
                        t.setFillColor(sf::Color(255,255,0));
                        hover = (int)i;
                    }
                    window.draw(t);
                    y += 36;
                }
                window.display();
                bool leave = false;
                sf::Event e2;
                while (window.pollEvent(e2)) {
                    if (e2.type == sf::Event::Closed) { window.close(); leave = true; }
                    if (e2.type == sf::Event::KeyPressed && e2.key.code == sf::Keyboard::Escape) leave = true;
                    if (e2.type == sf::Event::MouseButtonPressed && e2.mouseButton.button == sf::Mouse::Left && hover >= 0) {
                        window.clear(sf::Color(30,30,30));
                        std::string msg;
                        if (mgr.getPlayer().getEquips()[hover].used) {
                            mgr.getPlayer().unequip(hover);
                            msg = u8"卸下成功！";
                        } else if (mgr.getPlayer().getEquippedCount() < MaxWearEquips) {
                            mgr.getPlayer().wearEquip(hover);
                            msg = u8"裝備成功！";
                        } else {
                            msg = u8"裝備已達上限！";
                        }
                        sf::Text msgT(sf::String::fromUtf8(msg.begin(), msg.end()), font, 24);
                        msgT.setFillColor(sf::Color::White);
                        msgT.setPosition(100, 200);
                        window.draw(msgT);
                        window.display();
                        sf::sleep(sf::seconds(1));
                    }
                }
                if (leave) break;
                sf::sleep(sf::milliseconds(10));
            }
            triggerEquip = false;
            continue;
        }
        if (moveDir != -1) {
            mgr.moveMainCharacter(moveDir);
            moveDir = -1;
            // 新增：移動後自動檢查是否踩到樓梯，若是則進入下一層
            if (mgr.reachedStairs()) {
                if (floor_ == MAX_FLOOR) {
                    if (boss_.empty()) {
                        std::cerr << "Boss資料載入失敗！" << std::endl;
                        break;
                    }
                    victory_ = BattleSFML(window, mgr.getPlayer(), boss_[rand() % boss_.size()]);
                    saveProgress();
                    break;
                }
                ++floor_;
                mgr.nextMap();
                mgr.spawnEnemies(pool_, 5 + floor_, floor_);
                mgr.spawnMerchants(rand() % 5 + 1, KMerchantData);
                mgr.putTextureBox(rand() % 5 + 1);
                // 重新取得新地圖資料
                mapData = &mgr.getMapGenerator().getMap();
                H = mapData->size();
                W = (*mapData)[0].size();
            }
        }
        // 轉換地圖格式給 drawAll
        std::vector<std::vector<int>> mapInt(H, std::vector<int>(W, 0));
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
                mapInt[y][x] = (int)(*mapData)[y][x];
        // 整理物件座標
        std::vector<std::pair<int, int>> enemies, merchants, treasureBoxes;
        for (const auto& e : mgr.getEnemies())
            if (!e.enemy.Died())
                enemies.push_back(e.pos);
        for (const auto& m : mgr.getMerchants()) merchants.push_back(m.pos);
        for (const auto& t : mgr.getTreasureBoxes()) treasureBoxes.push_back(t.pos);
        auto playerPos = mgr.getPlayerPos();
        auto stairsPos = mgr.getMapGenerator().getStairs();
        window.clear();
        sfmlMgr.drawAll(window, mapInt, playerPos, enemies, merchants, treasureBoxes, stairsPos, mgr.getVisible(), tileSize);
        window.display();
        // 新增：遊戲結束判斷
        if (mgr.isGameOver()) {
            sf::Text gameOverText(sf::String::fromUtf8(u8"Game Over", u8"Game Over"+strlen(u8"Game Over")), font, 48);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(W*tileSize/2-120, H*tileSize/2-40);
            window.draw(gameOverText);
            window.display();
            sf::sleep(sf::seconds(2));
            saveProgress();
            break;
        }
    }
    g_sfmlWindow = nullptr;
    g_sfmlFont = nullptr;
}

