#pragma once
#include "GameField.h"
#include "Base.h"
#include "UnitFactory.h"
#include "NeutralObject.h"
#include <iostream>
#include <cstdlib>
#include <memory>
#include <string>

inline void scenario21() {
    std::cout << "\n Scenario 2.1: Base and Units creation (Observer) \n";

    AllUnitsFactory factory;
    Base myBase(5, 5, 1000, 5, 10, 500, 200, 300, &factory);

    myBase.createUnit("Mage", 1, 1);
    myBase.createUnit("Paladin", 2, 2);
}

inline void scenario22() {
    std::cout << "\n Scenario 2.2: Movement and Proxy \n";

    AllUnitsFactory factory;
    Base myBase(5, 5, 1000, 5, 10, 500, 200, 300, &factory);
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

inline void scenario23() {
    std::cout << "\n Scenario 2.3: Interaction with objects (Overload) \n";

    AllUnitsFactory factory;
    Base myBase(5, 5, 1000, 5, 10, 500, 200, 300, &factory);
    myBase.createUnit("Mage", 1, 1);

    Unit* hero = myBase.getUnit(0);

    TreasureChest chest;
    *hero + chest;

    Trap trap;
    *hero - trap;

    Altar altar;
    *hero* altar;

    EnergySource source;
    *hero / source;
}

inline void scenario24() {
    std::cout << "\n Scenario 2.4: Unit destruction and Observer \n";

    AllUnitsFactory factory;
    Base myBase(5, 5, 1000, 5, 10, 500, 200, 300, &factory);
    myBase.createUnit("Paladin", 1, 1);

    Unit* hero = myBase.getUnit(0);
    std::cout << "Dealing lethal damage to Hero...\n";
    hero->takeDamage(9999);
}