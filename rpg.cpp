#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <cmath>
#include "GameField.h"
#include "Base.h"
#include "UnitFactory.h"


void clearScreen() {
    std::cout << "\033[2J\033[1;1H" << std::flush;
}

void waitForEnter() {
    std::cin.ignore(10000, '\n');
    std::cout << "\nPress Enter to continue..." << std::flush;
    std::cin.get();
}

int findUnitIndexById(const GameField& field, int id) {
    for (std::size_t i = 0; i < field.size(); ++i) {
        if (field.getUnit(i)->getId() == id) return static_cast<int>(i);
    }
    return -1;
}


void scenario11() {
    std::cout << "\n Scenario 1.1: Creating field and populating via factories \n";

    GameField field(10, 10, 5);
    DamageFactory dmgFactory;
    TankFactory   tankFactory;
    FlyingFactory flyFactory;

    int nextId = 1;
    field.populateField(dmgFactory, "Mage", nextId++, 1, 1);
    field.populateField(tankFactory, "Paladin", nextId++, 4, 2);
    field.populateField(flyFactory, "Pegas", nextId++, 7, 4);
    field.populateField(dmgFactory, "Berserk", nextId++, 2, 5);

    std::cout << "\nUnits on field: " << field.size() << "\n";
    std::cout << "Field visualization:\n";
    field.printField();
}

void scenario12() {
    std::cout << "\n Scenario 1.2: Deep copy of the field \n";

    GameField field(10, 10, 5);
    DamageFactory dmgFactory;
    TankFactory   tankFactory;
    FlyingFactory flyFactory;

    int nextId = 1;
    field.populateField(dmgFactory, "Mage", nextId++, 1, 1);
    field.populateField(tankFactory, "Paladin", nextId++, 4, 2);
    field.populateField(flyFactory, "Pegas", nextId++, 7, 4);

    GameField copyField = field;

    field.getUnit(0)->move(5, 5);

    std::cout << "First unit coords in original: ("
        << field.getUnit(0)->getX() << ", " << field.getUnit(0)->getY() << ")\n";
    std::cout << "First unit coords in copy: ("
        << copyField.getUnit(0)->getX() << ", " << copyField.getUnit(0)->getY() << ")\n";
}

void scenario13() {
    std::cout << "\n Scenario 1.3: Iteration over the field \n";

    GameField field(10, 10, 5);
    DamageFactory dmgFactory;
    TankFactory   tankFactory;
    FlyingFactory flyFactory;

    int nextId = 1;
    field.populateField(dmgFactory, "Mage", nextId++, 1, 1);
    field.populateField(tankFactory, "Paladin", nextId++, 4, 2);
    field.populateField(flyFactory, "Pegas", nextId++, 7, 4);
    field.populateField(dmgFactory, "Berserk", nextId++, 2, 5);

    for (auto it = field.begin(); it != field.end(); ++it) {
        Unit* u = *it;
        std::cout << "Found unit: " << u->getType()
            << " at position (" << u->getX() << ", " << u->getY() << ")\n";
    }
}


void scenario21() {
    std::cout << "\n Scenario 2.1: Base and Units creation (Observer) \n";

    AllUnitsFactory factory;
    Base myBase(5, 5, 1000, 5, 10, 500, &factory);

    myBase.createUnit("Mage", 1, 1);
    myBase.createUnit("Paladin", 2, 2);
}

void scenario22() {
    std::cout << "\n Scenario 2.2: Movement and Proxy \n";

    AllUnitsFactory factory;
    Base myBase(5, 5, 1000, 5, 10, 500, &factory);
    myBase.createUnit("Mage", 1, 1);

    GameField newField;
    newField.initTerrain(10, 10);

    srand(42);
    for (int y = 0; y < 10; ++y) {
        for (int x = 0; x < 10; ++x) {
            int r = rand() % 100;
            if (y == 1 && x >= 0 && x <= 5) {
                newField.setTerrain(x, y, std::make_shared<Plains>());
                continue;
            }
            if (r < 25)      newField.setTerrain(x, y, std::make_shared<Forest>());
            else if (r < 40) newField.setTerrain(x, y, std::make_shared<Mountains>());
            else if (r < 55) newField.setTerrain(x, y, std::make_shared<Swamp>());
            else             newField.setTerrain(x, y, std::make_shared<Plains>());
        }
    }
    newField.setTerrain(3, 1, std::make_shared<Forest>());
    newField.setTerrain(4, 1, std::make_shared<Mountains>());

    Unit* hero = myBase.getUnit(0);
    std::string heroType = hero->getType();

    std::cout << "\nInitial map:\n";
    newField.printMap(hero->getX(), hero->getY(), heroType);

    std::cout << "\nHero starts moving from (1, 1) to the right...\n";
    for (int step = 1; step <= 4; ++step) {
        int newX = hero->getX() + 1;
        int newY = hero->getY();
        std::cout << "\nStep " << step << ". Target: (" << newX << ", " << newY << ").\n";

        TerrainProxy proxy = newField.getTerrainProxy(newX, newY);
        if (proxy.isPassable(*hero)) {
            proxy.getSpeedModifier(*hero);
            hero->move(newX, newY);
            std::cout << "Success! Hero is now at ("
                << hero->getX() << ", " << hero->getY() << ").\n";
            newField.printMap(hero->getX(), hero->getY(), heroType);
        }
        else {
            std::cout << "Path is blocked! Hero stays in place.\n";
            break;
        }
    }
}

void scenario23() {
    std::cout << "\n Scenario 2.3: Interaction with objects (Overload) \n";

    AllUnitsFactory factory;
    Base myBase(5, 5, 1000, 5, 10, 500, &factory);
    myBase.createUnit("Mage", 1, 1);

    Unit* hero = myBase.getUnit(0);

    TreasureChest chest;
    *hero + chest;      // GoldRewardStrategy

    Trap trap;
    *hero - trap;       // DamageStrategy

    Altar altar;
    *hero* altar;      // AttackBoostStrategy

    EnergySource source;
    *hero / source;     // HealthRestoreStrategy
}

void scenario24() {
    std::cout << "\n Scenario 2.4: Unit destruction and Observer \n";

    AllUnitsFactory factory;
    Base myBase(5, 5, 1000, 5, 10, 500, &factory);
    myBase.createUnit("Paladin", 1, 1);

    Unit* hero = myBase.getUnit(0);
    std::cout << "Dealing lethal damage to Hero...\n";
    hero->takeDamage(9999);
}


void runFreePlay() {
    clearScreen();
    std::cout << "\n FREE PLAY MODE \n";
    std::cout << "--- Map Setup ---\n";

    int w = 10, h = 10;
    bool validInput = false;

    do {
        std::cout << "Enter map width (10..50): ";
        if (!(std::cin >> w) || w < 10 || w > 50) {
            std::cin.clear(); std::cin.ignore(10000, '\n');
            std::cout << "Invalid width. Try again.\n";
        }
        else validInput = true;
    } while (!validInput);

    validInput = false;
    do {
        std::cout << "Enter map height (10..50): ";
        if (!(std::cin >> h) || h < 10 || h > 50) {
            std::cin.clear(); std::cin.ignore(10000, '\n');
            std::cout << "Invalid height. Try again.\n";
        }
        else validInput = true;
    } while (!validInput);

    AllUnitsFactory factory;
    GameField field(w, h, 50);

    srand(static_cast<unsigned>(time(nullptr)));
    for (int y = 0; y < field.getHeight(); ++y) {
        for (int x = 0; x < field.getWidth(); ++x) {
            int r = rand() % 100;
            if (r < 15)      field.setTerrain(x, y, std::make_shared<Forest>());
            else if (r < 25) field.setTerrain(x, y, std::make_shared<Mountains>());
            else if (r < 35) field.setTerrain(x, y, std::make_shared<Swamp>());


            int objChance = rand() % 100;
            if (objChance < 3)       field.addObject(x, y, std::make_unique<Altar>());
            else if (objChance < 6)  field.addObject(x, y, std::make_unique<EnergySource>());
            else if (objChance < 16) field.addObject(x, y, std::make_unique<Trap>());
            else if (objChance < 21) field.addObject(x, y, std::make_unique<TreasureChest>());
        }
    }

    int nextId = 1;
    int action = -1;

    while (action != 0) {
        clearScreen();
        field.printMap();

        std::cout << "\n--- FREE PLAY MENU ---\n";
        std::cout << "1. Add unit\n";
        std::cout << "2. Move unit (WASD)\n";
        std::cout << "3. Attack\n";
        std::cout << "4. List units\n";
        std::cout << "5. Show map\n";
        std::cout << "6. Build Base (1000 gold)\n";
        std::cout << "0. Back to main menu\n";
        std::cout << "Choice: ";
        std::cin >> action;

        if (std::cin.fail()) {
            std::cin.clear(); std::cin.ignore(10000, '\n');
            action = -1; continue;
        }

        switch (action) {
        case 1: {
            int typeChoice;
            std::cout << "\n--- Select unit type ---\n";
            std::cout << "1. Mage\n2. Berserk\n3. Paladin\n4. Vampir\n5. Pegas\n6. Dark Knight\n0. Cancel\nChoice: ";
            std::cin >> typeChoice;

            if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); break; }

            std::string type;
            switch (typeChoice) {
            case 1: type = "Mage"; break;
            case 2: type = "Berserk"; break;
            case 3: type = "Paladin"; break;
            case 4: type = "Vampir"; break;
            case 5: type = "Pegas"; break;
            case 6: type = "Dark Knight"; break;
            case 0: break;
            default: break;
            }
            if (type.empty()) break;

            int x, y;
            std::cout << "X and Y: "; std::cin >> x >> y;
            if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); break; }

            field.populateField(factory, type, nextId++, x, y);
            break;
        }
        case 2: {
            if (field.empty()) { break; }
            int id;
            std::cout << "Unit ID: "; std::cin >> id;
            int idx = findUnitIndexById(field, id);
            if (idx == -1) { std::cout << "Invalid ID!\n"; break; }

            Unit* u = field.getUnit(idx);
            char dir;
            std::cout << "Direction (W/A/S/D): "; std::cin >> dir;
            int nx = u->getX(), ny = u->getY();
            if (dir == 'w' || dir == 'W') --ny;
            else if (dir == 's' || dir == 'S') ++ny;
            else if (dir == 'a' || dir == 'A') --nx;
            else if (dir == 'd' || dir == 'D') ++nx;

            if (nx < 0 || nx >= field.getWidth() || ny < 0 || ny >= field.getHeight()) break;

            TerrainProxy proxy = field.getTerrainProxy(nx, ny);
            if (!proxy.isPassable(*u)) break;

            if (field.moveUnit(idx, nx, ny)) {
                NeutralObject* obj = field.getObjectAt(nx, ny);
                if (obj) {
                    if (Altar* altar = dynamic_cast<Altar*>(obj)) {
                        // Спец-эффект free play: x2 урон юнитам в 3x3
                        for (std::size_t i = 0; i < field.size(); ++i) {
                            Unit* other = field.getUnit(i);
                            if (std::abs(other->getX() - nx) <= 1 && std::abs(other->getY() - ny) <= 1) {
                                other->setDamageMultiplier(2);
                            }
                        }
                        std::cout << "Altar activated! Units in 3x3 get x2 damage.\n";
                        // Плюс общий эффект через стратегию (AttackBoostStrategy)
                        altar->interact(*u);
                    }
                    else {
                        obj->interact(*u);
                        if (dynamic_cast<Trap*>(obj) || dynamic_cast<TreasureChest*>(obj)) {
                            field.removeObject(nx, ny);
                        }
                    }
                }
            }
            break;
        }
        case 3: {
            if (field.empty()) { break; }
            int a_id, d_id;
            std::cout << "Attacker ID: "; std::cin >> a_id;
            std::cout << "Defender ID: "; std::cin >> d_id;

            int a_idx = findUnitIndexById(field, a_id);
            int d_idx = findUnitIndexById(field, d_id);

            if (a_idx == -1 || d_idx == -1 || a_idx == d_idx) break;

            Unit* atk = field.getUnit(a_idx);
            Unit* def = field.getUnit(d_idx);

            int dx = std::abs(atk->getX() - def->getX());
            int dy = std::abs(atk->getY() - def->getY());
            if (dx + dy != 1) break;

            atk->attackUnit(*def);
            if (def->getHealth().isZero()) {
                field.removeUnit(d_idx);
            }
            break;
        }
        case 4: {
            std::cout << "\nUnits on field (" << field.size() << "):\n";
            for (std::size_t i = 0; i < field.size(); ++i) {
                Unit* u = field.getUnit(i);
                std::cout << "ID: " << u->getId()
                    << " | " << u->getType()
                    << " at (" << u->getX() << ", " << u->getY() << ")"
                    << " HP=" << u->getHealth().getValue()
                    << " ATK=" << u->getAttack()
                    << " Gold=" << u->getGold() << "\n";
            }
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore(10000, '\n');
            std::cin.get();
            break;
        }
        case 5:
            break;
        case 6: {
            int id;
            std::cout << "Unit ID to build base: "; std::cin >> id;
            int idx = findUnitIndexById(field, id);
            if (idx == -1) { std::cout << "Invalid ID!\n"; break; }
            Unit* u = field.getUnit(idx);

            if (u->getGold() < 1000) {
                std::cout << "Not enough gold! Need 1000, have " << u->getGold() << ".\n";
                break;
            }

            char dir;
            std::cout << "Direction to build (W/A/S/D): "; std::cin >> dir;
            int nx = u->getX(), ny = u->getY();
            if (dir == 'w' || dir == 'W') --ny;
            else if (dir == 's' || dir == 'S') ++ny;
            else if (dir == 'a' || dir == 'A') --nx;
            else if (dir == 'd' || dir == 'D') ++nx;

            if (nx < 0 || nx >= field.getWidth() || ny < 0 || ny >= field.getHeight()) {
                std::cout << "Out of bounds!\n"; break;
            }
            if (field.getObjectAt(nx, ny)) {
                std::cout << "Cell already occupied!\n"; break;
            }

            bool unitHere = false;
            for (std::size_t i = 0; i < field.size(); ++i) {
                if (field.getUnit(i)->getX() == nx && field.getUnit(i)->getY() == ny) {
                    unitHere = true; break;
                }
            }
            if (unitHere) { std::cout << "Unit already there!\n"; break; }

            u->spendGold(1000);
            field.addObject(nx, ny, std::make_unique<Castle>());
            std::cout << "Base built at (" << nx << ", " << ny << ")!\n";
            break;
        }
        case 0:
            break;
        default:
            break;
        }
    }
}


int main() {
    setlocale(LC_ALL, "Russian");
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    int choice = -1;
    while (true) {
        clearScreen();
        std::cout << "\n    MAIN MENU (MMO RPG)\n";
        std::cout << "1. Lab 1 Scenarios\n";
        std::cout << "2. Lab 2 Scenarios\n";
        std::cout << "3. Free play\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear(); std::cin.ignore(10000, '\n'); continue;
        }

        if (choice == 1) {
            int subChoice = -1;
            while (subChoice != 0) {
                clearScreen();
                std::cout << "\n    LAB 1 SCENARIOS\n";
                std::cout << "1. Scenario 1.1\n2. Scenario 1.2\n3. Scenario 1.3\n0. Back\nChoice: ";
                std::cin >> subChoice;
                if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
                switch (subChoice) {
                case 1: scenario11(); waitForEnter(); break;
                case 2: scenario12(); waitForEnter(); break;
                case 3: scenario13(); waitForEnter(); break;
                case 0: break;
                default: waitForEnter(); break;
                }
            }
        }
        else if (choice == 2) {
            int subChoice = -1;
            while (subChoice != 0) {
                clearScreen();
                std::cout << "\n    LAB 2 SCENARIOS\n";
                std::cout << "1. Scenario 2.1\n2. Scenario 2.2\n3. Scenario 2.3\n4. Scenario 2.4\n0. Back\nChoice: ";
                std::cin >> subChoice;
                if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
                switch (subChoice) {
                case 1: scenario21(); waitForEnter(); break;
                case 2: scenario22(); waitForEnter(); break;
                case 3: scenario23(); waitForEnter(); break;
                case 4: scenario24(); waitForEnter(); break;
                case 0: break;
                default: waitForEnter(); break;
                }
            }
        }
        else if (choice == 3) {
            runFreePlay();
        }
        else if (choice == 0) {
            break;
        }
    }
    return 0;
}