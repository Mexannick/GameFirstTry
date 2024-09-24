#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream> 
#include <string>

using namespace std;

// Размеры карты
const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 10;

// Максимальное здоровье игрока
const int PLAYER_MAX_HEALTH = 20;

// Список редкости артефактов
enum ArtifactRarity { COMMON, UNCOMMON, RARE, EPIC, LEGENDARY, FORBIDDEN };
const char* rarity_names[] = { "Common", "Uncommon", "Rare", "Epic", "Legendary", "Forbidden" };

// Структура игрока
struct Player {
    int x, y;
    int health;
    int artifact_count[6] = { 0 };  // Количество артефактов каждой редкости
    string weapon;
    int monstersKilled;  // Количество убитых монстров
};

// Структура монстра
struct Monster {
    int x, y;
    int health;
};

// Структура артефакта
struct Artifact {
    int x, y;
    ArtifactRarity rarity;
    int healthAdd;
};

// Класс игры
class Game {
public:
    Game() : levelNumber(1) {
        srand(static_cast<unsigned int>(time(0)));
        player.health = PLAYER_MAX_HEALTH;
        player.monstersKilled = 0;  // Обнуление убитых монстров
        player.weapon = "Bare Hands";
        showLoadingScreen();  // Показ загрузочного экрана перед началом игры
        generateMap();
    }

    // Основной игровой цикл
    void play() {
        char command;
        while (player.health > 0) {
            printMap();
            // Генерация лестницы "YYY"
            map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = 'Y';
            map[MAP_HEIGHT - 2][MAP_WIDTH - 3] = 'Y';
            map[MAP_HEIGHT - 2][MAP_WIDTH - 4] = 'Y';
            cout << "Use WASD to move: ";
            cin >> command;

            movePlayer(command);
            checkTile();
        }

        cout << "Game Over! You died." << endl;
    }

private:
    char map[MAP_HEIGHT][MAP_WIDTH];
    Player player;
    vector<Monster> monsters;
    vector<Artifact> artifacts;
    int levelNumber;

    // Генерация карты с игроком, монстрами и артефактами
    void generateMap() {
        // Регенерируем карту
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                if (i == 0 || i == MAP_HEIGHT - 1 || j == 0 || j == MAP_WIDTH - 1) {
                    map[i][j] = '#'; // Стены
                }
                else {
                    map[i][j] = '.';
                }
            }
        }

        // Генерация лестницы "YYY"
        map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = 'Y';
        map[MAP_HEIGHT - 2][MAP_WIDTH - 3] = 'Y';
        map[MAP_HEIGHT - 2][MAP_WIDTH - 4] = 'Y';

        // Размещаем игрока в центре
        player.x = MAP_WIDTH / 2;
        player.y = MAP_HEIGHT / 2;
        map[player.y][player.x] = 'O';

        // Генерация монстров
        monsters.clear();
        int monsterCount = levelNumber + 2;
        for (int i = 0; i < monsterCount; i++) {
            Monster monster = { rand() % (MAP_WIDTH - 2) + 1, rand() % (MAP_HEIGHT - 2) + 1, rand() % 3 + 1 };
            monsters.push_back(monster);
            map[monster.y][monster.x] = 'M';
        }

        // Генерация артефактов
        artifacts.clear();
        int artifactCount = levelNumber + 1;
        for (int i = 0; i < artifactCount; i++) {
            Artifact artifact = { rand() % (MAP_WIDTH - 2) + 1, rand() % (MAP_HEIGHT - 2) + 1, generateArtifactRarity() };
            artifacts.push_back(artifact);
            map[artifact.y][artifact.x] = 'A';
        }

    }

    // Отрисовка карты и информации о состоянии игрока
    void printMap() {
        system("cls");  // Для Windows можно использовать "system("cls")"
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int j = 0; j < MAP_WIDTH; j++) {
                cout << map[i][j];
            }
            cout << endl;
        }
        cout << "Health: " << player.health << endl;
        cout << "Weapon: " << player.weapon << endl;
        // Отображение количества убитых монстров
        cout << "Monsters Killed: " << player.monstersKilled << "/" << monsters.size() + player.monstersKilled << endl;
        // Отображение количества артефактов по редкости
        cout << "Inventory (Artifacts):" << endl;
        for (int i = 0; i < 6; i++) {
            cout << rarity_names[i] << ": " << player.artifact_count[i] << endl;
        }
        cout << endl;
    }

    // Движение игрока
    void movePlayer(char command) {
        int newX = player.x;
        int newY = player.y;

        if (command == 'w') newY--;
        else if (command == 's') newY++;
        else if (command == 'a') newX--;
        else if (command == 'd') newX++;
        cout << map[newY][newX] << endl;

        if (map[newY][newX] == 'Y') {
            if (monsters.empty()) {
                cout << "You found the stairs to the next level!" << endl;
                saveInventoryToFile();  // Сохранение инвентаря
                levelNumber++;
                generateMap();  // Перегенерация карты
                // Генерация лестницы "YYY"
                map[MAP_HEIGHT - 2][MAP_WIDTH - 2] = 'Y';
                map[MAP_HEIGHT - 2][MAP_WIDTH - 3] = 'Y';
                map[MAP_HEIGHT - 2][MAP_WIDTH - 4] = 'Y';

                // Размещаем игрока в центре
                //player.x = MAP_WIDTH / 2;
                //player.y = MAP_HEIGHT / 2;
                //map[player.y][player.x] = 'O';
            }
            else {
                cout << "You must defeat all monsters before proceeding to the next level!" << endl;
            }
        }
        // Проверяем, можно ли двигаться на новую клетку
        if (map[newY][newX] != '#') {
            map[player.y][player.x] = ' ';  // Стираем старое положение
            player.x = newX;
            player.y = newY;
            map[player.y][player.x] = 'O';  // Новое положение игрока
        }
    }

    // Проверка, что находится на текущей клетке
    void checkTile() {
        // Проверка на монстра
        for (auto it = monsters.begin(); it != monsters.end(); ++it) {
            if (player.x == it->x && player.y == it->y) {
                fightMonster(*it);
                if (it->health <= 0) {
                    map[it->y][it->x] = ' ';
                    monsters.erase(it);
                    player.monstersKilled++;
                    map[player.y][player.x] = 'O';
                }
                return;
            }
        }

        // Проверка на артефакт
        for (auto it = artifacts.begin(); it != artifacts.end(); ++it) {
            if (player.x == it->x && player.y == it->y) {
                collectArtifact(*it);
                map[it->y][it->x] = ' ';
                artifacts.erase(it);
                map[player.y][player.x] = 'O';
                return;
            }
        }


    }

    // Бой с монстром
    void fightMonster(Monster& monster) {
        while (player.health > 0 && monster.health > 0) {
            int playerRoll = rand() % 31;  // Значение для игрока от 0 до 30
            int monsterRoll = rand() % 21; // Значение для монстра от 0 до 20

            if (playerRoll > monsterRoll) {
                cout << "You hit the monster!" << endl;
                monster.health -= 1;
            }
            else {
                cout << "The monster hits you!" << endl;
                player.health -= 1;
            }
        }
    }

    // Сбор артефакта
    void collectArtifact(Artifact& artifact) {
        cout << "You found an artifact of rarity " << rarity_names[artifact.rarity] << "!" << endl;
        player.artifact_count[artifact.rarity]++;
        if (artifact.rarity == LEGENDARY || artifact.rarity == FORBIDDEN) {
            player.weapon = rarity_names[artifact.rarity] + string(" Weapon");
            cout << "You equipped the " << player.weapon << "!" << endl;
        }
    }

    // Генерация редкости артефакта
    ArtifactRarity generateArtifactRarity() {
        int roll = rand() % 100 + 1;  // Значение от 1 до 100
        if (roll <= 40) return COMMON;      // 40% шанс
        else if (roll <= 65) return UNCOMMON; // 25% шанс
        else if (roll <= 85) return RARE;      // 20% шанс
        else if (roll <= 95) return EPIC;      // 10% шанс
        else if (roll <= 99) return LEGENDARY; // 4% шанс
        else return FORBIDDEN;                // 1% шанс
    }

    // Сохранение инвентаря в файл
    void saveInventoryToFile() {
        ofstream file("inventory.txt");
        if (file.is_open()) {
            for (int i = 0; i < 6; i++) {
                file << rarity_names[i] << ": " << player.artifact_count[i] << endl;
            }
            file << "MonstersKilled: " << player.monstersKilled << endl;
            file.close();
        }
        else {
            cout << "Unable to open file for saving!" << endl;
        }
    }

    // Показ загрузочного экрана и загрузка инвентаря
    void showLoadingScreen() {
        ifstream file("inventory.txt");
        if (file.is_open()) {
            string line;
            int artifact_index = 0;  // Индекс для чтения артефактов
            while (getline(file, line)) {
                // Проверка строки на информацию об артефактах
                if (artifact_index < 6 && line.find(rarity_names[artifact_index]) != string::npos) {
                    // Чтение количества артефактов
                    size_t pos = line.find(": ");
                    if (pos != string::npos) {
                        int count = stoi(line.substr(pos + 2));  // Извлечение числа после ": "
                        player.artifact_count[artifact_index] = count;
                    }
                    artifact_index++;  // Переходим к следующей редкости
                }
                // Проверка строки на информацию об убитых монстрах
                else if (line.find("MonstersKilled:") != string::npos) {
                    size_t pos = line.find(": ");
                    if (pos != string::npos) {
                        player.monstersKilled = stoi(line.substr(pos + 2));  // Извлечение числа убитых монстров
                    }
                }
            }
            file.close();

            // Отображение загруженных данных
            cout << "Loading previous game progress..." << endl;
            for (int i = 0; i < 6; i++) {
                cout << rarity_names[i] << ": " << player.artifact_count[i] << endl;
            }
            cout << "Monsters Killed: " << player.monstersKilled << endl;
            cin;

        }
        else {
            cout << "Welcome to the dungeon! Good luck!" << endl;
        }
    }

};

int main() {
    Game game;
    game.play();
    return 0;
}
