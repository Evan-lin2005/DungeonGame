#include "Merchant.h"
#include <cctype>

void Merchant::Interact(Player& p) {
    // 舊版console專用，已棄用
}

void Merchant::Interact(Player& p, MerchantUI& ui) {
	if (type == MerchantType::Material) {
        ui.showTitle("歡迎來到材料商店！");
        std::vector<std::string> names, descs, prices;
        for (auto& item : MaterialList) {
            names.push_back(item.material.getName());
            descs.push_back(item.material.getDesc());
            prices.push_back(
                (item.price.Gold ? std::to_string(item.price.Gold) + "金 " : "") +
                (item.price.Sliver ? std::to_string(item.price.Sliver) + "銀 " : "") +
                (item.price.Cooper ? std::to_string(item.price.Cooper) + "銅" : "")
            );
        }
        ui.showGoods(names, descs, prices);
        ui.showMoney(p.showmoney());
		while (true) {
            int choose = ui.selectGoods((int)MaterialList.size());
            if (choose < 0 || choose >= (int)MaterialList.size()) break;
            std::string action = ui.selectAction({u8"購買", u8"出售"});
            if (action == u8"出售") {
				std::vector<Material> materials;
				materials.push_back(MaterialList[choose].material);
				if (p.HaveEnoughMaterial(materials)) {
					p.throwMaterial(MaterialList[choose].material);
					p.earnmoney(MaterialList[choose].price);
                    ui.showTip("您已成功出售 " + MaterialList[choose].material.getName() + " x" + std::to_string(MaterialList[choose].material.amount));
                } else {
                    ui.showTip("您沒有足夠的材料來出售。");
				}
                ui.wait();
            } else if (action == u8"購買") {
				if (p.HaveEnoughMoney(MaterialList[choose].price)) {
					p.spendmoney(MaterialList[choose].price);
					p.getMaterial(MaterialList[choose].material);
                    ui.showTip("您已成功購買 " + MaterialList[choose].material.getName() + " x" + std::to_string(MaterialList[choose].material.amount));
					MaterialList.erase(MaterialList.begin() + choose);
                } else {
                    ui.showTip("您的錢不足以買下這份商品。");
                }
                ui.wait();
            } else {
                break;
            }
            // 更新商品與金錢顯示
            names.clear(); descs.clear(); prices.clear();
            for (auto& item : MaterialList) {
                names.push_back(item.material.getName());
                descs.push_back(item.material.getDesc());
                prices.push_back(
                    (item.price.Gold ? std::to_string(item.price.Gold) + "金 " : "") +
                    (item.price.Sliver ? std::to_string(item.price.Sliver) + "銀 " : "") +
                    (item.price.Cooper ? std::to_string(item.price.Cooper) + "銅" : "")
                );
				}
            ui.showGoods(names, descs, prices);
            ui.showMoney(p.showmoney());
		}
    } else if (type == MerchantType::Equipment) {
        ui.showTitle("歡迎來到裝備商店！");
        std::vector<std::string> names, descs, prices;
        for (auto& item : EquipmentList) {
            names.push_back(item.equip.getName());
            descs.push_back(item.equip.getDesc());
            std::string priceStr =
                (item.price.Gold ? std::to_string(item.price.Gold) + "金 " : "") +
                (item.price.Sliver ? std::to_string(item.price.Sliver) + "銀 " : "") +
                (item.price.Cooper ? std::to_string(item.price.Cooper) + "銅" : "");
            for (auto& materialneed : item.NeedMaterial) {
                priceStr += " 需:" + materialneed.getName() + "x" + std::to_string(materialneed.amount);
            }
            prices.push_back(priceStr);
			}
        ui.showGoods(names, descs, prices);
        ui.showMoney(p.showmoney());
        while (true) {
            int choose = ui.selectGoods((int)EquipmentList.size());
            if (choose < 0 || choose >= (int)EquipmentList.size()) break;
			if (p.HaveEnoughMoney(EquipmentList[choose].price) && p.HaveEnoughMaterial(EquipmentList[choose].NeedMaterial)) {
					p.spendmoney(EquipmentList[choose].price);
					p.getEquip(EquipmentList[choose].equip);
					for (auto& material : EquipmentList[choose].NeedMaterial) {
						p.throwMaterial(material);
					}
                ui.showTip("您已成功購買 " + EquipmentList[choose].equip.getName());
					EquipmentList.erase(EquipmentList.begin() + choose);
            } else {
                ui.showTip("您的錢或材料不足以買下這份商品。");
            }
            ui.wait();
            // 更新商品與金錢顯示
            names.clear(); descs.clear(); prices.clear();
            for (auto& item : EquipmentList) {
                names.push_back(item.equip.getName());
                descs.push_back(item.equip.getDesc());
                std::string priceStr =
                    (item.price.Gold ? std::to_string(item.price.Gold) + "金 " : "") +
                    (item.price.Sliver ? std::to_string(item.price.Sliver) + "銀 " : "") +
                    (item.price.Cooper ? std::to_string(item.price.Cooper) + "銅" : "");
                for (auto& materialneed : item.NeedMaterial) {
                    priceStr += " 需:" + materialneed.getName() + "x" + std::to_string(materialneed.amount);
                }
                prices.push_back(priceStr);
            }
            ui.showGoods(names, descs, prices);
            ui.showMoney(p.showmoney());
        }
    } else if (type == MerchantType::Misery) {
        ui.showTitle("歡迎來到道具商店！");
        std::vector<std::string> names, descs, prices;
        for (auto& item : MiseryList) {
            names.push_back(item.misery.getName());
            descs.push_back(item.misery.getDesc());
            prices.push_back(
                (item.price.Gold ? std::to_string(item.price.Gold) + "金 " : "") +
                (item.price.Sliver ? std::to_string(item.price.Sliver) + "銀 " : "") +
                (item.price.Cooper ? std::to_string(item.price.Cooper) + "銅" : "")
            );
        }
        ui.showGoods(names, descs, prices);
        ui.showMoney(p.showmoney());
		while (true) {
            int choose = ui.selectGoods((int)MiseryList.size());
            if (choose < 0 || choose >= (int)MiseryList.size()) break;
			if (p.HaveEnoughMoney(MiseryList[choose].price)) {
				p.spendmoney(MiseryList[choose].price);
				p.getMyseryItem(MiseryList[choose].misery);
                ui.showTip("您已成功購買 " + MiseryList[choose].misery.getName());
				MiseryList.erase(MiseryList.begin() + choose);
            } else {
                ui.showTip("您的錢不足以買下這份商品。");
            }
            ui.wait();
            // 更新商品與金錢顯示
            names.clear(); descs.clear(); prices.clear();
            for (auto& item : MiseryList) {
                names.push_back(item.misery.getName());
                descs.push_back(item.misery.getDesc());
                prices.push_back(
                    (item.price.Gold ? std::to_string(item.price.Gold) + "金 " : "") +
                    (item.price.Sliver ? std::to_string(item.price.Sliver) + "銀 " : "") +
                    (item.price.Cooper ? std::to_string(item.price.Cooper) + "銅" : "")
                );
			}
            ui.showGoods(names, descs, prices);
            ui.showMoney(p.showmoney());
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


