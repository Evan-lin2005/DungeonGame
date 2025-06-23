#include "Character.h"

// Player
Player::Player(const std::string& name)
    : name(name), MaxHp(100), currHp(100),
    MaxMp(100), currMp(100), Atk(10), Def(5),
    exp(0), lv(1),nextlevel(false),MissRate(0) {
}

Player::Player(const std::string& name, int lv, int exp, int hp, int mp, int atk, int def, float missRate):
    name(name),MaxHp(hp),currHp(hp),MaxMp(mp),currMp(mp),Atk(atk),Def(def),lv(lv),exp(exp),nextlevel(false),MissRate(missRate)
{}

Player& Player::operator=(const Player& P)
{
    if (this == &P) {
        return *this;
    }
    name = P.name;
    MaxHp = P.MaxHp; currHp = P.currHp;
    MaxMp = P.MaxMp; currMp = P.currMp;
    Atk = P.Atk; Def = P.Def;
    lv = P.lv; exp = P.exp;
    Skills = (P.Skills);
    Equips = (P.Equips);
    BattleSkills = (P.BattleSkills);
    Effects = P.Effects;
	nextlevel = P.nextlevel;
	MissRate = P.MissRate;
    return *this;
}

Player::Player(const Player& P)
    : name(P.name), MaxHp(P.MaxHp), currHp(P.currHp), MaxMp(P.MaxMp), currMp(P.currMp),
    Atk(P.Atk), Def(P.Def), exp(P.exp), lv(P.lv),
	Equips(P.Equips), Skills(P.Skills), BattleSkills(P.BattleSkills), Effects(P.Effects), nextlevel(P.nextlevel),MissRate (P.MissRate)
	
{}

Player::~Player() {}

void Player::wearEquip(int idx) {
    if (idx < 0 || idx >= Equips.size()) return;
    auto& it = Equips[idx];
    if (!it.used) {
        MaxHp += it.affectHp;
        Atk += it.affectAtk;
        Def += it.affectDef;
        MaxMp += it.affectMp;
		it.used = true;// 標記為已裝備
    }
}

void Player::throwEquip(int idx) {
    if (idx < 0 || idx >= Equips.size()) return;
    auto& it = Equips[idx];
	if (it.used) {// 如果裝備中，則卸下要減去其提供之屬性
        MaxHp -= it.affectHp;
        Atk -= it.affectAtk;
        Def -= it.affectDef;
        MaxMp -= it.affectMp;
    }
    Equips.erase(Equips.begin() + idx);
}

void Player::getEquip(const Equip& e) {
    Equips.push_back(e);
}

SkillResult Player::useSkill(int idx) {
    SkillResult result; // 建立回傳結果物件
    if (idx < 0 || idx >= BattleSkills.size()) return result;

    const Skill& s = BattleSkills[idx];
    if (currMp < s.costPower) return result; // 魔力不足，直接回傳空結果

    currMp = std::max(currMp - s.costPower, 0);

    // 處理有持續效果的技能
    if (s.affectRound > 0) {
        // 建立效果物件，並由 result 的指標持有
        result.effect = std::make_unique<Effect>(s.Desc + "效果", 0, 0, 0, 0, 0,s.affectRound);

        // 根據技能類型，決定效果內容與目標
        switch (s.Type) {
        case 1: // 治療 -> 目標:自己
            result.effect->affectHp = int(s.Power);
            result.target = Target::SELF;
            break;
        case 2: // 攻擊增益 -> 目標:自己
            result.effect->affectAtk = int(s.Power);
            result.target = Target::SELF;
            break;
        case 3: // 防禦增益 -> 目標:自己
            result.effect->affectDef = int(s.Power);
            result.target = Target::SELF;
            break;
        case 4: // 魔力恢復 -> 目標:自己
            result.effect->affectMp = int(s.Power);
            result.target = Target::SELF;
            break;
        case 5://閃避提升
            result.effect->affectMissRate = int(s.Power);
            result.target = Target::SELF;
			break;
        //對敵人的減益效果 (debuff)
		case 6: // 生命減益 -> 目標:敵人
            result.effect->affectHp = -int(s.Power); 
            result.target = Target::ENEMY;
            break;
		case 7: // 攻擊減益 -> 目標:敵人
            result.effect->affectAtk = -int(s.Power); 
            result.target = Target::ENEMY; 
            break;
		case 8: // 防禦減益 -> 目標:敵人
            result.effect->affectDef = -int(s.Power); 
            result.target = Target::ENEMY; 
            break;
		case 9: // 魔力減益 -> 目標:敵人
            result.effect->affectMp = -int(s.Power); 
            result.target = Target::ENEMY; 
            break;
        case 10://提高自身命中
            result.effect->affectMissRate = int(s.Power);
            result.target = Target::ENEMY;
			break;
        }
        return result;
    }
    long long dmg;
    // 處理立即生效的技能
    switch (s.Type) {
    case 1: currHp = std::min(std::max(currHp + int(s.Power), 0), MaxHp); break;
    case 2:
        dmg = 1LL * Atk * s.Power;
        result.immediateDamage = dmg;
        break;
    case 3: Def = std::max(0, Def + int(s.Power)); break;
    case 4: currMp = std::min(std::max(currMp + int(s.Power), 0), MaxMp); break;
	case 5: MissRate = std::min(std::max(MissRate + s.Power, 0.0f), 1.0f); break;
    }
    return result;
}

int Player::Basicattack() const {
    return Atk;
}

int Player::Beattacked(int atk) {
    int damage = std::max(atk - (Def * 0.4), 0.0); // 計算實際傷害
    currHp -= damage;
    return damage; // 回傳傷害值
}

// Player::BeEffect
void Player::BeEffect(const Effect& e) {
    // 效果施加時，立即改變一次性屬性 (攻/防/閃避)
    Atk = std::max(0, Atk + e.affectAtk);
    Def = std::max(0, Def + e.affectDef);
    MissRate = std::min(std::max(MissRate + e.affectMissRate, 0.0f), 1.0f);
    Effects.push_back(e);
}

// Player::Affected
void Player::Affected() {
    for (auto it = Effects.begin(); it != Effects.end();) {
        // 1. 每回合持續生效的效果 (HP/MP)
        currHp = std::min(currHp + it->affectHp, MaxHp);
        currMp = std::min(currMp + it->affectMp, MaxMp);

        // 2. 時效遞減
        it->nextRound();

        // 3. 效果結束 → 還原屬性並刪除
        if (!it->existEffect()) {
            Atk = std::max(0, Atk - it->affectAtk);
            Def = std::max(0, Def - it->affectDef);
            MissRate = std::min(std::max(MissRate - it->affectMissRate, 0.0f), 1.0f);
            it = Effects.erase(it);
        }
        else {
            ++it;
        }
    }
}


bool Player::Died() const { return currHp <= 0; }

void Player::learnskill(const Skill& skill) {
    Skills.push_back(skill);
}

bool Player::setbattleskill(int idx) {
    if (idx < 0 || idx >= Skills.size()) return false;
    if(BattleSkills.size() >= MaxBattleSkill) {
        // 如果已經達到最大戰鬥技能數量，則不添加
        return false;
	}
    if (BattleSkills.size() < MaxBattleSkill)
        BattleSkills.push_back(Skills[idx]);
    return true;
}

bool Player::changebattleskill(int idx1, int idx2)
{
    if(idx1<0||idx1>= BattleSkills.size() || idx2 < 0 || idx2 >= Skills.size()) {
        return false; // 索引不合法
	}
	BattleSkills.erase(BattleSkills.begin() + idx1); // 刪除原有技能
    BattleSkills.push_back(Skills[idx2]); // 添加新技能
	return true;
}

std::vector<std::pair<std::string, std::string>> Player::allskillsDesc() const
{
    std::vector<std::pair<std::string, std::string>> Result;
    for (auto& it : BattleSkills) {
        Result.push_back({ it.Name,it.Desc });
    }
	return Result;
}

std::vector<std::pair<std::string, std::string>> Player::allLearnskillsDesc() const
{
    std::vector<std::pair<std::string, std::string>> Result;
    for (auto& it : Skills) {
        Result.push_back({ it.Name,it.Desc });
    }
    return Result;
}

std::vector<std::pair<std::string, std::string>> Player::allEquipsDesc() const
{
    std::vector<std::pair<std::string, std::string>> Result;
    for (auto& it : Equips) {
        Result.push_back({ it.name,it.desc });
    }
    return Result;
}

bool Player::canEquip() const
{
    return Equips.size() <= MaxEquipCapacity;
}




std::string Player::getname() const { return name; }
int Player::gethp() const { return currHp; }
int Player::getmp() const { return currMp; }
int Player::getatk() const { return Atk; }
int Player::getdef() const { return Def; }
int Player::getlv() const{return lv;}
int Player::getexp() const{return exp;}

int Player::getMaxHp() const
{
    return MaxHp;
}

int Player::getMaxMp() const
{
    return MaxMp;
}

int Player::getEquipSize() const
{
    return Equips.size();
}

std::string Player::getSkillName(int idx) const
{
    return Skills[idx].Name;
}

std::string Player::getBattleSkillName(int idx) const
{
	return BattleSkills[idx].Name;
}

void Player::earnedexp(int e) {
	nextlevel = false; 
    exp += e;
    Levelup();
}

void Player::getMaterial(const Material& item) {
    auto res = MaterialBackpack.emplace(
        std::make_pair(
            item.getName(),
            Material(item.getName(),
                item.getDesc(),
                item.arity,
                item.amount)
        )
    );

    auto it = res.first;
    bool inserted = res.second;

    if (!inserted) {
        it->second.amount += item.amount;
    }
}

void Player::throwMaterial(const Material& item)
{
    if( MaterialBackpack.find(item.getName()) != MaterialBackpack.end()) {
        auto& it = MaterialBackpack[item.getName()];
        if (it.amount >= 1) {
            it.amount--;
        }
        if (it.amount <= 0) {
			MaterialBackpack.erase(item.getName());
        }
	}
}

bool Player::HaveEnoughMaterial(const std::vector<Material>& item) const
{
    int totalaccept = item.size();int have = 0;
    for(auto& it : item) {
        for(auto& m : MaterialBackpack) {
            if (m.first == it.getName()) {
                have ++;
                break;
            }
		}
	}
	return have >= totalaccept;
}

void Player::getMyseryItem(const MiseryItem& item)
{
    auto res = MyseryBackpack.emplace(
        std::make_pair(
            item.getName(),
            MiseryItem(item.getName(),
                item.getDesc(),item.arity,
                item.itemEffect,item.amount)
        )
    );
    auto it = res.first;
    bool inserted = res.second;
    if (!inserted) {
        it->second.amount+=item.amount;
	}
}

void Player::throwMyseryItem(const MiseryItem& item)
{
    auto it = MyseryBackpack.find(item.getName());
    if (it != MyseryBackpack.end()) {
        // 不會產生新的 default‐constructed MiseryItem
        it->second.amount--;
        if (it->second.amount <= 0)
            MyseryBackpack.erase(it);
    }
}



void Player::earnmoney(int type, int amount)
{
    if (type == 0) { // 金幣
        money.Gold += amount;
    }
    else if (type == 1) { // 銀幣
        money.Sliver += amount;
    }
    else if (type == 2) { // 銅幣
        money.Cooper += amount;
	}
}

void Player::earnmoney(const Money& amount)
{
    money.Gold += amount.Gold;
    money.Sliver += amount.Sliver;
	money.Cooper += amount.Cooper;
}

void Player::spendmoney(Money& amount)
{
	money.Gold -= amount.Gold;
	money.Sliver -= amount.Sliver;
	money.Cooper -= amount.Cooper;
}

bool Player::HaveEnoughMoney(const Money &m) const
{
    if (m.Gold <= money.Gold && m.Sliver <= money.Sliver && m.Cooper <= money.Cooper)
    {
        return true;
    }
    return false;
}

bool Player::goToNextLevel() const
{
    return nextlevel;
}

float Player::getMissRate() const
{
    return MissRate;
}



void Player::Levelup() {
    while (exp >= lv * lv * ExpPerLevel) {
        nextlevel = true;
        ++lv;
        MaxHp += 20; currHp = MaxHp;
        MaxMp += 10;  currMp = MaxMp;
        Atk += 5; Def += 5;
    }
}


#include <unordered_map>
// Character.cpp
#include "Character.h"
#include <unordered_map>
#include <cstdlib>   // std::rand

// --- 1. 各種族的技能表（完全硬編碼） ---
static const std::unordered_map<std::string, std::vector<Skill>> kEnemySkillTable = {
    { "哥布林", {
        Skill("匕首突襲", "迅速刺擊敵人造成傷害", 1.2f, 0, 5, 2),
        Skill("混亂尖笑", "嘲諷敵人降低攻擊",     10.0f, 2, 3, 7),
        Skill("潛影閃避", "提升自身命中閃避能力",   5.0f, 2, 3, 5)
      }
    },
    { "史萊姆", {
        Skill("酸液噴射", "酸液腐蝕敵人",           5.0f, 3, 4, 6),
        Skill("黏性纏繞", "降低敵人防禦力",         8.0f, 2, 4, 8),
        Skill("分裂重組", "回復自身部分生命",      20.0f, 0, 5, 1)
      }
    },
    { "猛獸", {
        Skill("撕裂咬擊", "野性猛攻造成傷害",       1.5f, 0, 6, 2),
        Skill("咆哮威嚇", "威懾敵人降低其攻擊",      15.0f, 2, 4, 7),
        Skill("野性奔襲", "提升自身攻擊力",         10.0f, 2, 4, 2)
      }
    },
    { "龍", {
        Skill("龍焰吐息",  "噴出火焰造成大量傷害",   2.5f, 0,10, 2),
        Skill("龍鱗護體",  "增強自身防禦",           20.0f, 3, 6, 3),
        Skill("怒吼",      "威嚇敵人降低其防禦",      15.0f, 2, 5, 8)
      }
    },
    { "不死族", {
        Skill("腐爛毒液", "中毒造成持續傷害",        10.0f, 3, 5, 6),
        Skill("再生",     "回復生命每回合",           8.0f, 3, 4, 1),
        Skill("骷髏之盾", "增加防禦力",              15.0f, 2, 4, 3)
      }
    },
    { "鬥士", {
        Skill("重擊",     "集中力量給予重創",        1.8f, 0, 7, 2),
        Skill("鐵壁",     "精神集中提高防禦",        25.0f, 2, 4, 3),
        Skill("反擊姿態", "每回合對敵造成傷害",       8.0f, 2, 3, 6)
      }
    },
    { "鳥族", {
        Skill("俯衝猛擊", "從空中急速攻擊",          1.5f, 0, 5, 2),
        Skill("狂風掩護", "增加閃避能力",            5.0f, 2, 4, 5),
        Skill("高頻鳴叫", "使敵攻擊力下降",          10.0f, 2, 4, 7)
      }
    }
};

// --- 2. 各種族的掉落素材候選清單（完全硬編碼） ---
static const std::unordered_map<std::string, std::vector<Material>> kEnemyMaterialTable = {
    { "哥布林", {
        Material("葉子", "綠色葉片", 0, 1),
        Material("木枝", "普通木頭", 0, 2)
      }
    },
    { "史萊姆", {
        Material("黏液", "滑滑的黏液", 1, 1),
        Material("殘骸", "破碎的殘骸", 1, 1)
      }
    },
    { "猛獸", {
        Material("獸皮", "厚重獸皮", 2, 1),
        Material("獸骨", "堅硬獸骨", 2, 1)
      }
    },
    { "龍", {
        Material("龍鱗", "堅硬龍鱗", 4, 1),
        Material("龍爪", "強韌龍爪", 4, 1)
      }
    },
    { "不死族", {
        Material("骨頭", "骸骨碎片", 1, 1),
        Material("腐肉", "腐爛肉塊", 1, 1)
      }
    },
    { "鬥士", {
        Material("徽章",       "戰士徽章", 2, 1),
        Material("鋼劍碎片",   "鋼劍殘片", 2, 1)
      }
    },
    { "鳥族", {
        Material("鳥羽",   "輕盈羽毛", 1, 2),
        Material("鳥爪",   "銳利鳥爪", 1, 1)
      }
    }
};

// --- 3. Enemy 建構子：自動學習技能並隨機掉落素材 ---
Enemy::Enemy(const std::string& race,
    const std::string& name,
    int hp, int mp, int atk, int def,
    int expGain, float missRate)
    : Race(race), name(name),
    MaxHp(hp), HP(hp),
    MaxMp(mp), mp(mp),
    Atk(atk), Def(def),
    expGain(expGain), MissRate(missRate)
{
    // (1) 載入技能
    auto itSkill = kEnemySkillTable.find(Race);
    if (itSkill != kEnemySkillTable.end()) {
        for (const Skill& s : itSkill->second)
            learnskill(s);
    }

    // (2) 隨機決定掉落素材（50% 機率）
    auto itMat = kEnemyMaterialTable.find(Race);
    if (itMat != kEnemyMaterialTable.end()) {
        for (const Material& m : itMat->second) {
            if (std::rand() % 100 < 50) {
                FallBackpack.push_back(m);
            }
        }
    }
}



Enemy& Enemy::operator=(const Enemy& E)
{
    if (this == &E) {
        return *this;
    }
    Race = E.Race;
    name = E.name;
    MaxHp = E.MaxHp; HP = E.HP;
    MaxMp = E.MaxMp; mp = E.mp;
    Atk = E.Atk; Def = E.Def;
    expGain = E.expGain; 
    Skills = E.Skills;   
    Effects = E.Effects; 
    MissRate = E.MissRate;
	Skills = E.Skills; // 複製技能列表
    return *this;
}

Enemy::~Enemy() {}

std::string Enemy::getname() const { return name; }

SkillResult Enemy::Attack(Player& player) {
	Skillidx = -1; // 重置技能索引
    SkillResult result;
    // 1. 每回合自動回魔
    mp = std::min(mp + 5, MaxMp);

    // 2. 沒有技能 → 普攻
    if (Skills.empty()) {
        result.immediateDamage = Atk;
        return result;
    }

    // 3. 篩選可用技能
    std::vector<const Skill*> usable;
    usable.reserve(Skills.size());
    for (const auto& sk : Skills) {
        if (mp >= sk.costPower) usable.push_back(&sk);
    }
    if (usable.empty()) {
        result.immediateDamage = Atk;
        return result;
    }

    // 4. AI 選擇策略
    float hpRatio = static_cast<float>(player.gethp()) / std::max(1, player.getMaxHp());
    const Skill* chosen = nullptr;

    if (hpRatio > 0.5f) {
        if (rand() % 100 < 50) {
            result.immediateDamage = Atk;
            return result;
        }
        chosen = usable[rand() % usable.size()];
    }
    else if (hpRatio <= 0.2f) {
        chosen = *std::max_element(usable.begin(), usable.end(),
            [](const Skill* a, const Skill* b) { return a->Power < b->Power; });
    }
    else {
        chosen = usable[rand() % usable.size()];
    }
	// 5. 應用選擇的技能
    for(auto & it : Skills) {
        if (it.Name == chosen->Name) {
            Skillidx = &it - &Skills[0]; // 計算索引
            break;
        }
	}
    // 5. 扣魔
    mp = std::max(0, mp - chosen->costPower);

    // 6. 持續效果 (含 Debuff 6~9)
    if (chosen->affectRound > 0) {
        result.effect =  std::make_unique<Effect>(chosen->Desc + " 效果",
            0, 0, 0, 0,0,
            chosen->affectRound);
        if (chosen->Type >= 6 ) {
            result.target = Target::ENEMY; // 敵人之敵人 = 玩家
            switch (chosen->Type) {
            case 6: result.effect->affectHp = -int(chosen->Power); break;
            case 7: result.effect->affectAtk = -int(chosen->Power); break;
            case 8: result.effect->affectDef = -int(chosen->Power); break;
            case 9: result.effect->affectMp = -int(chosen->Power); break;
			case 10: result.effect->affectMissRate = -int(chosen->Power); break;
            }
        }
        else {
            // 自我增益型持續效果
            result.target = Target::SELF;
            switch (chosen->Type) {
            case 1: result.effect->affectHp = int(chosen->Power); break;
            case 2: result.effect->affectAtk = int(chosen->Power); break;
            case 3: result.effect->affectDef = int(chosen->Power); break;
            case 4: result.effect->affectMp = int(chosen->Power); break;
			case 5: result.effect->affectMissRate = int(chosen->Power); break;
            default: break;
            }
        }
        return result;
    }

    // 7. 立即效果技能
    switch (chosen->Type) {
    case 2: // 立即傷害 (乘攻擊力)
        result.immediateDamage = int(static_cast<float>(Atk) * chosen->Power);
        result.target = Target::ENEMY;
        break;

    case 1: // 立即自我治療
        HP = std::min(HP + int(chosen->Power), MaxHp);
        break;

    case 3: // 立即自我防禦
        Def = std::max(0, Def + int(chosen->Power));
        break;

    case 4: // 立即自我回魔
        mp = std::min(mp + int(chosen->Power), MaxMp);
        break;
    case 5: // 立即自我提升命中
        MissRate = std::min(std::max(MissRate + chosen->Power, 0.0f), 1.0f);
		break;
    default:
        // 無特殊作用則普攻
        result.immediateDamage = Atk;
        result.target = Target::ENEMY;
        break;
    }

    return result;
}



void Enemy::learnskill(const Skill& skill) {
    Skills.push_back(skill);
}

int Enemy::BeAttacked(int atk) {
    int damage = std::max(atk - Def / 2, 0); // 計算實際傷害
    HP -= damage;
    return damage; // 回傳傷害值
}

// Enemy::BeEffect (同樣修改)
void Enemy::BeEffect(const Effect& e) {
    // 效果施加時，立即改變一次性屬性 (攻/防/閃避)
    Atk = std::max(0, Atk + e.affectAtk);
    Def = std::max(0, Def + e.affectDef);
    MissRate = std::min(std::max(MissRate + e.affectMissRate, 0.0f), 1.0f);
    Effects.push_back(e);
}

// Enemy::Affected (同樣修改)
void Enemy::Affected() {
    for (auto it = Effects.begin(); it != Effects.end();) {
        // 1. 每回合持續生效的效果 (HP/MP)
        HP = std::min(HP + it->affectHp, MaxHp);
        mp = std::min(mp + it->affectMp, MaxMp);

        // 2. 時效遞減
        it->nextRound();

        // 3. 效果結束 → 還原屬性並刪除
        if (!it->existEffect()) {
            Atk = std::max(0, Atk - it->affectAtk);
            Def = std::max(0, Def - it->affectDef);
            MissRate = std::min(std::max(MissRate - it->affectMissRate, 0.0f), 1.0f);
            it = Effects.erase(it);
        }
        else {
            ++it;
        }
    }
}

bool Enemy::Died() const { return HP <= 0; }
int Enemy::Giveexp() const { return expGain; }
int Enemy::gethp() const { return HP; }
int Enemy::getmp() const { return mp; }
int Enemy::getatk() const { return Atk; }
int Enemy::getdef() const { return Def; }
float Enemy::getMissRate()const { return MissRate; }
std::string Enemy::getSkillName() const { return Skillidx == -1 ? "攻擊" : Skills[Skillidx].Name; }

std::string Enemy::getRace() const
{
    return Race;
}


std::vector<Material> Enemy::getFallBackpack() const
{
	return FallBackpack;
}
