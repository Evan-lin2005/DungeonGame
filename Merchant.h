#pragma once
#include "Item.h"
#include "Character.h"

enum class MerchantType {
	Material,///���ưӤH
	Equipment,//�Z���ӤH
	Misery,//�_���Ĥ��ӤH
};

struct SellMaterial
{
	Material material;
	Money price;//����
	SellMaterial(Material m, Money p) : material(m), price(p) {};//�c�y���
};

struct SellEquip
{
	Equip equip;
	Money price;//����
	std::vector<Material> NeedMaterial;//�s�@�˳Ʃһݧ���(���ǻݭn�۳Ƨ���)
	SellEquip(Equip e, Money p, std::vector<Material> m) : equip(e), price(p), NeedMaterial(m) {};//�c�y���
};

struct SellMisery
{
	MiseryItem misery;
	Money price;//����
	SellMisery(MiseryItem m, Money p) : misery(m), price(p) {};//�c�y���
};


class Merchant
{
public:
	void Interact(Player &p);//�P�ӤH����
	void showGoods();//��ܰӤH�c�檺���~
	Merchant(MerchantType mtype, int level);
	void getMaterialList(const SellMaterial& m);//�H���ͦ��ӤH�c�檺���~
	void getEquipmentList(const SellEquip& e);//�H���ͦ��ӤH�c�檺�˳�
	void getMiseryList(const SellMisery& m);//�H���ͦ��ӤH�c�檺�_���Ĥ�
private:
	MerchantType type;
	int level;//�ӤH����(�V����c�檫�~�V�}��)
	std::vector<SellMaterial> MaterialList;//�c��ӫ~��
	std::vector<SellEquip> EquipmentList;//�c��˳ƪ�
	std::vector<SellMisery> MiseryList;//�c��_���Ĥ���
};

