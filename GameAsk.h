#pragma once
#include <iostream>

void GameAsk(int& h, int& w, int& floor) {
	using namespace std;
	cout << "�w��Ө�a�U���t��(�п������) : " << std::endl;
	cout << "1. ²��(3�h)" << std::endl;
	cout << "2. ���q(6�h)" << std::endl;
	cout << "3. �x��(9�h)" << std::endl;
	cout << "4. ���L(12�h)" << std::endl;
	cout << "5 ����(15�h)" << std::endl;
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
			cout << "��J���~���O�A�Э��s����" << endl;
			continue;
		}
		break;
	}
}
