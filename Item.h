#pragma once
#include <string>
#include "Addition.h"

constexpr int MaxEquipCapacity = 20;

// 物品稀有度
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

//展示稀有度
inline void showRare(Arity a) {
    switch (a) {
    case Arity::common:
        std::cout << "普通 ";
        break;
    case Arity::good:
        std::cout << "良好 ";
        break;
    case Arity::rare:
        std::cout << "稀有 ";
        break;
    case Arity::special:
        std::cout << "特製 ";
        break;
    case Arity::master:
        std::cout << "大師 ";
        break;
    case Arity::legendary:
        std::cout << "史詩 ";
        break;
    case Arity::mythical:
        std::cout << "神話 ";
        break;
    case Arity::Only:
        std::cout << "不朽";
        break;
    }
}

struct Item {
public:
    std::string name;
    std::string desc;

    Arity arity;

    // 建構子：使用整數 rank
    Item(const std::string& name = "None",
        const std::string& desc = "None",
        int rank = 0)
        : name(name), desc(desc), arity(ArityFromInt(rank)) {
    }

    // 建構子：使用 Arity
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

    // 建構子：整數 rank
    Equip(const std::string& name,
        const std::string& desc,
        int rank,
        int round, int ahp, int aatk, int adef, int amp)
        : Item(name, desc, rank),
        affectRound(round), affectHp(ahp), affectAtk(aatk),
        affectDef(adef), affectMp(amp) {
    }

    // 建構子：Arity
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

    // 建構子：整數 rank
    Money(const std::string& name = "通用貨幣",
        const std::string& desc = "100銅幣=1銀,100銀=1金",
        int rank = 0,
        int gold = 0, int sliver = 0, int cooper = 0)
        : Item(name, desc, rank), Gold(gold), Sliver(sliver), Cooper(cooper) {
    }

    // 建構子：Arity
    Money(const std::string& name,
        const std::string& desc,
        Arity rank,
        int gold, int sliver, int cooper)
        : Item(name, desc, rank), Gold(gold), Sliver(sliver), Cooper(cooper) {
    }

    // 建構子：只傳金額
    Money(int gold, int sliver, int cooper)
        : Item("通用貨幣", "", Arity::common), Gold(gold), Sliver(sliver), Cooper(cooper) {
    }
};

struct Material : public Item {
public:
    int amount;

	Material() :Item(), amount(1) {}

    // 建構子：整數 rank
    Material(const std::string& name ,
        const std::string& desc ,
        int rank = 0,
        int amount = 1)
        : Item(name, desc, rank), amount(amount) {
    }

    // 建構子：Arity
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

    // 建構子：整數 rank
    MiseryItem(const std::string& name,
        const std::string& desc,
        int rank,
        const Effect& effect,
        int amount)
        : Item(name, desc, rank), itemEffect(effect), amount(amount) {
    }

    // 建構子：Arity
    MiseryItem(const std::string& name,
        const std::string& desc,
        Arity rank,
        const Effect& effect,
        int amount)
        : Item(name, desc, rank), itemEffect(effect), amount(amount) {
    }
};
