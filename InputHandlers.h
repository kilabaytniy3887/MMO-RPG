#pragma once
#include "Commands.h"
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <memory>

class InputHandler {
protected:
    std::unique_ptr<InputHandler> nextHandler = nullptr;
public:
    virtual ~InputHandler() = default;
    void setNext(std::unique_ptr<InputHandler> next) { nextHandler = std::move(next); }

    virtual bool handle(const std::string& input, GameMediator* med) {
        if (nextHandler) return nextHandler->handle(input, med);
        return false;
    }
};

class ScenarioCommandHandler : public InputHandler {
public:
    bool handle(const std::string& input, GameMediator* med) override {
        std::string cleanInput = input;
        std::replace(cleanInput.begin(), cleanInput.end(), ',', ' ');

        std::stringstream ss(cleanInput);
        std::string word1, word2;
        ss >> word1 >> word2;

        if (word1 == "create" && word2 == "unit") {
            std::string type, at; int x, y;
            ss >> type >> at >> x >> y;
            if (at == "at") {
                med->executeCommand(std::make_unique<ProduceUnitCommand>(med, type, x, y));
                return true;
            }
        }

        if (word1 == "stats") {
            med->executeCommand(std::make_unique<StatusCommand>(med));
            return true;
        }

        if (word1 == "move" && word2 == "unit") {
            int id; std::string to; int x, y;
            ss >> id >> to >> x >> y;
            if (to == "to") {
                med->executeCommand(std::make_unique<MoveCommand>(med, id, x, y));
                return true;
            }
        }

        if (word1 == "attack" && word2 == "unit") {
            int id1; std::string target; int id2;
            ss >> id1 >> target >> id2;
            if (target == "target") {
                med->executeCommand(std::make_unique<AttackCommand>(med, id1, id2));
                return true;
            }
        }

        if (word1 == "attack" && word2 == "building") {
            int id1; std::string target; int bId;
            ss >> id1 >> target >> bId;
            if (target == "target") {
                med->executeCommand(std::make_unique<AttackBuildingCommand>(med, id1, bId));
                return true;
            }
        }

        if (word1 == "build") {
            std::string type, at; int x, y;
            ss >> type >> at >> x >> y;
            if (at == "at") {
                med->executeCommand(std::make_unique<BuildCommand>(med, type, x, y));
                return true;
            }
        }

        if (word1 == "research") {
            std::string tech;
            ss >> tech;
            if (tech == "attack_upgrade") tech = "DamageUp";
            med->executeCommand(std::make_unique<ResearchCommand>(med, tech));
            return true;
        }

        if (word1 == "produce") {
            std::string type, in; int bId;
            ss >> type >> in >> bId;
            if (in == "in") {
                auto coords = med->getBuildingCoords(bId);
                if (coords.first != -1) {
                    med->executeCommand(std::make_unique<ProduceUnitCommand>(med, type, coords.first, coords.second));
                    return true;
                }
                else {
                    std::cout << "[Handler] Building with ID " << bId << " not found!\n";
                    return true;
                }
            }
        }

        if (word1 == "collect" && word2 == "unit") {
            int id; std::string from; int targetId;
            ss >> id >> from >> targetId;
            med->executeCommand(std::make_unique<GatherCommand>(med, id));
            return true;
        }

        if (word1 == "distribute") {
            std::string res, to, target; int amount;
            ss >> res >> to >> target >> amount;
            med->executeCommand(std::make_unique<AllocateCommand>(med, res, amount));
            return true;
        }

        return InputHandler::handle(input, med);
    }
};

class BaseCommandHandler : public InputHandler {
public:
    bool handle(const std::string& input, GameMediator* med) override {
        std::stringstream ss(input);
        std::string cmd; ss >> cmd;

        if (cmd == "build") {
            std::string type; int x, y;
            if (ss >> type >> x >> y) { 
                med->executeCommand(std::make_unique<BuildCommand>(med, type, x, y)); return true;
            }
        }
        else if (cmd == "produce") {
            std::string type; int x, y;
            if (ss >> type >> x >> y) { 
                med->executeCommand(std::make_unique<ProduceUnitCommand>(med, type, x, y)); return true; 
            }
        }
        else if (cmd == "research") {
            std::string tech; if (ss >> tech) { 
                med->executeCommand(std::make_unique<ResearchCommand>(med, tech)); return true; 
            }
        }
        else if (cmd == "allocate") {
            std::string res; int amount; if (ss >> res >> amount) { 
                med->executeCommand(std::make_unique<AllocateCommand>(med, res, amount)); return true; 
            }
        }
        return InputHandler::handle(input, med);
    }
};

class UnitCommandHandler : public InputHandler {
public:
    bool handle(const std::string& input, GameMediator* med) override {
        std::stringstream ss(input);
        std::string cmd; ss >> cmd;

        if (cmd == "move") {
            int id, x, y; if (ss >> id >> x >> y) {
                med->executeCommand(std::make_unique<MoveCommand>(med, id, x, y)); return true;
            }
        }
        else if (cmd == "attack") {
            int atk, def; if (ss >> atk >> def) { 
                med->executeCommand(std::make_unique<AttackCommand>(med, atk, def)); return true;
            }
        }
        else if (cmd == "attack_building") {
            int atk, bId; if (ss >> atk >> bId) { 
                med->executeCommand(std::make_unique<AttackBuildingCommand>(med, atk, bId)); return true;
            }
        }
        else if (cmd == "gather") {
            int id; if (ss >> id) { 
                med->executeCommand(std::make_unique<GatherCommand>(med, id)); return true; 
            }
        }
        else if (cmd == "heal") {
            int id; if (ss >> id) {
                med->executeCommand(std::make_unique<HealCommand>(med, id)); return true; 
            }
        }
        return InputHandler::handle(input, med);
    }
};

class SystemCommandHandler : public InputHandler {
public:
    bool handle(const std::string& input, GameMediator* med) override {
        if (input == "status") { med->executeCommand(std::make_unique<StatusCommand>(med)); return true; }
        if (input == "undo") { med->executeCommand(std::make_unique<UndoCommand>(med)); return true; }
        return InputHandler::handle(input, med);
    }
};
