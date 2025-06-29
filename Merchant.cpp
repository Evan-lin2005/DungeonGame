#include "Merchant.h"
#include <cctype>

void Merchant::Interact(Player& p)
{
	system("cls");
	if (type == MerchantType::Material) {
		std::cout << "�w��Ө���ưө��I" << std::endl;
		std::cout << "�o�̦��U�ا��ƨѧA�ʶR�C" << std::endl;
		showGoods();
		std::cin.clear();
		std::cin.sync();
		std::cout << "�п�ܧA�Q�ʶR�ΥX�⪺���ơA��J�������s���G(��J(Q/q)���}�ө�)" << std::endl;
		std::cout << "�ثe�֦�����" <<"����: "<< p.showmoney().Gold <<" �ȹ�: " << p.showmoney().Sliver << " �ɹ�: "<< p.showmoney().Cooper << std::endl;
		while (true) {
			std::string input;
			std::cin >> input;
			if (input == "Q" || input == "q") {
				std::cout << "�P�¥��{�A�w��U���A�ӡI" << std::endl;
				break;
			}
			bool legal = true;
			for (char c : input) {
				if (!isdigit(c)) {
					std::cout << "�ȤH,�п�J���T���s���G" << std::endl;
					legal = false;
					break;
				}
			}
			if (!legal) continue;
			int choose = stoi(input);
			if (choose < 0 || choose >= MaterialList.size()) {
				std::cout << "�ȤH,�����S���o���ӫ~�G" << std::endl;
				continue;
			}
			std::cout << "�аݬO�n�X����ʶR" << std::endl;
			std::string action;
			std::cin >> action;
			if (action != "�ʶR" && action != "�X��") {
				std::cout << "�ȤH,�п�J���T���ާ@(�ʶR/�X��)�G" << std::endl;
				continue;
			}
			if (action == "�X��") {
				std::vector<Material> materials;
				materials.push_back(MaterialList[choose].material);
				if (p.HaveEnoughMaterial(materials)) {
					p.throwMaterial(MaterialList[choose].material);
					p.earnmoney(MaterialList[choose].price);
					std::cout << "�ȤH,�z�w���\�X�� " << MaterialList[choose].material.getName() << " x" << MaterialList[choose].material.amount << "�C" << std::endl;
				}
				else {
					std::cout << "�ȤH,�z�S�����������ƨӥX��C" << std::endl;
				}
				continue;
			}
			else {
				if (p.HaveEnoughMoney(MaterialList[choose].price)) {
					p.spendmoney(MaterialList[choose].price);
					p.getMaterial(MaterialList[choose].material);
					std::cout << "�ȤH,�z�w���\�ʶR " << MaterialList[choose].material.getName() << " x" << MaterialList[choose].material.amount << "�C" << std::endl;
					MaterialList.erase(MaterialList.begin() + choose);
				}
				else {
					std::cout << "�ȤH,�z���������H�R�U�o���ӫ~�C" << std::endl;
				}
			}
			
		}
	}
	else if (type == MerchantType::Equipment) {
		std::cout << "�w��Ө�˳ưө��I" << std::endl;
		std::cout << "�o�̦��U�ظ˳ƨѧA�ʶR�C" << std::endl;
		showGoods();
		std::cout << "�п�ܧA�Q�ʶR���˳ơA��J�������s���G(��J(Q/q)���}�ө�)" << std::endl;
		std::cout << "�ثe�֦�����" << "����: " << p.showmoney().Gold << " �ȹ�: " << p.showmoney().Sliver << " �ɹ�: " << p.showmoney().Cooper << std::endl;
		while (true)
		{
			std::string input;
			std::cin >> input;
			if (input == "Q" || input == "q") {
				std::cout << "�P�¥��{�A�w��U���A�ӡI" << std::endl;
				break;
			}
			int choose = stoi(input);
			if (choose < 0 || choose >= EquipmentList.size()) {
				std::cout << "�ȤH,�����S���o���ӫ~�G" << std::endl;
				continue;
			}
			if (p.HaveEnoughMoney(EquipmentList[choose].price) && p.HaveEnoughMaterial(EquipmentList[choose].NeedMaterial)) {
				if (p.HaveEnoughMoney(EquipmentList[choose].price)) {
					p.spendmoney(EquipmentList[choose].price);
					p.getEquip(EquipmentList[choose].equip);
					for (auto& material : EquipmentList[choose].NeedMaterial) {
						p.throwMaterial(material);
					}
					std::cout << "�ȤH,�z�w���\�ʶR " << EquipmentList[choose].equip.getName() << "�C" << std::endl;
					p.getEquip(EquipmentList[choose].equip);
					EquipmentList.erase(EquipmentList.begin() + choose);
				}
				else {
					std::cout << "�ȤH,�z�����Ƥ����H�R�U�o���ӫ~�C" << std::endl;
				}
			}
			else {
				std::cout << "�ȤH,�z���������H�R�U�o���ӫ~�C" << std::endl;
			}
		}
	}
	else {
		std::cout << "�w��Ө�D��ө��I" << std::endl;
		std::cout << "�o�̦��U�عD��ѧA�ʶR�C" << std::endl;
		showGoods();
		std::cout << "�п�ܧA�Q�ʶR���D��A��J�������s���G(��J(Q/q)���}�ө�)" << std::endl;
		std::cout << "�ثe�֦�����" << "����: " << p.showmoney().Gold << " �ȹ�: " << p.showmoney().Sliver << " �ɹ�: " << p.showmoney().Cooper << std::endl;
		while (true) {
			std::string input;
			std::cin >> input;
			if (input == "Q" || input == "q") {
				std::cout << "�P�¥��{�A�w��U���A�ӡI" << std::endl;
				break;
			}
			int choose = stoi(input);
			if (choose < 0 || choose >= MiseryList.size()) {
				std::cout << "�ȤH,�����S���o���ӫ~�G" << std::endl;
				continue;
			}
			if (p.HaveEnoughMoney(MiseryList[choose].price)) {
				p.spendmoney(MiseryList[choose].price);
				p.getMyseryItem(MiseryList[choose].misery);
				std::cout << "�ȤH,�z�w���\�ʶR " << MiseryList[choose].misery.getName() << "�C" << std::endl;
				MiseryList.erase(MiseryList.begin() + choose);
			}
			else {
				std::cout << "�ȤH,�z���������H�R�U�o���ӫ~�C" << std::endl;
			}
		}
	}
}

Merchant::Merchant(MerchantType mtype, int level):
	type(mtype), level(level)
{
	
}

void Merchant::getMaterialList(const SellMaterial& m)
{
	MaterialList.push_back(m);
}

void Merchant::getEquipmentList(const SellEquip& e)
{
	EquipmentList.push_back(e);
}

void Merchant::getMiseryList(const SellMisery& m)
{
	MiseryList.push_back(m);
}

void Merchant::showGoods()
{
	if (type == MerchantType::Material) {
		for (auto& item : MaterialList) {
			std::cout << "�W�� : " << item.material.getName() << " --- " << item.material.getDesc() << " ���� : ";
			showRare(item.material.arity);
			if (item.price.Gold != 0) {
				std::cout << item.price.Gold << " ���� ";
			}
			if (item.price.Sliver != 0) {
				std::cout << item.price.Sliver << " �ȹ� ";
			}
			if (item.price.Cooper != 0) {
				std::cout << item.price.Cooper << " �ɹ� ";
			}
			std::cout << std::endl;
		}
	}
	else if (type == MerchantType::Equipment) {
		for (auto& item : EquipmentList) {
			std::cout << "�W�� : " << item.equip.getName() << " --- " << item.equip.getDesc() << " �}���� : ";
			showRare(item.equip.arity);
			std::cout << " ���� : ";
			if (item.price.Gold != 0) {
				std::cout << item.price.Gold << " ���� ";
			}
			if (item.price.Sliver != 0) {
				std::cout << item.price.Sliver << " �ȹ� ";
			}
			if (item.price.Cooper != 0) {
				std::cout << item.price.Cooper << " �ɹ� ";
			}
			for (auto& materialneed : item.NeedMaterial) {
				std::cout << "�ݭn���� : " << materialneed.getName() << " x" << materialneed.amount << " ";
			}
			std::cout << std::endl;
		}
	}
	else if (type == MerchantType::Misery) {
		for (auto& Item : MiseryList) {
			std::cout << "�W�� : " << Item.misery.getName() << " --- " << Item.misery.getDesc() << " ���� : ";
			showRare(Item.misery.arity);
			std::cout<<std::endl;
		}
	}
}


