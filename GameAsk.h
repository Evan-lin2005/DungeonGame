#pragma once
#include <iostream>

void GameAsk(int& h, int& w, int& floor) {
	using namespace std;
	cout << "歡迎來到地下城系統(請選擇難度) : " << std::endl;
	cout << "1. 簡易(3層)" << std::endl;
	cout << "2. 普通(6層)" << std::endl;
	cout << "3. 困難(9層)" << std::endl;
	cout << "4. 夢魘(12層)" << std::endl;
	cout << "5 絕望(15層)" << std::endl;
	string c;
	while (true) {
		
		getline(cin, c);
		if (c == "1") {
			h = 20;w = 20;floor = 3;
		}
		else if (c == "2") {
			h = 25;w = 25;floor = 6;
		}
		else if (c == "3") {
			h = 30;w = 30;floor = 9;
		}
		else if (c == "4") {
			h = 35;w = 35;floor = 12;
		}
		else if (c == "5") {
			h = 40;w = 40;floor = 15;
		}
		else {
			cout << "輸入錯誤指令，請重新驗證" << endl;
			continue;
		}
		break;
	}
}
