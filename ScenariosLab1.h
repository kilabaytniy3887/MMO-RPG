#pragma once
#include "GameField.h"
#include "UnitFactory.h"
#include <iostream>

inline void scenario11() {
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

inline void scenario12() {
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

inline void scenario13() {
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