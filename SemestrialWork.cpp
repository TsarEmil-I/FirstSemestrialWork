#include <iostream>
#include <cmath>
#include <windows.h>

using namespace std;

struct Configuration
{
	unsigned int serialNumber;
	string brand;
	string model;
	string CPU;
	string cpuManufacturer;
	unsigned short int cores;
	unsigned short int RAM;
	double price;
	bool isSold;
};

void Menu(Configuration configs[], int& count);
void AddConfigs(Configuration configs[], int& count);
void ShowAll(Configuration configs[], int count);
bool SerialExists(Configuration configs[], int count, unsigned int serial);

int main()
{
	SetConsoleOutputCP(1251);
	int configCount = 0;
	Configuration configs[100];
	
	while (true) {
		Menu(configs, configCount);
	}
}

void Menu(Configuration configs[], int& count)
{
	int choice;

	cout << "\n=== М Е Н Ю ===\n";
	cout << "1. Добави нова конфигурация\n";
	cout << "2. Покажи всички конфигурации\n";
	cout << "3. Търсене\n"; // TO DO with 3a and 3b. I will think about the addition funcs later
	cout << "4. Подреждане по цена\n";
	cout << "5. Работа с файл\n";
	cout << "6. Изход\n";

	cout << "Избор: ";
	cin >> choice;

	switch (choice)
	{
	case 1: AddConfigs(configs, count); break;
	case 2: ShowAll(configs, count); break;
	case 6: exit(0);
	default: cout << "Невалиден избор!\n";
	}
}

void AddConfigs(Configuration configs[], int& count)
{
	int n;
	int inputSerial;
	bool ifSerialExists;
	cout << "Колко конфигурации искате да добавите? ";
	cin >> n;

	if (n <= 0) {
		cout << "Грешен брой!\n";
		return;
	}

	if (count + n > 100) {
		cout << "Няма достатъчно място! Свободни: " << (100 - count) << endl;
		return;
	}

	for (int i = 0; i < n; i++) {
		cout << "\nКонфигурация #" << count + 1 << endl;

		cout << "Сериен номер (8-цифрено число): ";
		do
		{
			cin >> inputSerial;
			ifSerialExists = SerialExists(configs, count, inputSerial);
			if (inputSerial < 10000000 || inputSerial > 99999999)
			{
				cout << "Грешен сериен номер! Опитайте отново: ";
			}
			else if (ifSerialExists == true)
			{
				cout << "Серийният номер вече съществува! Опитайте отново: ";
			}
			else
			{
				configs[count].serialNumber = inputSerial;
			}
		} while (inputSerial < 10000000 || inputSerial > 99999999 || ifSerialExists == true); // нека се генерира автоматично по-нататък

		cout << "Марка: ";
		cin >> configs[count].brand;

		cout << "Модел: ";
		cin >> configs[count].model;

		cout << "Процесор: ";
		cin >> configs[count].CPU;

		cout << "Производител на процесора: ";
		cin >> configs[count].cpuManufacturer;

		cout << "Брой ядра: ";
		cin >> configs[count].cores;

		cout << "RAM (GB): ";
		cin >> configs[count].RAM;

		cout << "Цена: ";
		cin >> configs[count].price;

		configs[count].isSold = false; 

		count++;
	}

	cout << "\nУспешно добавени " << n << " конфигурации.\n" << endl;
}

void ShowAll(Configuration configs[], int count)
{
	if (count == 0) {
		cout << "Няма въведени конфигурации.\n";
		return;
	}

	cout << "\n=== ВСИЧКИ КОНФИГУРАЦИИ ===\n";

	for (int i = 0; i < count; i++) {
		cout << "\n#" << i + 1 << endl;
		cout << "Сериен номер: " << configs[i].serialNumber << endl;
		cout << "Марка: " << configs[i].brand << endl;
		cout << "Модел: " << configs[i].model << endl;
		cout << "Процесор: " << configs[i].CPU << endl;
		cout << "CPU производител: " << configs[i].cpuManufacturer << endl;
		cout << "Ядра: " << configs[i].cores << endl;
		cout << "RAM: " << configs[i].RAM << " GB" << endl;
		cout << "Цена: " << configs[i].price << endl;
		cout << "Статус: " << (configs[i].isSold ? "Продадена" : "В продажба") << endl;
	}
}

bool SerialExists(Configuration configs[], int count, unsigned int serial)
{
	for (int i = 0; i < count; i++) {
		if (configs[i].serialNumber == serial)
			return true;
	}
	return false; 
}
