#pragma once
#include "Character.h"
#include <iostream>
#include <vector>
#include <cstdlib>

inline void clearScreen(){
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\x1B[2J\x1B[H";
#endif
}

inline void printItem(const std::vector<Material>& items) {
    if (items.empty()) {
        std::cout << "（沒有獲得任何道具）\n";
        return;
    }
    std::cout << "獲得道具:\n";
    for (const auto& item : items) {
        std::cout << item.getName() << "---" << item.getDesc() << "\n";
    }
}

inline int choose() {
    while (true) {
        std::cout << "請選擇:1.攻擊 2.技能 3.道具> ";
        std::string idx; std::cin >> idx;
        if (idx == "1" || idx == "2"||idx == "3") return stoi(idx);
        std::cout << "輸入錯誤\n";
    }
}

inline void printallskill(const Player& a) {
    auto list = a.allskillsDesc();
    for (int i = 0; i < static_cast<int>(list.size()); ++i)
        std::cout << i << ":" << list[i].first << "(" << list[i].second << ")\n";
}

inline void getallItem(Player& a, const std::vector<Material>& items) {
    if (items.empty()) return;
    std::cout << "獲得道具:\n";
    for (const auto& item : items) {
        a.getMaterial(item);
        std::cout << item.getName() << "---" << item.getDesc() << "\n";
    }
}

inline void playerUseItemDuringBattle(Player& a, Enemy& b) {
    if (a.getMyseryBackpack().empty()) {
        std::cout << "你沒有可以使用的道具。\n";
        system("pause");
        return;
    }

    std::cout << "可用道具如下:\n";
    int idx = 0;
    std::vector<std::string> items;
    for (auto& it : a.getMyseryBackpack()) {
        std::cout << idx << ". " << it.second.getName() << " - " << it.second.getDesc() << "\n";
        items.push_back(it.first);
        idx++;
    }

    std::cout << "請輸入要使用的道具編號:";
    int choice;
    std::cin >> choice;
    if (choice < 0 || choice >= (int)items.size()) {
        std::cout << "輸入錯誤，無法使用。\n";
        system("pause");
        return;
    }

    const std::string& itemName = items[choice];
    auto it = a.getMyseryBackpack().find(itemName);
    if (it == a.getMyseryBackpack().end()) return;

    Effect e = it->second.itemEffect;

    bool reversed = (rand() % 100 < 30);
    if (reversed) {
        std::cout << "[道具反噬!] 效果將反向\n";
        e.affectHp = -e.affectHp;
        e.affectMp = -e.affectMp;
        e.affectAtk = -e.affectAtk;
        e.affectDef = -e.affectDef;
        e.affectMissRate = -e.affectMissRate;
    }

    // 根據效果決定加在誰身上
    bool isBuff = (e.affectHp > 0 || e.affectMp > 0 || e.affectAtk > 0 || e.affectDef > 0 || e.affectMissRate > 0);

    if (isBuff) {
        std::cout << a.getname() << " 獲得 " << it->second.getName() << " 的效果。\n";
        a.BeEffect(e);
    }
    else {
        std::cout << b.getname() << " 承受了 " << it->second.getName() << " 的效果。\n";
        b.BeEffect(e);
    }

    a.throwMyseryItem(it->second);
    system("pause");
}

// 戰鬥顯示/輸入介面
struct BattleUI {
    virtual void showState(const Player&, const Enemy&, const std::string& info) = 0;
    virtual int getPlayerAction(const Player&, const Enemy&) = 0; // 1=攻擊 2=技能 3=道具
    virtual void showResult(const std::string& result) = 0;
    virtual void wait() = 0;
    virtual ~BattleUI() {}
};

// Console 版 BattleUI
struct ConsoleBattleUI : public BattleUI {
    void showState(const Player& a, const Enemy& b, const std::string& info) override {
        clearScreen();
        std::cout << info << std::endl;
        std::cout << "玩家: " << a.getname() << " HP=" << a.gethp() << "/" << a.getMaxHp() << " MP=" << a.getmp() << "/" << a.getMaxMp() << std::endl;
        std::cout << "敵人: " << b.getname() << " HP=" << b.gethp()<< " MP=" << b.getmp() << std::endl;
    }
    int getPlayerAction(const Player& a, const Enemy& b) override {
        return choose();
    }
    void showResult(const std::string& result) override {
        std::cout << result << std::endl;
    }
    void wait() override { system("pause"); }
};

// 新版 Battle：可插拔 UI
bool Battle(Player& a, Enemy& b, BattleUI* ui);
// 保留原本 console 互動的 Battle
inline bool Battle(Player& a, Enemy& b) {
    ConsoleBattleUI ui;
    return Battle(a, b, &ui);
}

