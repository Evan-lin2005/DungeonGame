#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <map>
struct Skill {
    std::string Name, Desc;
    float Power;
    int costPower;
    int affectRound;
    int Type;
    Skill(const std::string& name = "None", const std::string& desc = "None",
        float power = 0.0, int round = 0, int costpower = 0, int type = 0);
    Skill& operator=(const Skill& s);
};

struct Effect {
    std::string Desc;
    int affectHp;
    int affectAtk;
    int affectDef;
    int affectMp;
    int affectMissRate;
    int count;
    void nextRound();
    bool existEffect() const;
    Effect(const std::string& desc = "None",
        int ahp = 0, int aatk = 0, int adef = 0, int amp = 0,int affectmissrate =0, int c = 0);
    Effect(const Effect& e);
};
