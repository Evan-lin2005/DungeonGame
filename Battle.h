#pragma once
#include "Character.h"
#include <iostream>
#include <vector>
#include <cstdlib>

inline void clearScreen(){
#ifdef _WIN32
    system("cls");
#else
    std::cout<<"\x1B[2J\x1B[H";
#endif
}

inline void printItem(const std::vector<Material>& items) {
    if (items.empty()) {
        std::cout << "沒有掉落物品\n";
        return;
    }
    std::cout << "掉落物品:\n";
    for (const auto& item : items) {
        std::cout << item.getName() << "---" << item.getDesc() << "\n";
    }
}

inline int choose() {
    while (true) {
        std::cout << "請選擇:1.普攻 2.技能> ";
        std::string idx; std::cin >> idx;
        if (idx == "1" || idx == "2") return stoi(idx);
        std::cout << "無效操作\n";
    }
}

inline void printallskill(const Player& a) {
    auto list = a.allskillsDesc();
    for (int i = 0; i < static_cast<int>(list.size()); ++i)
        std::cout << i << ":" << list[i].first << "(" << list[i].second << ")\n";
}

inline void getallItem(Player& a, const std::vector<Material>& items) {
    if (items.empty()) return;
    std::cout << "獲得物品:\n";
    for (const auto& item : items) {
        a.getMaterial(item);
        std::cout << item.getName() << "---" << item.getDesc() << "\n";
    }
}

// Battle.h 內修改後的 Battle() 函式
inline bool Battle(Player& a, Enemy& b) {
    clearScreen();
    std::cout << "遭遇了怪物!!!\n"
        << "敵人: " << b.getname()
        << " HP=" << b.gethp()
        << " MP=" << b.getmp() << "\n";

    int round = 0;
    while (true) {
        ++round;
        std::cout << "--- Battle Round " << round << " ---\n";

        // 回合開始：玩家與敵人各自處理持續效果
        a.Affected();
        b.Affected();

        // ==== 玩家回合 ====
        int choice = choose();  // 1. 普攻 2. 技能
        SkillResult playerRes;
        if (choice == 1) {
            playerRes.immediateDamage = a.Basicattack();
			std::cout << a.getname() << " 進行攻擊！\n";
        }
        else {
            printallskill(a);
            int idx; std::cin >> idx;
            playerRes = a.useSkill(idx);
            std::cout<< a.getname() << " 使用了技能 "
				<< a.getBattleSkillName(idx) << "！\n";
        }

        // 1) 玩家持續／瞬發效果
        if (playerRes.effect) {
            // 判定敵人是否閃避此效果
            if (rand() % 100 < b.getMissRate()) {
                std::cout << b.getname() << " 閃避了效果 "
                    << playerRes.effect->Desc << "！\n";
            }
            else {
                if (playerRes.target == Target::SELF) {
                    a.BeEffect(*playerRes.effect);
                    std::cout << a.getname() << " 獲得了 "
                        << playerRes.effect->Desc << "！\n";
                }
                else {
                    b.BeEffect(*playerRes.effect);
                    std::cout << b.getname() << " 被施加了 "
                        << playerRes.effect->Desc << "！\n";
                }
            }
        }

        // 2) 處理玩家立即傷害
        if (playerRes.immediateDamage > 0) {
            if (rand() % 100 < b.getMissRate()) {
                std::cout << b.getname() << " 閃避了你的攻擊！\n";
            }
            else {
                int dmg = b.BeAttacked(playerRes.immediateDamage);
                std::cout << a.getname() << " 對 " << b.getname()
                    << " 造成了 " << dmg << " 點傷害！\n";
            }
        }

        // 顯示敵人狀態
        std::cout << b.getname() << " HP=" << b.gethp()
            << " MP=" << b.getmp() << "\n";

        // 檢查敵人是否死亡
        if (b.Died()) {
            a.earnedexp(b.Giveexp());
            getallItem(a, b.getFallBackpack());
            printItem(b.getFallBackpack());
            if (a.goToNextLevel())
                std::cout << a.getname() << " 升級 !!!\n";
            std::cout << "Victory!\n";
			system("pause");
            return true;
        }
        std::cout << "\n";

        // ==== 敵人回合 ====
        SkillResult enemyRes = b.Attack(a);  // 敵人決定行動
        std::cout<<b.getname()<<"使用" << b.getSkillName() << " 對 " << a.getname()
			<< " 發動了攻擊！\n";
        // 1) 處理敵人持續／瞬發效果
        if (enemyRes.effect) {
            if (rand() % 100 < a.getMissRate()) {
                std::cout << a.getname() << " 閃避了效果 "
                    << enemyRes.effect->Desc << "！\n";
            }
            else {
                if (enemyRes.target == Target::SELF) {
                    b.BeEffect(*enemyRes.effect);
                    std::cout << b.getname() << " 獲得了 "
                        << enemyRes.effect->Desc << "！\n";
                }
                else {
                    a.BeEffect(*enemyRes.effect);
                    std::cout << a.getname() << " 被施加了 "
                        << enemyRes.effect->Desc << "！\n";
                }
            }
        }

        // 2) 處理敵人立即傷害
        if (enemyRes.immediateDamage > 0) {
            if (rand() % 100 < a.getMissRate()) {
                std::cout << a.getname() << " 閃避了敵人的攻擊！\n";
            }
            else {
                int dmg = a.Beattacked(enemyRes.immediateDamage);
                std::cout << b.getname() << " 對 " << a.getname()
                    << " 造成了 " << dmg << " 點傷害！\n";
            }
        }

        // 顯示玩家狀態
        std::cout << a.getname() << " HP=" << a.gethp()
            << " MP=" << a.getmp() << "\n";
        if (a.Died()) {
            std::cout << "Defeated...\n";
            return false;
        }
        std::cout << "\n";
    }
}

