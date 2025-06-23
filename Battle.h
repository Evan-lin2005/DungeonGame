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
        std::cout << "�S���������~\n";
        return;
    }
    std::cout << "�������~:\n";
    for (const auto& item : items) {
        std::cout << item.getName() << "---" << item.getDesc() << "\n";
    }
}

inline int choose() {
    while (true) {
        std::cout << "�п��:1.���� 2.�ޯ�> ";
        std::string idx; std::cin >> idx;
        if (idx == "1" || idx == "2") return stoi(idx);
        std::cout << "�L�ľާ@\n";
    }
}

inline void printallskill(const Player& a) {
    auto list = a.allskillsDesc();
    for (int i = 0; i < static_cast<int>(list.size()); ++i)
        std::cout << i << ":" << list[i].first << "(" << list[i].second << ")\n";
}

inline void getallItem(Player& a, const std::vector<Material>& items) {
    if (items.empty()) return;
    std::cout << "��o���~:\n";
    for (const auto& item : items) {
        a.getMaterial(item);
        std::cout << item.getName() << "---" << item.getDesc() << "\n";
    }
}

// Battle.h ���ק�᪺ Battle() �禡
inline bool Battle(Player& a, Enemy& b) {
    clearScreen();
    std::cout << "�D�J�F�Ǫ�!!!\n"
        << "�ĤH: " << b.getname()
        << " HP=" << b.gethp()
        << " MP=" << b.getmp() << "\n";

    int round = 0;
    while (true) {
        ++round;
        std::cout << "--- Battle Round " << round << " ---\n";

        // �^�X�}�l�G���a�P�ĤH�U�۳B�z����ĪG
        a.Affected();
        b.Affected();

        // ==== ���a�^�X ====
        int choice = choose();  // 1. ���� 2. �ޯ�
        SkillResult playerRes;
        if (choice == 1) {
            playerRes.immediateDamage = a.Basicattack();
			std::cout << a.getname() << " �i������I\n";
        }
        else {
            printallskill(a);
            int idx; std::cin >> idx;
            playerRes = a.useSkill(idx);
            std::cout<< a.getname() << " �ϥΤF�ޯ� "
				<< a.getBattleSkillName(idx) << "�I\n";
        }

        // 1) ���a��������o�ĪG
        if (playerRes.effect) {
            // �P�w�ĤH�O�_�{�צ��ĪG
            if (rand() % 100 < b.getMissRate()) {
                std::cout << b.getname() << " �{�פF�ĪG "
                    << playerRes.effect->Desc << "�I\n";
            }
            else {
                if (playerRes.target == Target::SELF) {
                    a.BeEffect(*playerRes.effect);
                    std::cout << a.getname() << " ��o�F "
                        << playerRes.effect->Desc << "�I\n";
                }
                else {
                    b.BeEffect(*playerRes.effect);
                    std::cout << b.getname() << " �Q�I�[�F "
                        << playerRes.effect->Desc << "�I\n";
                }
            }
        }

        // 2) �B�z���a�ߧY�ˮ`
        if (playerRes.immediateDamage > 0) {
            if (rand() % 100 < b.getMissRate()) {
                std::cout << b.getname() << " �{�פF�A�������I\n";
            }
            else {
                int dmg = b.BeAttacked(playerRes.immediateDamage);
                std::cout << a.getname() << " �� " << b.getname()
                    << " �y���F " << dmg << " �I�ˮ`�I\n";
            }
        }

        // ��ܼĤH���A
        std::cout << b.getname() << " HP=" << b.gethp()
            << " MP=" << b.getmp() << "\n";

        // �ˬd�ĤH�O�_���`
        if (b.Died()) {
            a.earnedexp(b.Giveexp());
            getallItem(a, b.getFallBackpack());
            printItem(b.getFallBackpack());
            if (a.goToNextLevel())
                std::cout << a.getname() << " �ɯ� !!!\n";
            std::cout << "Victory!\n";
			system("pause");
            return true;
        }
        std::cout << "\n";

        // ==== �ĤH�^�X ====
        SkillResult enemyRes = b.Attack(a);  // �ĤH�M�w���
        std::cout<<b.getname()<<"�ϥ�" << b.getSkillName() << " �� " << a.getname()
			<< " �o�ʤF�����I\n";
        // 1) �B�z�ĤH��������o�ĪG
        if (enemyRes.effect) {
            if (rand() % 100 < a.getMissRate()) {
                std::cout << a.getname() << " �{�פF�ĪG "
                    << enemyRes.effect->Desc << "�I\n";
            }
            else {
                if (enemyRes.target == Target::SELF) {
                    b.BeEffect(*enemyRes.effect);
                    std::cout << b.getname() << " ��o�F "
                        << enemyRes.effect->Desc << "�I\n";
                }
                else {
                    a.BeEffect(*enemyRes.effect);
                    std::cout << a.getname() << " �Q�I�[�F "
                        << enemyRes.effect->Desc << "�I\n";
                }
            }
        }

        // 2) �B�z�ĤH�ߧY�ˮ`
        if (enemyRes.immediateDamage > 0) {
            if (rand() % 100 < a.getMissRate()) {
                std::cout << a.getname() << " �{�פF�ĤH�������I\n";
            }
            else {
                int dmg = a.Beattacked(enemyRes.immediateDamage);
                std::cout << b.getname() << " �� " << a.getname()
                    << " �y���F " << dmg << " �I�ˮ`�I\n";
            }
        }

        // ��ܪ��a���A
        std::cout << a.getname() << " HP=" << a.gethp()
            << " MP=" << a.getmp() << "\n";
        if (a.Died()) {
            std::cout << "Defeated...\n";
            return false;
        }
        std::cout << "\n";
    }
}

