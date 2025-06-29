#include "Merchant.h"
#include <cctype>

void Merchant::Interact(Player& p)
{
	system("cls");
	if (type == MerchantType::Material) {
		std::cout << "歡迎來到材料商店！" << std::endl;
		std::cout << "這裡有各種材料供你購買。" << std::endl;
		showGoods();
		std::cin.clear();
		std::cin.sync();
		std::cout << "請選擇你想購買或出售的材料，輸入對應的編號：(輸入(Q/q)離開商店)" << std::endl;
		std::cout << "目前擁有金錢" <<"金幣: "<< p.showmoney().Gold <<" 銀幣: " << p.showmoney().Sliver << " 銅幣: "<< p.showmoney().Cooper << std::endl;
		while (true) {
			std::string input;
			std::cin >> input;
			if (input == "Q" || input == "q") {
				std::cout << "感謝光臨，歡迎下次再來！" << std::endl;
				break;
			}
			bool legal = true;
			for (char c : input) {
				if (!isdigit(c)) {
					std::cout << "客人,請輸入正確的編號：" << std::endl;
					legal = false;
					break;
				}
			}
			if (!legal) continue;
			int choose = stoi(input);
			if (choose < 0 || choose >= MaterialList.size()) {
				std::cout << "客人,本店沒有這項商品：" << std::endl;
				continue;
			}
			std::cout << "請問是要出售或購買" << std::endl;
			std::string action;
			std::cin >> action;
			if (action != "購買" && action != "出售") {
				std::cout << "客人,請輸入正確的操作(購買/出售)：" << std::endl;
				continue;
			}
			if (action == "出售") {
				std::vector<Material> materials;
				materials.push_back(MaterialList[choose].material);
				if (p.HaveEnoughMaterial(materials)) {
					p.throwMaterial(MaterialList[choose].material);
					p.earnmoney(MaterialList[choose].price);
					std::cout << "客人,您已成功出售 " << MaterialList[choose].material.getName() << " x" << MaterialList[choose].material.amount << "。" << std::endl;
				}
				else {
					std::cout << "客人,您沒有足夠的材料來出售。" << std::endl;
				}
				continue;
			}
			else {
				if (p.HaveEnoughMoney(MaterialList[choose].price)) {
					p.spendmoney(MaterialList[choose].price);
					p.getMaterial(MaterialList[choose].material);
					std::cout << "客人,您已成功購買 " << MaterialList[choose].material.getName() << " x" << MaterialList[choose].material.amount << "。" << std::endl;
					MaterialList.erase(MaterialList.begin() + choose);
				}
				else {
					std::cout << "客人,您的錢不足以買下這份商品。" << std::endl;
				}
			}
			
		}
	}
	else if (type == MerchantType::Equipment) {
		std::cout << "歡迎來到裝備商店！" << std::endl;
		std::cout << "這裡有各種裝備供你購買。" << std::endl;
		showGoods();
		std::cout << "請選擇你想購買的裝備，輸入對應的編號：(輸入(Q/q)離開商店)" << std::endl;
		std::cout << "目前擁有金錢" << "金幣: " << p.showmoney().Gold << " 銀幣: " << p.showmoney().Sliver << " 銅幣: " << p.showmoney().Cooper << std::endl;
		while (true)
		{
			std::string input;
			std::cin >> input;
			if (input == "Q" || input == "q") {
				std::cout << "感謝光臨，歡迎下次再來！" << std::endl;
				break;
			}
			int choose = stoi(input);
			if (choose < 0 || choose >= EquipmentList.size()) {
				std::cout << "客人,本店沒有這項商品：" << std::endl;
				continue;
			}
			if (p.HaveEnoughMoney(EquipmentList[choose].price) && p.HaveEnoughMaterial(EquipmentList[choose].NeedMaterial)) {
				if (p.HaveEnoughMoney(EquipmentList[choose].price)) {
					p.spendmoney(EquipmentList[choose].price);
					p.getEquip(EquipmentList[choose].equip);
					for (auto& material : EquipmentList[choose].NeedMaterial) {
						p.throwMaterial(material);
					}
					std::cout << "客人,您已成功購買 " << EquipmentList[choose].equip.getName() << "。" << std::endl;
					p.getEquip(EquipmentList[choose].equip);
					EquipmentList.erase(EquipmentList.begin() + choose);
				}
				else {
					std::cout << "客人,您的材料不足以買下這份商品。" << std::endl;
				}
			}
			else {
				std::cout << "客人,您的錢不足以買下這份商品。" << std::endl;
			}
		}
	}
	else {
		std::cout << "歡迎來到道具商店！" << std::endl;
		std::cout << "這裡有各種道具供你購買。" << std::endl;
		showGoods();
		std::cout << "請選擇你想購買的道具，輸入對應的編號：(輸入(Q/q)離開商店)" << std::endl;
		std::cout << "目前擁有金錢" << "金幣: " << p.showmoney().Gold << " 銀幣: " << p.showmoney().Sliver << " 銅幣: " << p.showmoney().Cooper << std::endl;
		while (true) {
			std::string input;
			std::cin >> input;
			if (input == "Q" || input == "q") {
				std::cout << "感謝光臨，歡迎下次再來！" << std::endl;
				break;
			}
			int choose = stoi(input);
			if (choose < 0 || choose >= MiseryList.size()) {
				std::cout << "客人,本店沒有這項商品：" << std::endl;
				continue;
			}
			if (p.HaveEnoughMoney(MiseryList[choose].price)) {
				p.spendmoney(MiseryList[choose].price);
				p.getMyseryItem(MiseryList[choose].misery);
				std::cout << "客人,您已成功購買 " << MiseryList[choose].misery.getName() << "。" << std::endl;
				MiseryList.erase(MiseryList.begin() + choose);
			}
			else {
				std::cout << "客人,您的錢不足以買下這份商品。" << std::endl;
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
			std::cout << "名稱 : " << item.material.getName() << " --- " << item.material.getDesc() << " 價格 : ";
			showRare(item.material.arity);
			if (item.price.Gold != 0) {
				std::cout << item.price.Gold << " 金幣 ";
			}
			if (item.price.Sliver != 0) {
				std::cout << item.price.Sliver << " 銀幣 ";
			}
			if (item.price.Cooper != 0) {
				std::cout << item.price.Cooper << " 銅幣 ";
			}
			std::cout << std::endl;
		}
	}
	else if (type == MerchantType::Equipment) {
		for (auto& item : EquipmentList) {
			std::cout << "名稱 : " << item.equip.getName() << " --- " << item.equip.getDesc() << " 稀有度 : ";
			showRare(item.equip.arity);
			std::cout << " 價格 : ";
			if (item.price.Gold != 0) {
				std::cout << item.price.Gold << " 金幣 ";
			}
			if (item.price.Sliver != 0) {
				std::cout << item.price.Sliver << " 銀幣 ";
			}
			if (item.price.Cooper != 0) {
				std::cout << item.price.Cooper << " 銅幣 ";
			}
			for (auto& materialneed : item.NeedMaterial) {
				std::cout << "需要材料 : " << materialneed.getName() << " x" << materialneed.amount << " ";
			}
			std::cout << std::endl;
		}
	}
	else if (type == MerchantType::Misery) {
		for (auto& Item : MiseryList) {
			std::cout << "名稱 : " << Item.misery.getName() << " --- " << Item.misery.getDesc() << " 價格 : ";
			showRare(Item.misery.arity);
			std::cout<<std::endl;
		}
	}
}


