#include "Addition.h"
// Skill
Skill::Skill(const std::string& name, const std::string& desc,
    float power, int round, int costpower, int type)
    : Name(name), Desc(desc), Power(power), costPower(costpower),
    affectRound(round), Type(type) {
}

Skill& Skill::operator=(const Skill& s)
{
    if (this == &s) {
        return *this;
    }
    Name = s.Name;
    Desc = s.Desc;
    Power = s.Power;
    affectRound = s.affectRound;
    costPower = s.costPower;
    Type = s.Type;
    return *this;
}

// Effect
void Effect::nextRound() { --count; }
bool Effect::existEffect() const { return count >= 0; }
Effect::Effect(const std::string& desc,
    int ahp, int aatk, int adef, int amp,int affectmissrate, int c)
    : Desc(desc), affectHp(ahp), affectAtk(aatk),
    affectDef(adef), affectMp(amp),affectMissRate(affectmissrate), count(c) {
}

Effect::Effect(const Effect& e) :Desc(e.Desc), affectHp(e.affectHp), affectAtk(e.affectAtk),
affectDef(e.affectDef), affectMp(e.affectMp), count(e.count),affectMissRate(e.affectMissRate){
}