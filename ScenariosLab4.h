#pragma once
#include "GameFacade.h"
#include <iostream>
#include <memory>

inline void scenario41() {
    std::cout << "\n Scenario 4.1: Logging to console text \n";
    AllUnitsFactory factory;
    Base base(2, 2, 5000, 20, 10, 1500, 800, 1200, &factory);
    GameField field(20, 20, 50);
    GameFacade facade(&base, &field, &factory);

    facade.executeUserInput("set_logger console");
    facade.executeUserInput("set_format text");
    facade.executeUserInput("create unit Berserk at 5,5");
    facade.executeUserInput("create unit Mage at 5,6");
    facade.executeUserInput("move unit 1 to 6,5");
    facade.executeUserInput("attack unit 1 target 2");
}

inline void scenario42() {
    std::cout << "\n Scenario 4.2: Logging to file JSON \n";
    AllUnitsFactory factory;
    Base base(2, 2, 5000, 20, 10, 1500, 800, 1200, &factory);
    GameField field(20, 20, 50);
    GameFacade facade(&base, &field, &factory);

    facade.executeUserInput("set_logger file game_log.json");
    facade.executeUserInput("set_format json");
    facade.executeUserInput("build Barracks at 3,3");
    std::cout << "[Scenario] Log written to game_log.json\n";
}

inline void scenario43() {
    std::cout << "\n Scenario 4.3: Disabling and re-enabling logging \n";
    AllUnitsFactory factory;
    Base base(2, 2, 5000, 20, 10, 1500, 800, 1200, &factory);
    GameField field(20, 20, 50);
    GameFacade facade(&base, &field, &factory);

    facade.executeUserInput("set_logger console");
    facade.executeUserInput("set_format text");
    facade.executeUserInput("create unit Berserk at 5,5");
    facade.executeUserInput("damage unit 1 20");
    facade.executeUserInput("set_logger null");
    facade.executeUserInput("create unit Paladin at 6,5");
    facade.executeUserInput("set_logger console");
    facade.executeUserInput("stats");
}

inline void scenario44() {
    std::cout << "\n Scenario 4.4: Switching formats on the fly \n";
    AllUnitsFactory factory;
    Base base(2, 2, 5000, 20, 10, 1500, 800, 1200, &factory);
    GameField field(20, 20, 50);
    field.setTerrain(5, 5, std::make_shared<Forest>());
    GameFacade facade(&base, &field, &factory);

    facade.executeUserInput("create unit Kobold at 5,5");
    facade.executeUserInput("set_logger console");
    facade.executeUserInput("set_format csv");
    facade.executeUserInput("gather unit 1");
    facade.executeUserInput("set_format text");
    facade.executeUserInput("gather unit 1");
}