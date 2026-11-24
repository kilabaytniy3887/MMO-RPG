#pragma once
#include "InputHandlers.h"
#include "GameMediator.h"
#include "GameField.h"
#include "Logger.h"
#include <string>
#include <memory>

class GameFacade {
private:
    std::unique_ptr<GameMediator> mediator;
    std::unique_ptr<InputHandler> chain;
    GameField* field;
    std::shared_ptr<LoggingProxy> logger;

public:
    GameFacade(Base* base, GameField* f, AllUnitsFactory* factory)
        : logger(std::make_shared<LoggingProxy>()), field(f) {

        mediator = std::make_unique<GameMediator>(base, f, factory, logger.get());

        auto scenarioHandler = std::make_unique<ScenarioCommandHandler>();
        auto baseHandler = std::make_unique<BaseCommandHandler>();
        auto unitHandler = std::make_unique<UnitCommandHandler>();
        auto systemHandler = std::make_unique<SystemCommandHandler>();

        unitHandler->setNext(std::move(systemHandler));
        baseHandler->setNext(std::move(unitHandler));
        scenarioHandler->setNext(std::move(baseHandler));
        chain = std::move(scenarioHandler);
    }

    ~GameFacade() = default;

    std::shared_ptr<LoggingProxy> getLogger() { return logger; }

    void executeUserInput(const std::string& input) {
        std::cout << "\n> " << input << "\n";
        if (logger) logger->log("INFO", "USER", input);
        if (!chain->handle(input, mediator.get())) {
            std::cout << "[Facade] Unknown command or invalid arguments.\n";
        }
    }

    void updateTime() { mediator->update(); }
    void printMap(int heroX = -1, int heroY = -1, const std::string& heroType = "") {
        field->printMap(heroX, heroY, heroType, mediator->getBuildingsForMap());
    }
    int getUnitX(int id) { return mediator->getUnitX(id); }
    int getUnitY(int id) { return mediator->getUnitY(id); }
    int getUnitMovesLeft(int id) { return mediator->getUnitMovesLeft(id); }
    int getUnitMaxMoves(int id) { return mediator->getUnitMaxMoves(id); }
    int getCurrentTurn() { return mediator->getCurrentTurn(); }
};