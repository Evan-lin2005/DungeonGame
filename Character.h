#pragma once
#include "Addition.h"
#include "Item.h"

constexpr int MaxBattleSkill = 4;
constexpr int ExpPerLevel = 100;



enum class Target {
    NONE,  // 無目標
    SELF,  // 對自己施放
    ENEMY  // 對敵人施放
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

	//Item相關
    void wearEquip(int idx);
    void throwEquip(int idx);
    void getEquip(const Equip& e);
    void earnmoney(int type, int amount);//賺錢
	void earnmoney(const Money& amount);//賺錢
    void spendmoney(Money &amount);//花錢
	bool HaveEnoughMoney(const Money &m) const;//是否有足夠的錢
    void getMaterial(const Material& item);
	void throwMaterial(const Material& item);
	bool HaveEnoughMaterial(const std::vector<Material>& item) const;//是否有足夠的材料
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
private:
    void Levelup();
    bool nextlevel;
    std::string name;
    int MaxHp, currHp;
    int MaxMp, currMp;
    int Atk, Def;
    int exp, lv;
	float MissRate;//閃避

	Money money;//擁有金錢
    
    std::vector<Equip> Equips;//裝備
	std::vector<Skill> Skills;//學會的技能
	std::vector<Skill> BattleSkills;//戰鬥技能
	std::vector<Effect> Effects;//持續效果
    std::map<std::string,Material> MaterialBackpack;//背包物品(材料)
	std::map<std::string, MiseryItem> MyseryBackpack;//背包物品(道具)
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

    int gethp() const;
    int getmp() const;
    int getatk() const;
    int getdef() const;
    int getrank()const;
	float getMissRate() const;
	std::string getRace() const;
    std::string getSkillName() const;
	std::vector<Material> getFallBackpack() const;
    void upgrageByFloor(int floor);//隨著地下城深入而升級
    void CritizeByPlayerLv(int lv);
private:
    std::string Race;//種族(決定擁有技能)
    std::string name;
    int MaxHp, HP;
    int MaxMp, mp;
    int Atk, Def;
    int expGain;
	int Skillidx;// 技能索引
	float MissRate; // 閃避率
	int rank;// 階級(決定出現機率)
	std::vector<Material> FallBackpack; // 掉落物品
    std::vector<Skill> Skills;
    std::vector<Effect> Effects;
    SkillResult result;
    static std::map<std::string, std::vector<Skill>> allSkillBase;
};

bool Battle(Player& a, Enemy& b);

