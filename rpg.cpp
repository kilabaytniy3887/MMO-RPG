#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <cmath>
#include <memory>
#include <string>
#include <limits>
#include "GameField.h"
#include "Base.h"
#include "UnitFactory.h"
#include "ScenariosLab1.h"
#include "ScenariosLab2.h"
#include "ScenariosLab3.h"
#include "GameFacade.h" 

void clearScreen() {
    std::cout << "\033[2J\033[1;1H" << std::flush;
}

void waitForEnter() {
    std::cin.ignore(10000, '\n');
    std::cout << "\nPress Enter to continue..." << std::flush;
    std::cin.get();
}

void runFreePlay() {
    clearScreen();
    std::cout << "\n=== FREE PLAY MODE (Lab 3 Architecture) ===\n";

    int w = 20, h = 20;
    int baseX = 2, baseY = 2;

    int testChoice = 0;
    std::cout << "Use test map configuration? (20x20, base at 2 2)\n";
    std::cout << "1. Yes\n";
    std::cout << "0. No (custom setup)\n";
    std::cout << "Choice: ";
    std::cin >> testChoice;

    if (testChoice == 1) {
        std::cout << "\nTest map selected: 20x20, base at (2, 2).\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(10000, '\n'); std::cin.get();
    }
    else {
        bool validInput = false;
        do {
            std::cout << "Enter map width (10..50): ";
            if (!(std::cin >> w) || w < 10 || w > 50) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
            }
            else validInput = true;
        } while (!validInput);

        validInput = false;
        do {
            std::cout << "Enter map height (10..50): ";
            if (!(std::cin >> h) || h < 10 || h > 50) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
            }
            else validInput = true;
        } while (!validInput);

        validInput = false;
        do {
            std::cout << "Choose base position (X Y), X in [2.." << (w - 3) << "], Y in [2.." << (h - 3) << "]: ";
            if (!(std::cin >> baseX >> baseY)) {
                std::cin.clear(); std::cin.ignore(10000, '\n');
                continue;
            }
            if (baseX < 2 || baseX > w - 3 || baseY < 2 || baseY > h - 3) continue;
            validInput = true;
        } while (!validInput);
    }

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

    for (int dy = -2; dy <= 2; ++dy) {
        for (int dx = -2; dx <= 2; ++dx) {
            int nx = baseX + dx, ny = baseY + dy;
            if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
            field.setTerrain(nx, ny, std::make_shared<Plains>());
            field.removeObject(nx, ny);
        }
    }

    std::unique_ptr<Base> playerBase(new Base(baseX, baseY, 5000, 20, 10, 1500, 800, 1200, &factory));
    field.addObject(baseX, baseY, std::make_unique<Castle>());

    GameFacade facade(playerBase.get(), &field, &factory);

    int mainChoice = -1;
    while (mainChoice != 0) {
        clearScreen();
        facade.printMap(playerBase->getX(), playerBase->getY(), "Base");

        std::cout << "\n----------------------------------------\n";
        std::cout << "TURN: " << facade.getCurrentTurn() << " | Base at (" << playerBase->getX() << "," << playerBase->getY() << ")\n";
        std::cout << "=== FREE PLAY MENU ===\n";
        std::cout << "1. Unit Management\n";
        std::cout << "2. Base Management\n";
        std::cout << "3. View Game State\n";
        std::cout << "4. End Turn (Next Turn)\n";
        std::cout << "0. Exit to main menu\nChoice: ";

        if (!(std::cin >> mainChoice)) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
        if (mainChoice == 0) break;

        switch (mainChoice) {
        case 1: {
            int subChoice = -1;
            while (subChoice != 0) {
                clearScreen();
                facade.printMap(playerBase->getX(), playerBase->getY(), "Base");
                std::cout << "\n----------------------------------------\n";
                std::cout << "=== UNIT MANAGEMENT (Turn " << facade.getCurrentTurn() << ") ===\n";
                std::cout << "1. Create unit\n2. Move unit\n3. Attack\n4. Gather resource\n5. Heal unit\n6. Check all units\n0. Back\nChoice: ";
                std::cin >> subChoice;
                if (subChoice == 0) break;

                std::string cmd = "", type; int id1, id2, x, y;
                switch (subChoice) {
                case 1: {
                    int typeChoice;
                    std::cout << "\n--- Select unit type ---\n";
                    std::cout << "1. Mage\n2. Berserk\n3. Paladin\n4. Vampir\n5. Pegas\n6. Dark Knight\n";
                    std::cout << "7. Archangel (Requires Lab Lvl 4)\n8. Titan (Requires Lab Lvl 5)\n";
                    std::cout << "9. Kobold (3x Ore)\n10. Beaver Mutant (3x Wood)\n";
                    std::cout << "0. Cancel\nChoice: ";
                    std::cin >> typeChoice;

                    switch (typeChoice) {
                    case 1: type = "Mage"; break;
                    case 2: type = "Berserk"; break;
                    case 3: type = "Paladin"; break;
                    case 4: type = "Vampir"; break;
                    case 5: type = "Pegas"; break;
                    case 6: type = "DarkKnight"; break;
                    case 7: type = "Archangel"; break;
                    case 8: type = "Titan"; break;
                    case 9: type = "Kobold"; break;
                    case 10: type = "BeaverMutant"; break;
                    case 0: break;
                    default: break;
                    }
                    if (type.empty()) break;

                    std::cout << "X Y (Must be near Barracks or Base): "; std::cin >> x >> y;
                    cmd = "produce " + type + " " + std::to_string(x) + " " + std::to_string(y);
                    break;
                }
                case 2: {
                    std::cout << "Unit ID: "; std::cin >> id1;

                    int cx = facade.getUnitX(id1);
                    int cy = facade.getUnitY(id1);
                    if (cx == -1) {
                        std::cout << "Unit not found!\n";
                        waitForEnter();
                        break;
                    }

                    bool moving = true;
                    while (moving) {
                        clearScreen();
                        facade.printMap(cx, cy, "Unit");
                        std::cout << "\n----------------------------------------\n";
                        std::cout << "=== MOVING UNIT " << id1 << " ===\n";
                        std::cout << "Position: (" << cx << ", " << cy << ") | Moves left: "
                            << facade.getUnitMovesLeft(id1) << "/" << facade.getUnitMaxMoves(id1) << "\n";
                        std::cout << "Use W/A/S/D to move, Q to stop.\n";
                        std::cout << "Direction: ";

                        char dir;
                        std::cin >> dir;
                        if (dir == 'q' || dir == 'Q' || dir == '0') {
                            moving = false;
                            break;
                        }

                        if (facade.getUnitMovesLeft(id1) <= 0) {
                            std::cout << "\nNo movement points left for this unit! Press Q to go back or End Turn.\n";
                            waitForEnter();
                            continue;
                        }

                        int nx = cx, ny = cy;
                        if (dir == 'w' || dir == 'W') --ny;
                        else if (dir == 's' || dir == 'S') ++ny;
                        else if (dir == 'a' || dir == 'A') --nx;
                        else if (dir == 'd' || dir == 'D') ++nx;
                        else continue;

                        std::string moveCmd = "move " + std::to_string(id1) + " " + std::to_string(nx) + " " + std::to_string(ny);
                        facade.executeUserInput(moveCmd);

                        cx = facade.getUnitX(id1);
                        cy = facade.getUnitY(id1);

                        if (facade.getUnitMovesLeft(id1) <= 0) {
                            std::cout << "Unit spent all movement points for this turn.\n";
                            waitForEnter();
                            moving = false;
                        }
                    }
                    break;
                }
                case 3:
                    int targetType;
                    std::cout << "Attacker Unit ID: "; std::cin >> id1;
                    std::cout << "Select target type (1. Unit, 2. Building): "; std::cin >> targetType;
                    if (targetType == 1) {
                        std::cout << "Defender Unit ID: "; std::cin >> id2;
                        cmd = "attack " + std::to_string(id1) + " " + std::to_string(id2);
                    }
                    else if (targetType == 2) {
                        std::cout << "Target Building ID: "; std::cin >> id2;
                        cmd = "attack_building " + std::to_string(id1) + " " + std::to_string(id2);
                    }
                    break;
                case 4:
                    std::cout << "Unit ID: "; std::cin >> id1;
                    cmd = "gather " + std::to_string(id1);
                    break;
                case 5:
                    std::cout << "Unit ID: "; std::cin >> id1;
                    cmd = "heal " + std::to_string(id1);
                    break;
                case 6:
                    cmd = "status";
                    break;
}

if (!cmd.empty()) {
    facade.executeUserInput(cmd);
    waitForEnter();
}
}
break;
}
case 2: {
    int subChoice = -1;
    while (subChoice != 0) {
        clearScreen();
        facade.printMap(playerBase->getX(), playerBase->getY(), "Base");
        std::cout << "\n----------------------------------------\n";
        std::cout << "=== BASE MANAGEMENT (Turn " << facade.getCurrentTurn() << ") ===\n";
        std::cout << "1. Build building\n2. Research technology (Requires Lab)\n3. Distribute resources\n4. View Base Stats\n0. Back\nChoice: ";
        std::cin >> subChoice;
        if (subChoice == 0) break;

        std::string cmd = "", type; int x, y, amt;
        switch (subChoice) {
        case 1: {
            int bTypeChoice;
            std::cout << "\n--- Select building type ---\n";
            std::cout << "1. Barracks (100 Wood, 50 Metal)\n";
            std::cout << "2. Hospital (150 Wood, 100 Metal)\n";
            std::cout << "3. Lab (200 Wood, 150 Metal)\n";
            std::cout << "4. PowerPlant (250 Wood, 200 Metal)\n";
            std::cout << "5. Warehouse (120 Wood, 80 Metal)\n";
            std::cout << "0. Cancel\nChoice: ";
            std::cin >> bTypeChoice;
            switch (bTypeChoice) {
            case 1: type = "Barracks"; break;
            case 2: type = "Hospital"; break;
            case 3: type = "Lab"; break;
            case 4: type = "PowerPlant"; break;
            case 5: type = "Warehouse"; break;
            case 0: break;
            }
            if (type.empty()) break;
            std::cout << "X Y (Must be a free cell): "; std::cin >> x >> y;
            cmd = "build " + type + " " + std::to_string(x) + " " + std::to_string(y);
            break;
        }
        case 2: {
            int rTypeChoice;
            std::cout << "\n--- Select research (Takes 2 turns) ---\n";
            std::cout << "1. Armor Up (+20 Armor, Lab Lvl 1)\n";
            std::cout << "2. Damage Up (+30 Damage, Lab Lvl 2)\n";
            std::cout << "3. Speed Up (+1 Speed, Lab Lvl 3)\n";
            std::cout << "0. Cancel\nChoice: ";
            std::cin >> rTypeChoice;
            switch (rTypeChoice) {
            case 1: type = "ArmorUp"; break;
            case 2: type = "DamageUp"; break;
            case 3: type = "SpeedUp"; break;
            case 0: break;
            }
            if (type.empty()) break;
            cmd = "research " + type;
            break;
        }
        case 3: {
            int distChoice;
            std::cout << "\n--- Distribute resources ---\n";
            std::cout << "1. Convert Wood to Energy (Requires PowerPlant)\n";
            std::cout << "2. Exchange Metal to Wood (Requires Warehouse)\n";
            std::cout << "0. Cancel\nChoice: ";
            std::cin >> distChoice;
            if (distChoice == 1) {
                std::cout << "Wood amount: "; std::cin >> amt;
                cmd = "allocate wood " + std::to_string(amt);
            }
            else if (distChoice == 2) {
                std::cout << "Metal amount: "; std::cin >> amt;
                cmd = "allocate ore " + std::to_string(amt);
            }
            break;
        }
        case 4:
            cmd = "status";
            break;
        }
        if (!cmd.empty()) {
            facade.executeUserInput(cmd);
            waitForEnter();
        }
    }
    break;
}
case 3: {
    clearScreen();
    facade.printMap(playerBase->getX(), playerBase->getY(), "Base");
    std::cout << "\n----------------------------------------\n";
    std::cout << "=== GAME STATE ===\n";
    facade.executeUserInput("status");
    waitForEnter();
    break;
}
case 4: {
    clearScreen();
    facade.printMap(playerBase->getX(), playerBase->getY(), "Base");
    std::cout << "\n----------------------------------------\n";
    std::cout << "=== ENDING TURN " << facade.getCurrentTurn() << " ===\n";
    facade.updateTime();
    std::cout << "Turn " << facade.getCurrentTurn() << " started! Unit moves restored, energy and buildings updated.\n";
    waitForEnter();
    break;
}
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
        std::cout << "1. Lab 1 Scenarios\n2. Lab 2 Scenarios\n3. Lab 3 Scenarios (Patterns)\n4. Free play\n0. Exit\nChoose an option: ";
        std::cin >> choice;
        if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
        if (choice == 1) {
            int subChoice = -1;
            while (subChoice != 0) {
                clearScreen();
                std::cout << "\n    LAB 1 SCENARIOS\n1. Scenario 1.1\n2. Scenario 1.2\n3. Scenario 1.3\n0. Back\nChoice: ";
                std::cin >> subChoice;
                if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
                switch (subChoice) {
                case 1: scenario11(); waitForEnter(); break;
                case 2: scenario12(); waitForEnter(); break;
                case 3: scenario13(); waitForEnter(); break;
                case 0: break;
                }
            }
        }
        else if (choice == 2) {
            int subChoice = -1;
            while (subChoice != 0) {
                clearScreen();
                std::cout << "\n    LAB 2 SCENARIOS\n1. Scenario 2.1\n2. Scenario 2.2\n3. Scenario 2.3\n4. Scenario 2.4\n0. Back\nChoice: ";
                std::cin >> subChoice;
                if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
                switch (subChoice) {
                case 1: scenario21(); waitForEnter(); break;
                case 2: scenario22(); waitForEnter(); break;
                case 3: scenario23(); waitForEnter(); break;
                case 4: scenario24(); waitForEnter(); break;
                case 0: break;
                }
            }
        }
        else if (choice == 3) {
            int subChoice = -1;
            while (subChoice != 0) {
                clearScreen();
                std::cout << "\n    LAB 3 SCENARIOS\n";
                std::cout << "1. Scenario 3.1 (Creation and Stats)\n";
                std::cout << "2. Scenario 3.2 (Movement and Attack)\n";
                std::cout << "3. Scenario 3.3 (Building and Research)\n";
                std::cout << "4. Scenario 3.4 (Resource Management)\n";
                std::cout << "0. Back\nChoice: ";
                std::cin >> subChoice;
                if (std::cin.fail()) { std::cin.clear(); std::cin.ignore(10000, '\n'); continue; }
                switch (subChoice) {
                case 1: scenario31(); waitForEnter(); break;
                case 2: scenario32(); waitForEnter(); break;
                case 3: scenario33(); waitForEnter(); break;
                case 4: scenario34(); waitForEnter(); break;
                case 0: break;
                }
            }
        }
        else if (choice == 4) {
            runFreePlay();
        }
        else if (choice == 0) break;
    }
    return 0;
}