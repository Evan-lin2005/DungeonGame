#pragma once
#include <string>
#include "Addition.h"

constexpr int MaxEquipCapacity = 20;

// ���~�}����
enum class Arity {
    common,
    good,
    rare,
    special,
    master,
    legendary,
    mythical,
    Only
};

inline Arity ArityFromInt(int r) {
    switch (r) {
    case 0: return Arity::common;
    case 1: return Arity::good;
    case 2: return Arity::rare;
    case 3: return Arity::special;
    case 4: return Arity::master;
    case 5: return Arity::legendary;
    case 6: return Arity::mythical;
    case 7: return Arity::Only;
    default: return Arity::common;
    }
}

//�i�ܵ}����
inline void showRare(Arity a) {
    switch (a) {
    case Arity::common:
        std::cout << "���q ";
        break;
    case Arity::good:
        std::cout << "�}�n ";
        break;
    case Arity::rare:
        std::cout << "�}�� ";
        break;
    case Arity::special:
        std::cout << "�S�s ";
        break;
    case Arity::master:
        std::cout << "�j�v ";
        break;
    case Arity::legendary:
        std::cout << "�v�� ";
        break;
    case Arity::mythical:
        std::cout << "���� ";
        break;
    case Arity::Only:
        std::cout << "����";
        break;
    }
}

struct Item {
public:
    std::string name;
    std::string desc;

    Arity arity;

    // �غc�l�G�ϥξ�� rank
    Item(const std::string& name = "None",
        const std::string& desc = "None",
        int rank = 0)
        : name(name), desc(desc), arity(ArityFromInt(rank)) {
    }

    // �غc�l�G�ϥ� Arity
    Item(const std::string& name,
        const std::string& desc,
        Arity rank)
        : name(name), desc(desc), arity(rank) {
    }

    virtual ~Item() = default;
    const std::string& getName() const { return name; }
    const std::string& getDesc() const { return desc; }
};

struct Equip : public Item {
public:
    int affectRound;
    int affectHp, affectAtk, affectDef, affectMp;
    bool used = false;

    // �غc�l�G��� rank
    Equip(const std::string& name,
        const std::string& desc,
        int rank,
        int round, int ahp, int aatk, int adef, int amp)
        : Item(name, desc, rank),
        affectRound(round), affectHp(ahp), affectAtk(aatk),
        affectDef(adef), affectMp(amp) {
    }

    // �غc�l�GArity
    Equip(const std::string& name,
        const std::string& desc,
        Arity rank,
        int round, int ahp, int aatk, int adef, int amp)
        : Item(name, desc, rank),
        affectRound(round), affectHp(ahp), affectAtk(aatk),
        affectDef(adef), affectMp(amp) {
    }
};

struct Money : public Item {
public:
    int Gold, Sliver, Cooper;

    // �غc�l�G��� rank
    Money(const std::string& name = "�q�γf��",
        const std::string& desc = "100�ɹ�=1��,100��=1��",
        int rank = 0,
        int gold = 0, int sliver = 0, int cooper = 0)
        : Item(name, desc, rank), Gold(gold), Sliver(sliver), Cooper(cooper) {
    }

    // �غc�l�GArity
    Money(const std::string& name,
        const std::string& desc,
        Arity rank,
        int gold, int sliver, int cooper)
        : Item(name, desc, rank), Gold(gold), Sliver(sliver), Cooper(cooper) {
    }

    // �غc�l�G�u�Ǫ��B
    Money(int gold, int sliver, int cooper)
        : Item("�q�γf��", "", Arity::common), Gold(gold), Sliver(sliver), Cooper(cooper) {
    }
};

struct Material : public Item {
public:
    int amount;

	Material() :Item(), amount(1) {}

    // �غc�l�G��� rank
    Material(const std::string& name ,
        const std::string& desc ,
        int rank = 0,
        int amount = 1)
        : Item(name, desc, rank), amount(amount) {
    }

    // �غc�l�GArity
    Material(const std::string& name,
        const std::string& desc,
        Arity rank,
        int amount)
        : Item(name, desc, rank), amount(amount) {
    }

    bool exist() const { return amount > 0; }
};

struct MiseryItem : public Item {
public:
    Effect itemEffect;
    int amount;

	MiseryItem() : Item(), itemEffect(), amount(1) {}

    // �غc�l�G��� rank
    MiseryItem(const std::string& name,
        const std::string& desc,
        int rank,
        const Effect& effect,
        int amount)
        : Item(name, desc, rank), itemEffect(effect), amount(amount) {
    }

    // �غc�l�GArity
    MiseryItem(const std::string& name,
        const std::string& desc,
        Arity rank,
        const Effect& effect,
        int amount)
        : Item(name, desc, rank), itemEffect(effect), amount(amount) {
    }
};
