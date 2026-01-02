#include <iostream>
#include <cmath>
#include <windows.h>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

const unsigned int MAX_BRAND_LEN = 50;
const unsigned int MAX_MODEL_LEN = 50;
const unsigned int MAX_CPU_LEN = 50;
const unsigned int MAX_CPU_MANUFACTURER_LEN = 50;

const int MAX_CONFIGS = 100;
const char* DATA_FILENAME = "configs.bin";

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

void LoadFromFile(Configuration configs[], int& count);
void SaveToFile(Configuration configs[], int count);
void Menu(Configuration configs[], int& count);
void AddConfigs(Configuration configs[], int& count);
void ShowAll(Configuration configs[], int count);
bool SerialExists(Configuration configs[], int count, unsigned int serial);
void ShowMaxRAM(Configuration configs[], int count);
void SearchByCPUManufacturer(Configuration configs[], int count);
void SortByPrice(Configuration configs[], int count);
void SubmenuExtra(Configuration configs[], int count);
void ShowUnsoldSortedBySerial(Configuration configs[], int count);
void SearchByBrandAndModel(Configuration configs[], int count);
void SearchByCriteria(Configuration configs[], int count);
void SellBySerial(Configuration configs[], int count);
void FileMenu(Configuration configs[], int& count);
void writeStringToStream(ofstream& ofs, const string& s, unsigned int maxLen);
void readStringFromStream(ifstream& ifs, string& s, unsigned int maxLen);
void readStringInput(const string& prompt, string& out, unsigned int maxLen);

int main()
{
    SetConsoleOutputCP(1251);
    int configCount = 0;
    Configuration configs[MAX_CONFIGS];
    LoadFromFile(configs, configCount);
    while (true) {
        Menu(configs, configCount);
    }
    return 0;
}

void SaveToFile(Configuration configs[], int count)
{
    ofstream ofs(DATA_FILENAME, ios::binary);
    if (!ofs) {
        cout << "Грешка при отваряне на файл за запис: " << DATA_FILENAME << endl;
        return;
    }
    unsigned int c = (unsigned int)count;
    ofs.write((const char*)&c, sizeof(c));
    for (int i = 0; i < count; ++i) {
        ofs.write((const char*)&configs[i].serialNumber, sizeof(configs[i].serialNumber));
        writeStringToStream(ofs, configs[i].brand, MAX_BRAND_LEN);
        writeStringToStream(ofs, configs[i].model, MAX_MODEL_LEN);
        writeStringToStream(ofs, configs[i].CPU, MAX_CPU_LEN);
        writeStringToStream(ofs, configs[i].cpuManufacturer, MAX_CPU_MANUFACTURER_LEN);
        unsigned short cores = configs[i].cores;
        unsigned short RAM = configs[i].RAM;
        ofs.write((const char*)&cores, sizeof(cores));
        ofs.write((const char*)&RAM, sizeof(RAM));
        ofs.write((const char*)&configs[i].price, sizeof(configs[i].price));
        unsigned char sold = configs[i].isSold ? 1 : 0;
        ofs.write((const char*)&sold, sizeof(sold));
    }
    ofs.close();
}

void LoadFromFile(Configuration configs[], int& count)
{
    ifstream ifs(DATA_FILENAME, ios::binary);
    if (!ifs) {
        count = 0;
        return;
    }
    unsigned int c = 0;
    ifs.read((char*)&c, sizeof(c));
    int toRead = (int)c;
    if (toRead > MAX_CONFIGS) toRead = MAX_CONFIGS;
    count = 0;
    for (int i = 0; i < toRead; ++i) {
        Configuration cfg;
        ifs.read((char*)&cfg.serialNumber, sizeof(cfg.serialNumber));
        readStringFromStream(ifs, cfg.brand, MAX_BRAND_LEN);
        readStringFromStream(ifs, cfg.model, MAX_MODEL_LEN);
        readStringFromStream(ifs, cfg.CPU, MAX_CPU_LEN);
        readStringFromStream(ifs, cfg.cpuManufacturer, MAX_CPU_MANUFACTURER_LEN);
        unsigned short cores = 0, RAM = 0;
        ifs.read((char*)&cores, sizeof(cores));
        ifs.read((char*)&RAM, sizeof(RAM));
        cfg.cores = cores;
        cfg.RAM = RAM;
        ifs.read((char*)&cfg.price, sizeof(cfg.price));
        unsigned char sold = 0;
        ifs.read((char*)&sold, sizeof(sold));
        cfg.isSold = (sold != 0);
        configs[count++] = cfg;
    }
    ifs.close();
}

void writeStringToStream(ofstream& ofs, const string& s, unsigned int maxLen)
{
    unsigned int len = (s.size() <= (size_t)maxLen) ? (unsigned int)s.size() : maxLen;
    ofs.write((const char*)&len, sizeof(len));
    if (len) ofs.write(s.data(), len);
}

void readStringFromStream(ifstream& ifs, string& s, unsigned int maxLen)
{
    unsigned int len = 0;
    ifs.read((char*)&len, sizeof(len));
    s.clear();
    if (len) {
        string tmp;
        tmp.resize(len);
        ifs.read(&tmp[0], len);
        if (len > maxLen) tmp = tmp.substr(0, maxLen);
        s = tmp;
    }
}

void readStringInput(const string& prompt, string& out, unsigned int maxLen)
{
    cout << prompt;
    string tmp;
    getline(cin >> ws, tmp);
    if (tmp.size() > maxLen) {
        cout << "(Внимание: въведената стойност е съкратена до " << maxLen << " символа)\n";
        tmp = tmp.substr(0, maxLen);
    }
    out = tmp;
}

void Menu(Configuration configs[], int& count)
{
    int choice;
    cout << "\n=== М Е Н Ю ===\n";
    cout << "1. Добави нова конфигурация\n";
    cout << "2. Покажи всички конфигурации\n";
    cout << "3. Търсене и извеждане (подменю)\n";
    cout << "4. Подреждане по цена (нарастващо)\n";
    cout << "5. Работа с файл (вмък/извеждане)\n";
    cout << "6. Подменю допълнително (G)\n";
    cout << "7. Търсене подходяща конфигурация / Продай (H)\n";
    cout << "8. Изход (запазва автоматично)\n";
    cout << "Избор: ";
    cin >> choice;
    switch (choice)
    {
    case 1: AddConfigs(configs, count); break;
    case 2: ShowAll(configs, count); break;
    case 3:
        cout << "\nТърсене:\n1) Най-голяма RAM\n2) По производител на процесор\nИзбор: ";
        {
            int s; cin >> s;
            if (s == 1) ShowMaxRAM(configs, count);
            else if (s == 2) SearchByCPUManufacturer(configs, count);
            else cout << "Невалиден избор.\n";
        }
        break;
    case 4: SortByPrice(configs, count); cout << "Подредено по цена.\n"; break;
    case 5: FileMenu(configs, count); break;
    case 6: SubmenuExtra(configs, count); break;
    case 7: SearchByCriteria(configs, count); break;
    case 8:
        SaveToFile(configs, count);
        cout << "Данните са записани. Край на програмата.\n";
        exit(0);
        break;
    default: cout << "Невалиден избор!\n";
    }
}

void AddConfigs(Configuration configs[], int& count)
{
    int n;
    unsigned int inputSerial;
    bool ifSerialExists;
    cout << "Колко конфигурации искате да добавите? ";
    cin >> n;
    if (n <= 0) {
        cout << "Грешен брой!\n";
        return;
    }
    if (count + n > MAX_CONFIGS) {
        cout << "Няма достатъчно място! Свободни: " << (MAX_CONFIGS - count) << endl;
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
        } while (inputSerial < 10000000 || inputSerial > 99999999 || ifSerialExists == true);
        readStringInput("Марка: ", configs[count].brand, MAX_BRAND_LEN);
        readStringInput("Модел: ", configs[count].model, MAX_MODEL_LEN);
        readStringInput("Процесор (име): ", configs[count].CPU, MAX_CPU_LEN);
        readStringInput("Производител на процесора: ", configs[count].cpuManufacturer, MAX_CPU_MANUFACTURER_LEN);
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

void ShowMaxRAM(Configuration configs[], int count)
{
    if (count == 0) {
        cout << "Няма въведени конфигурации.\n";
        return;
    }
    unsigned short maxRAM = 0;
    for (int i = 0; i < count; ++i) {
        if (configs[i].RAM > maxRAM) maxRAM = configs[i].RAM;
    }
    cout << "Най-голяма RAM: " << maxRAM << " GB. Конфигурации:\n";
    for (int i = 0; i < count; ++i) {
        if (configs[i].RAM == maxRAM) {
            cout << "- Сер.номер: " << configs[i].serialNumber << " | " << configs[i].brand << " " << configs[i].model << " | RAM: " << configs[i].RAM << "GB | Цена: " << configs[i].price << " | Статус: " << (configs[i].isSold ? "Продадена" : "В продажба") << endl;
        }
    }
}

void SearchByCPUManufacturer(Configuration configs[], int count)
{
    if (count == 0) {
        cout << "Няма въведени конфигурации.\n";
        return;
    }
    string manufacturer;
    readStringInput("Въведете производител на процесора: ", manufacturer, MAX_CPU_MANUFACTURER_LEN);
    bool found = false;
    for (int i = 0; i < count; ++i) {
        if (_stricmp(configs[i].cpuManufacturer.c_str(), manufacturer.c_str()) == 0) {
            if (!found) {
                cout << "Намерени конфигурации:\n";
                found = true;
            }
            cout << "- Сер.номер: " << configs[i].serialNumber << " | " << configs[i].brand << " " << configs[i].model << " | CPU: " << configs[i].CPU << " | RAM: " << configs[i].RAM << "GB | Цена: " << configs[i].price << " | Статус: " << (configs[i].isSold ? "Продадена" : "В продажба") << endl;
        }
    }
    if (!found) cout << "Не са намерени конфигурации за този производител.\n";
}

void SortByPrice(Configuration configs[], int count)
{
    if (count <= 1) {
        cout << "Няма нужда от сортиране.\n";
        return;
    }
    for (int i = 0; i < count - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < count; ++j) {
            if (configs[j].price < configs[minIdx].price) minIdx = j;
        }
        if (minIdx != i) {
            Configuration tmp = configs[i];
            configs[i] = configs[minIdx];
            configs[minIdx] = tmp;
        }
    }
}

void FileMenu(Configuration configs[], int& count)
{
    cout << "\nРабота с файл:\n1) Запиши масива във файл (двоичен)\n2) Зареди масива от файл (двоичен)\nИзбор: ";
    int ch; cin >> ch;
    if (ch == 1) {
        SaveToFile(configs, count);
        cout << "Записано в " << DATA_FILENAME << endl;
    }
    else if (ch == 2) {
        LoadFromFile(configs, count);
        cout << "Заредено от " << DATA_FILENAME << ". Текущ брой: " << count << endl;
    }
    else {
        cout << "Невалиден избор.\n";
    }
}

void SubmenuExtra(Configuration configs[], int count)
{
    cout << "\nПодменю G:\n1) Покажи всички непродадени, подредени по сериен номер\n2) Търсене по марка и модел\nИзбор: ";
    int ch; cin >> ch;
    if (ch == 1) ShowUnsoldSortedBySerial(configs, count);
    else if (ch == 2) SearchByBrandAndModel(configs, count);
    else cout << "Невалиден избор.\n";
}

void ShowUnsoldSortedBySerial(Configuration configs[], int count)
{
    Configuration unsold[MAX_CONFIGS];
    int unsoldCount = 0;
    for (int i = 0; i < count; ++i) if (!configs[i].isSold) { unsold[unsoldCount++] = configs[i]; }
    if (unsoldCount == 0) { cout << "Няма непродадени конфигурации.\n"; return; }
    for (int i = 0; i < unsoldCount - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < unsoldCount; ++j) {
            if (unsold[j].serialNumber < unsold[minIdx].serialNumber) minIdx = j;
        }
        if (minIdx != i) {
            Configuration tmp = unsold[i];
            unsold[i] = unsold[minIdx];
            unsold[minIdx] = tmp;
        }
    }
    cout << "Непродадени конфигурации (подредени по сериен номер):\n";
    for (int i = 0; i < unsoldCount; ++i) {
        cout << "- " << unsold[i].serialNumber << " | " << unsold[i].brand << " " << unsold[i].model << " | RAM: " << unsold[i].RAM << "GB | Цена: " << unsold[i].price << endl;
    }
}

void SearchByBrandAndModel(Configuration configs[], int count)
{
    if (count == 0) { cout << "Няма въведени конфигурации.\n"; return; }
    string brand, model;
    readStringInput("Въведете марка: ", brand, MAX_BRAND_LEN);
    readStringInput("Въведете модел: ", model, MAX_MODEL_LEN);
    bool found = false;
    for (int i = 0; i < count; ++i) {
        if (_stricmp(configs[i].brand.c_str(), brand.c_str()) == 0 &&
            _stricmp(configs[i].model.c_str(), model.c_str()) == 0) {
            if (!found) { cout << "Намерени:\n"; found = true; }
            cout << "- " << configs[i].serialNumber << " | CPU: " << configs[i].CPU << " | RAM: " << configs[i].RAM << "GB | Цена: " << configs[i].price << " | " << (configs[i].isSold ? "Продадена" : "В продажба") << endl;
        }
    }
    if (!found) cout << "Не са намерени конфигурации с посочената марка и модел.\n";
}

void SearchByCriteria(Configuration configs[], int count)
{
    if (count == 0) { cout << "Няма въведени конфигурации.\n"; return; }
    cout << "\nТърсене на подходяща конфигурация. Изберете критерий:\n";
    cout << "1) Марка и модел\n2) Процесор - производител\n3) Брой ядра\n4) RAM памет (минимум)\n5) Максимална цена\nИзбор: ";
    int ch; cin >> ch;
    int matches[MAX_CONFIGS];
    int matchesCount = 0;
    if (ch == 1) {
        string brand, model;
        readStringInput("Марка: ", brand, MAX_BRAND_LEN);
        readStringInput("Модел: ", model, MAX_MODEL_LEN);
        for (int i = 0; i < count; ++i)
            if (!configs[i].isSold && _stricmp(configs[i].brand.c_str(), brand.c_str()) == 0 && _stricmp(configs[i].model.c_str(), model.c_str()) == 0)
                matches[matchesCount++] = i;
    }
    else if (ch == 2) {
        string manufacturer;
        readStringInput("Производител на процесора: ", manufacturer, MAX_CPU_MANUFACTURER_LEN);
        for (int i = 0; i < count; ++i)
            if (!configs[i].isSold && _stricmp(configs[i].cpuManufacturer.c_str(), manufacturer.c_str()) == 0)
                matches[matchesCount++] = i;
    }
    else if (ch == 3) {
        unsigned short cores; cout << "Брой ядра (точно): "; cin >> cores;
        for (int i = 0; i < count; ++i)
            if (!configs[i].isSold && configs[i].cores == cores)
                matches[matchesCount++] = i;
    }
    else if (ch == 4) {
        unsigned short minRAM; cout << "Минимална RAM (GB): "; cin >> minRAM;
        for (int i = 0; i < count; ++i)
            if (!configs[i].isSold && configs[i].RAM >= minRAM)
                matches[matchesCount++] = i;
    }
    else if (ch == 5) {
        double maxPrice; cout << "Максимална цена: "; cin >> maxPrice;
        for (int i = 0; i < count; ++i)
            if (!configs[i].isSold && configs[i].price <= maxPrice)
                matches[matchesCount++] = i;
    }
    else {
        cout << "Невалиден избор.\n";
        return;
    }
    if (matchesCount == 0) {
        cout << "Няма намерени конфигурации за избрания критерий.\n";
        return;
    }
    cout << "Намерени конфигурации:\n";
    for (int i = 0; i < matchesCount; ++i) {
        int idx = matches[i];
        cout << "- Сер.номер: " << configs[idx].serialNumber << " | " << configs[idx].brand << " " << configs[idx].model << " | CPU: " << configs[idx].CPU << " | RAM: " << configs[idx].RAM << "GB | Цена: " << configs[idx].price << endl;
    }
    cout << "Желаете ли да продадете някоя от тези конфигурации? (1=Да, 0=Не): ";
    int sellChoice; cin >> sellChoice;
    if (sellChoice == 1) {
        SellBySerial(configs, count);
    }
}

void SellBySerial(Configuration configs[], int count)
{
    unsigned int serial; cout << "Въведете сериен номер за продажба: "; cin >> serial;
    int foundIdx = -1;
    for (int i = 0; i < count; ++i) if (configs[i].serialNumber == serial) { foundIdx = i; break; }
    if (foundIdx == -1) {
        cout << "Не е открита конфигурация с посочения сериен номер.\n";
        return;
    }
    Configuration& cfg = configs[foundIdx];
    if (cfg.isSold) {
        cout << "Конфигурацията вече не е налична.\n";
        return;
    }
    cout << "Намерена: " << cfg.brand << " " << cfg.model << " | Цена: " << cfg.price << endl;
    cout << "Потвърждавате ли продажбата? (1=Да, 0=Не): ";
    int confirm; cin >> confirm;
    if (confirm != 1) { cout << "Продажбата отменена.\n"; return; }
    cout << "Имате ли код за намаление? (1=Да, 0=Не): ";
    int hasCode; cin >> hasCode;
    double oldPrice = cfg.price;
    double newPrice = oldPrice;
    if (hasCode == 1) {
        int code; cout << "Въведете 4-цифрен код: "; cin >> code;
        if (code >= 1000 && code <= 9999 && (code % 56 == 0)) {
			newPrice = oldPrice * 0.95; // 5% отстъпка
            cout << "Код валиден. Приложена 5% отстъпка.\n";
        }
        else {
            cout << "Код невалиден — няма отстъпка.\n";
        }
    }
    cfg.isSold = true;
    cout << "Успешна продажба.\nСтара цена: " << oldPrice << " -> Нова цена: " << newPrice << endl;
}