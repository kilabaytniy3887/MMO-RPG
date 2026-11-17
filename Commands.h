#pragma once
#include "GameMediator.h"

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class ProduceUnitCommand : public ICommand {
    GameMediator* med; std::string type; int x, y; int createdId = -1;
public:
    ProduceUnitCommand(GameMediator* m, std::string t, int x, int y) : med(m), type(t), x(x), y(y) {}
    void execute() override { createdId = med->produceUnit(type, x, y); }
    void undo() override { if (createdId != -1) med->removeUnit(createdId); }
};

class MoveCommand : public ICommand {
    GameMediator* med; int id, x, y; int oldX, oldY;
public:
    MoveCommand(GameMediator* m, int id, int x, int y) : med(m), id(id), x(x), y(y) {}
    void execute() override {
        oldX = med->getUnitX(id); oldY = med->getUnitY(id);
        med->moveUnit(id, x, y);
    }
    void undo() override { med->undoMoveUnit(id, oldX, oldY); }
};

class AttackCommand : public ICommand {
    GameMediator* med; int atkId, defId;
public:
    AttackCommand(GameMediator* m, int a, int d) : med(m), atkId(a), defId(d) {}
    void execute() override { med->attack(atkId, defId); }
    void undo() override { std::cout << "[Undo] Cannot undo damage taken! (Combat is permanent)\n"; }
};

class AttackBuildingCommand : public ICommand {
    GameMediator* med; int atkId, buildingId;
public:
    AttackBuildingCommand(GameMediator* m, int a, int b) : med(m), atkId(a), buildingId(b) {}
    void execute() override { med->attackBuilding(atkId, buildingId); }
    void undo() override { std::cout << "[Undo] Cannot undo building damage!\n"; }
};

class GatherCommand : public ICommand {
    GameMediator* med; int id;
public:
    GatherCommand(GameMediator* m, int id) : med(m), id(id) {}
    void execute() override { med->gatherResources(id); }
    void undo() override { std::cout << "[Undo] Resources already spent. Cannot undo gathering.\n"; }
};

class HealCommand : public ICommand {
    GameMediator* med; int id;
public:
    HealCommand(GameMediator* m, int id) : med(m), id(id) {}
    void execute() override { med->healUnit(id); }
    void undo() override { std::cout << "[Undo] Cannot undo healing!\n"; }
};

class BuildCommand : public ICommand {
    GameMediator* med; std::string type; int x, y; int bId = -1;
public:
    BuildCommand(GameMediator* m, std::string t, int x, int y) : med(m), type(t), x(x), y(y) {}
    void execute() override { bId = med->buildBuilding(type, x, y); }
    void undo() override { if (bId != -1) med->removeBuilding(bId); }
};

class ResearchCommand : public ICommand {
    GameMediator* med; std::string tech;
public:
    ResearchCommand(GameMediator* m, std::string t) : med(m), tech(t) {}
    void execute() override { med->researchTech(tech); }
    void undo() override { std::cout << "[Undo] Research cannot be aborted once started!\n"; }
};

class AllocateCommand : public ICommand {
    GameMediator* med; std::string res; int amount;
public:
    AllocateCommand(GameMediator* m, std::string r, int a) : med(m), res(r), amount(a) {}
    void execute() override { med->allocateResources(res, amount); }
    void undo() override { std::cout << "[Undo] Resources already converted.\n"; }
};

class StatusCommand : public ICommand {
    GameMediator* med;
public:
    StatusCommand(GameMediator* m) : med(m) {}
    void execute() override { med->printStatus(); }
    void undo() override {}
};

class UndoCommand : public ICommand {
    GameMediator* med;
public:
    UndoCommand(GameMediator* m) : med(m) {}
    void execute() override { med->undoLastCommand(); }
    void undo() override {}
};

inline void GameMediator::executeCommand(std::unique_ptr<ICommand> cmd) {
    cmd->execute();
    history.push_back(std::move(cmd));
}

inline void GameMediator::undoLastCommand() {
    if (!history.empty()) {
        std::cout << "[Mediator] Undoing last action...\n";
        history.back()->undo();
        history.pop_back();
    }
    else {
        std::cout << "[Mediator] Nothing to undo.\n";
    }
}
