#pragma once
#include "GameFacade.h"
#include <iostream>
#include <memory>

inline void scenario31() {
    std::cout << "\n Scenario 3.1: Creation and Stats \n";
    std::cout << "Commands: create unit infantry at 5,5 | stats\n\n";

    AllUnitsFactory factory;
    Base base(2, 2, 5000, 20, 10, 1500, 800, 1200, &factory);
    GameField field(20, 20, 50);
    GameFacade facade(&base, &field, &factory);

    facade.executeUserInput("create unit infantry at 5,5");
    facade.executeUserInput("stats");
}

inline void scenario32() {
    std::cout << "\n Scenario 3.2: Movement and Attack \n";
    std::cout << "Commands: move unit 1 to 10,10 | attack unit 1 target 2 | stats units\n\n";

    AllUnitsFactory factory;
    Base base(2, 2, 5000, 20, 10, 1500, 800, 1200, &factory);
    GameField field(20, 20, 50);
    GameFacade facade(&base, &field, &factory);

    facade.executeUserInput("create unit infantry at 5,5");
    facade.executeUserInput("create unit berserk at 5,6");

    std::cout << "\n--- Executing commands ---\n";
    facade.executeUserInput("move unit 1 to 10,10");
    facade.executeUserInput("attack unit 1 target 2");
    facade.executeUserInput("stats units");
}

inline void scenario33() {
    std::cout << "\n Scenario 3.3: Building and Research \n";
    std::cout << "Commands: build barracks at 3,3 | research attack_upgrade | produce infantry in 1\n\n";

    AllUnitsFactory factory;
    Base base(2, 2, 5000, 20, 10, 1500, 800, 1200, &factory);
    GameField field(20, 20, 50);
    GameFacade facade(&base, &field, &factory);

    facade.executeUserInput("build barracks at 3,3");

    std::cout << "\n[Skipping 2 turns for construction...]\n";
    facade.updateTime();
    facade.updateTime();


    facade.executeUserInput("research attack_upgrade");

    std::cout << "\n[Producing unit in Barracks ID 1...]\n";
    facade.executeUserInput("produce infantry in 1");
}

inline void scenario34() {
    std::cout << "\n Scenario 3.4: Resource Management \n";
    std::cout << "Commands: collect unit 1 from 1 | distribute wood to barracks 50\n\n";

    AllUnitsFactory factory;
    Base base(2, 2, 5000, 20, 10, 1500, 800, 1200, &factory);
    GameField field(20, 20, 50);

    field.setTerrain(5, 5, std::make_shared<Forest>());
    GameFacade facade(&base, &field, &factory);

    facade.executeUserInput("create unit infantry at 5,5");

    std::cout << "\n--- Executing commands ---\n";
    facade.executeUserInput("collect unit 1 from 1");
    facade.executeUserInput("distribute wood to barracks 50");
}