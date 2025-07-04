#pragma once
#include "Item.h"

struct TreasureBox {
	std::vector<Equip> ecEquip; 
	std::vector<Material> ecMaterial;
	std::vector<MiseryItem> ecMiseryItem; 
	bool IsOpened = false;
	static std::vector<Equip> allEquip;
	static std::vector<Material> allMaterial;
	static std::vector<MiseryItem> allMiseryItem;
	bool randomset(int num) {
		bool notempty = false;
		if (!allMaterial.empty()) {
			for(int i = 0;i<rand()%num; i++) {
				int materialIndex = rand() %  allMaterial.size();
				ecMaterial.push_back(allMaterial[materialIndex]);
				notempty = true;
			}
		}
		if(!allEquip.empty()) {
			for(int i = 0;i<rand()%num; i++) {
				int equipIndex = rand() %  allEquip.size();
				ecEquip.push_back(allEquip[equipIndex]);
				notempty = true;
			}
		}
		if (!allMiseryItem.empty()) {
			for (int i = 0;i < rand() % num; i++) {
				int miseryItemIndex = rand() % allMiseryItem.size();
				ecMiseryItem.push_back(allMiseryItem[miseryItemIndex]);
				notempty = true;
			}
		}
		return notempty;
	}
};
