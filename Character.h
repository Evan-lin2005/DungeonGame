#pragma once
#include "Addition.h"
#include "Item.h"

constexpr int MaxBattleSkill = 6;
constexpr int ExpPerLevel = 100;



enum class Target {
    NONE,  
    SELF,  
    ENEMY  
};

#include <memory>

struct SkillResult {
    int immediateDamage = 0;
    std::unique_ptr<Effect> effect;  
    Target target = Target::NONE;

    SkillResult() = default;

    SkillResult(const SkillResult& rhs)
        : immediateDamage(rhs.immediateDamage),
        target(rhs.target),
        effect(rhs.effect ? std::make_unique<Effect>(*rhs.effect) : nullptr) {
    }

    SkillResult& operator=(const SkillResult& rhs) {
        if (this != &rhs) {
            immediateDamage = rhs.immediateDamage;
            target = rhs.target;
            effect = rhs.effect ? std::make_unique<Effect>(*rhs.effect) : nullptr;
        }
        return *this;
    }

    SkillResult(SkillResult&&) noexcept = default;
    SkillResult& operator=(SkillResult&&) noexcept = default;
};


class Player {
public:
    Player(const std::string& name);
    Player(const std::string& name, int lv, int exp, int hp, int mp, int atk, int def,float missRate);
    Player& operator=(const Player& P);
    Player(const Player& P);
    ~Player();

	//Item
    void wearEquip(int idx);
    void throwEquip(int idx);
    void getEquip(const Equip& e);
    void earnmoney(int type, int amount);
	void earnmoney(const Money& amount);
    void spendmoney(Money &amount);
	bool HaveEnoughMoney(const Money &m) const;
    void getMaterial(const Material& item);
	void throwMaterial(const Material& item);
	bool HaveEnoughMaterial(const std::vector<Material>& item) const;
	void getMyseryItem(const MiseryItem& item);
	void throwMyseryItem(const MiseryItem& item);

    int Basicattack() const;
    SkillResult useSkill(int idx);

    int Beattacked(int atk);
    void BeEffect(const Effect& e);
    void Affected();
    bool Died() const;
    const Money& showmoney();

    void learnskill(const Skill& skill);
    bool setbattleskill(int idx);
	bool changebattleskill(int idx1,int idx2);

    std::vector<std::pair<std::string, std::string>> allskillsDesc() const;
    std::vector<std::pair<std::string, std::string>> allLearnskillsDesc() const;
	std::vector<std::pair<std::string, std::string>> allEquipsDesc() const;

	bool canEquip()const;

    std::string getname() const;
    int gethp() const;
    int getmp() const;
    int getatk() const;
    int getdef() const;
    int getlv() const;
    int getexp()const;
    int getMaxHp()const;
	int getMaxMp()const;
	int getEquipSize() const;
    std::string getSkillName(int idx) const;
	std::string getBattleSkillName(int idx) const;
    const std::map<std::string, MiseryItem>& getMyseryBackpack()const;

    void earnedexp(int exp);

    bool goToNextLevel()const;

    float getMissRate()const;
    std::vector<std::string> listEffectsDesc() const;
    // 新增：取得裝備列表（只讀）
    const std::vector<Equip>& getEquips() const;
private:
    void Levelup();
    bool nextlevel;
    std::string name;
    int MaxHp, currHp;
    int MaxMp, currMp;
    int Atk, Def;
    int exp, lv;
	float MissRate;//�{��

	Money money;//�֦�����
    
    std::vector<Equip> Equips;
	std::vector<Skill> Skills;
	std::vector<Skill> BattleSkills;
	std::vector<Effect> Effects;
    std::map<std::string,Material> MaterialBackpack;
	std::map<std::string, MiseryItem> MyseryBackpack;
};

class Enemy {
public:
    Enemy(const std::string &race,const std::string& name,
        int hp, int mp, int atk, int def, int expGain,float missRate,int rank);
    Enemy& operator= (const Enemy& E);
    ~Enemy();
    std::string getname() const;
    SkillResult Attack(Player& a);
    int BeAttacked(int atk);
    void BeEffect(const Effect& e);
    void Affected();
    bool Died() const;
    int Giveexp() const;

    void learnskill(const Skill& skill);

	int getMaxhp() const;
	int getMaxmp() const;
    int gethp() const;
    int getmp() const;
    int getatk() const;
    int getdef() const;
    int getrank()const;
	float getMissRate() const;
	std::string getRace() const;
    std::string getSkillName() const;
    std::vector<Material> getFallBackpack() const;
    std::vector<std::string> listEffectsDesc() const;
    void upgrageByFloor(int floor);
    void CritizeByPlayerLv(int lv);
private:
    std::string Race;
    std::string name;
    int MaxHp, HP;
    int MaxMp, mp;
    int Atk, Def;
    int expGain;
	int Skillidx;
	float MissRate;
	int rank;
	std::vector<Material> FallBackpack;
    std::vector<Skill> Skills;
    std::vector<Effect> Effects;
    SkillResult result;
    static std::map<std::string, std::vector<Skill>> allSkillBase;
};

bool Battle(Player& a, Enemy& b);

