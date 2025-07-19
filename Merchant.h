#pragma once
#include "Item.h"
#include "Character.h"

enum class MerchantType {
	Material,///材料商人
	Equipment,//武器商人
	Misery,//奇異藥水商人
};

struct SellMaterial
{
	Material material;
	Money price;//價格
	SellMaterial(Material m, Money p) : material(m), price(p) {};//構造函數
};

struct SellEquip
{
	Equip equip;
	Money price;//價格
	std::vector<Material> NeedMaterial;//製作裝備所需材料(有些需要自備材料)
	SellEquip(Equip e, Money p, std::vector<Material> m) : equip(e), price(p), NeedMaterial(m) {};//構造函數
};

struct SellMisery
{
	MiseryItem misery;
	Money price;//價格
	SellMisery(MiseryItem m, Money p) : misery(m), price(p) {};//構造函數
};

class MerchantUI {
public:
    virtual ~MerchantUI() = default;
    virtual void showTitle(const std::string& title) = 0;
    virtual void showGoods(const std::vector<std::string>& names, const std::vector<std::string>& descs, const std::vector<std::string>& prices) = 0;
    virtual void showMoney(const Money& money) = 0;
    virtual int selectGoods(int maxIdx) = 0;
    virtual std::string selectAction(const std::vector<std::string>& actions) = 0;
    virtual void showTip(const std::string& msg) = 0;
    virtual void wait() = 0;
};

class Merchant
{
public:
    void Interact(Player &p, MerchantUI& ui);//與商人互動（重構，支援多種UI）
    void Interact(Player &p);//舊版console專用，將來可移除
    void showGoods();//顯示商人販賣的物品
    Merchant(MerchantType mtype, int level);
    void getMaterialList(const SellMaterial& m);//隨機生成商人販賣的物品
    void getEquipmentList(const SellEquip& e);//隨機生成商人販賣的裝備
    void getMiseryList(const SellMisery& m);//隨機生成商人販賣的奇異藥水
    // 新增getter供SFML UI使用
    MerchantType getType() const { return type; }
    std::vector<SellMaterial>& getMaterialListRef() { return MaterialList; }
    std::vector<SellEquip>& getEquipmentListRef() { return EquipmentList; }
    std::vector<SellMisery>& getMiseryListRef() { return MiseryList; }
private:
    MerchantType type;
    int level;//商人等級(越高其販賣物品越稀有)
    std::vector<SellMaterial> MaterialList;//販賣商品表
    std::vector<SellEquip> EquipmentList;//販賣裝備表
    std::vector<SellMisery> MiseryList;//販賣奇異藥水表
};

