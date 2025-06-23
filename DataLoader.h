#pragma once
// �w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w
// Enemy  ��Gname hp mp atk def exp missrate
// Player ��Gname lv exp hp mp atk def missrate
// �w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w�w
#include "Character.h"
#include "Merchant.h"
#include "treasurebox.h"
#include <string>
#include <fstream>
#include <sstream>



// =========== �@�ΡG�N�@����ά� token ============
inline std::vector<std::string> splitTokens(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

// -------- �@�ΡGŪ���U�@�榳�� tokens --------
inline bool nextTokens(std::ifstream& in, std::vector<std::string>& out) {
    std::string line;
    out.clear();
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        out = splitTokens(line);
        if (!out.empty()) return true;
    }
    return false; // EOF
}

// ================= �ĤH =================
inline Enemy makeEnemy(const std::vector<std::string>& t) {
    return Enemy(t[0],t[1], std::stoi(t[2]), std::stoi(t[3]),
        std::stoi(t[4]), std::stoi(t[5]), std::stoi(t[6]),stof(t[7]));
}

inline std::vector<Enemy> loadEnemies(const std::string& path) {
    std::vector<Enemy> list;
    std::ifstream in(path);
    if (!in) return list;

    std::vector<std::string> tk;
    while (nextTokens(in, tk)) {
        if (tk.size() >= 8) list.push_back(makeEnemy(tk));
    }
    return list;
}

// ================= ���a =================
inline Player loadPlayer(const std::string& path) {
    std::ifstream in(path);
    if (!in) return Player("Hero");

    std::vector<std::string> tk;
    if (!nextTokens(in, tk) || tk.size() < 8) return Player("Hero");

    return Player(tk[0], std::stoi(tk[1]), std::stoi(tk[2]),
        std::stoi(tk[3]), std::stoi(tk[4]),
        std::stoi(tk[5]), std::stoi(tk[6]),std::stof(tk[7]));
}

inline void savePlayer(const Player& p, const std::string& path) {
    std::ofstream out(path, std::ios::trunc);
    out << p.getname() << ' ' << p.getlv() << ' ' << p.getexp() << ' '
        << p.getMaxHp() << ' ' << p.getMaxMp() << ' ' << p.getatk() << ' ' << p.getdef()<<' ' << p.getMissRate() << '\n';
}

//================= �ӤH =================
inline void LoadMerchantData(Merchant& m, const std::string& path) {
    std::ifstream in(path);
    if (!in) return;

    // �� enum ��²�檺�u���A���v�A����Ū���Ϭq
    enum Section { None, MaterialSec, EquipmentSec, MysterySec } sec = None;
    std::string line;

    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        // �Ϭq���Y
        if (line == "Material") { sec = MaterialSec;  continue; }
        if (line == "Equipment") { sec = EquipmentSec; continue; }
        if (line == "MysteryItem") { sec = MysterySec;   continue; }

        std::istringstream iss(line);
        switch (sec) {
        case MaterialSec: {
            std::string name, desc;
            int rank, amount, gold, sliver, cooper;
            if (!(iss >> name >> desc >> rank >> amount >> gold >> sliver >> cooper)) break;
            m.getMaterialList(
                SellMaterial{
                    Material{name, desc, rank, amount},
                    Money   {gold, sliver, cooper}
                }
            );
            break;
        }
        case EquipmentSec: {
            std::string name, desc;
            int hp, atk, def, mp, rankVal, gold, sliver, cooper;
            if (!(iss >> name >> desc
                >> hp >> atk >> def >> mp
                >> rankVal
                >> gold >> sliver >> cooper))
                break;

            // Ū���o��˳Ʃһݪ������M��
            std::vector<Material> materials;
            std::string mn, md;
            int mr, ma;
            while (iss >> mn >> md >> mr >> ma) {
                materials.emplace_back(mn, md, mr, ma);
            }

            m.getEquipmentList(
                SellEquip{
                    // Equip(name, desc, rank, round, ahp, aatk, adef, amp)
                    Equip{name, desc, rankVal, 0, hp, atk, def, mp},
                    Money{gold, sliver, cooper},
                    materials
                }
            );
            break;
        }
        case MysterySec: {
            std::string name, desc, effdesc;
            int ahp, aatk, adef, amp, cnt, rankVal, gold, sliver, cooper, amount;
            if (!(iss >> name >> desc >> effdesc
                >> ahp >> aatk >> adef >> amp >> cnt
                >> rankVal
                >> gold >> sliver >> cooper >> amount))
                break;
            m.getMiseryList(
                SellMisery{
                    MiseryItem{
                        name, desc, rankVal,
                        Effect{effdesc, ahp, aatk, adef, amp, cnt},
                        amount
                    },
                    Money{gold, sliver, cooper}
                }
            );
            break;
        }
        default:
            break;
        }
    }
}

//==================Ū���ޯ�==================
inline void learnPlayerSkill(Player& p, const std::string& skillFilePath) {
    std::ifstream in(skillFilePath);
    if (!in) return;

    std::string skillLine;
    while (std::getline(in, skillLine)) {
        if (!skillLine.empty() && skillLine[0] != '#') {
            std::vector<std::string> tokens = splitTokens(skillLine);
            if (tokens.size() == 6) { // Assuming Skill requires at least 6 tokens
                Skill newSkill(tokens[0], tokens[1], std::stof(tokens[2]),
                               std::stoi(tokens[3]), std::stoi(tokens[4]), std::stoi(tokens[5]));
                p.learnskill(newSkill);
            }
        }
    }
}

//==================�_�c==================
inline void loadTreasureBox(const std::string& path) {
    std::ifstream in(path);
    if (!in)return;
    std::string line;

    enum Section { None, MaterialSec, EquipmentSec, MysterySec } sec = None;

    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;

        // �Ϭq���Y
        if (line == "Material") { sec = MaterialSec;  continue; }
        if (line == "Equipment") { sec = EquipmentSec; continue; }
        if (line == "MysteryItem") { sec = MysterySec;   continue; }

        std::istringstream iss(line);
        switch (sec) {
        case MaterialSec: {
            std::string name, desc;
            int rank, amount;
            if ((iss >> name >> desc >> rank >> amount)) {
                TreasureBox::allMaterial.push_back(Material(name, desc, rank, amount));
            }
            
            break;
        }
        case EquipmentSec: {
            std::string name, desc;
            int hp, atk, def, mp, rankVal;
            if ((iss >> name >> desc
                >> rankVal >> hp >> atk >> def
                >> mp)) {
                   TreasureBox::allEquip.push_back(Equip(name,desc,rankVal,0,hp,atk,def,mp));
            }
                
                break;
        }
        case MysterySec: {
            std::string name, desc, effdesc;
            int ahp, aatk, adef, amp, cnt, rankVal, amount;
            if ((iss >> name >> desc >> effdesc
                >> ahp >> aatk >> adef >> amp >> cnt
                >> rankVal >> amount
                ))
            {
                TreasureBox::allMiseryItem.push_back(MiseryItem{ name,desc,rankVal,Effect(effdesc,ahp,aatk,adef,amp,cnt),amount });
            }
            break;
        }
        default:
            break;
        }
    }
}









