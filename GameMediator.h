#pragma once
#include "Base.h"
#include "GameField.h"
#include "UnitFactory.h"
#include "GameEntities.h"
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include <map>

class ICommand;

class GameMediator {
private:
    Base* base;
    GameField* field;
    AllUnitsFactory* factory;

    std::vector<Building> buildings;
    std::vector<Research> research;
    std::vector<std::unique_ptr<ICommand>> history;

    int nextBuildingId;
    int nextUnitId;
    int currentTurn;

    int currentEnergy;
    int maxEnergy;
    int labLevel;

    bool armorResearchDone;
    bool damageResearchDone;
    bool speedResearchDone;

    int findUnitIndex(int id) {
        for (std::size_t i = 0; i < field->size(); ++i) {
            if (field->getUnit(i)->getId() == id) return static_cast<int>(i);
        }
        return -1;
    }

    bool isBuildingNear(Building::Type type, int x, int y) {
        for (auto& b : buildings) {
            if (b.type == type && b.isBuilt) {
                if (std::abs(b.x - x) <= 1 && std::abs(b.y - y) <= 1) return true;
            }
        }
        return false;
    }

    bool hasBuilding(Building::Type type) {
        for (auto& b : buildings) {
            if (b.type == type && b.isBuilt) return true;
        }
        return false;
    }

    bool isResearchActive(const std::string& name) const {
        for (const auto& r : research) {
            if (r.name == name && r.turnsLeft > 0) return true;
        }
        return false;
    }

    bool isCellFree(int x, int y) const {
        if (x == base->getX() && y == base->getY()) return false;

        for (const auto& b : buildings) {
            if (b.x == x && b.y == y) return false;
        }

        for (std::size_t i = 0; i < field->size(); ++i) {
            Unit* u = field->getUnit(i);
            if (u->getX() == x && u->getY() == y) return false;
        }

        return true;
    }

public:
    GameMediator(Base* b, GameField* f, AllUnitsFactory* fac)
        : base(b), field(f), factory(fac), nextBuildingId(1), nextUnitId(1),
        currentTurn(1), currentEnergy(200), maxEnergy(200), labLevel(0),
        armorResearchDone(false), damageResearchDone(false), speedResearchDone(false) {
    }

    void executeCommand(std::unique_ptr<ICommand> cmd);
    void undoLastCommand();

    int getCurrentTurn() const { return currentTurn; }

    int getUnitMovesLeft(int id) {
        int idx = findUnitIndex(id);
        return idx != -1 ? field->getUnit(idx)->getMovesLeft() : 0;
    }

    int getUnitMaxMoves(int id) {
        int idx = findUnitIndex(id);
        return idx != -1 ? field->getUnit(idx)->getSpeed() : 0;
    }

    std::pair<int, int> getBuildingCoords(int id) {
        for (auto& b : buildings) {
            if (b.id == id) return { b.x, b.y };
        }
        return { -1, -1 };
    }

    std::map<std::pair<int, int>, char> getBuildingsForMap() const {
        std::map<std::pair<int, int>, char> res;
        for (const auto& b : buildings) {
            char c = '?';
            if (b.type == Building::Type::Barracks) c = 'b';
            else if (b.type == Building::Type::Hospital) c = 'h';
            else if (b.type == Building::Type::PowerPlant) c = 'e';
            else if (b.type == Building::Type::Laboratory) c = 'u';
            else if (b.type == Building::Type::Warehouse) c = 'w';
            res[{b.x, b.y}] = c;
        }
        return res;
    }

    void update() {
        currentTurn++;

        for (auto& b : buildings) {
            bool wasBuilt = b.isBuilt;
            b.update();
            if (!wasBuilt && b.isBuilt) {
                if (b.type == Building::Type::Laboratory && labLevel < 5) {
                    labLevel++;
                    std::cout << "[Mediator] Lab finished! Lab Level is now " << labLevel << ".\n";
                }
                else if (b.type == Building::Type::PowerPlant) {
                    maxEnergy += 500;
                    std::cout << "[Mediator] Power Plant finished! Max Energy increased to " << maxEnergy << ".\n";
                }
                else if (b.type == Building::Type::Warehouse) {
                    base->increaseStorageLimit(1000);
                    std::cout << "[Mediator] Warehouse finished! Resource storage limit increased to "
                        << base->getMaxResources() << ".\n";
                }
            }
        }

        for (auto it = research.begin(); it != research.end();) {
            it->update();
            if (it->turnsLeft == 0) {
                if (it->name == "ArmorUp" && !armorResearchDone) {
                    armorResearchDone = true;
                    for (std::size_t i = 0; i < field->size(); ++i) field->getUnit(i)->buffArmor(20);
                }
                else if (it->name == "DamageUp" && !damageResearchDone) {
                    damageResearchDone = true;
                    for (std::size_t i = 0; i < field->size(); ++i) field->getUnit(i)->buffDamage(30);
                }
                else if (it->name == "SpeedUp" && !speedResearchDone) {
                    speedResearchDone = true;
                    for (std::size_t i = 0; i < field->size(); ++i) field->getUnit(i)->buffSpeed(1);
                }
                it = research.erase(it);
            }
            else {
                ++it;
            }
        }

        for (std::size_t i = 0; i < field->size(); ++i) {
            field->getUnit(i)->resetMoves();
            field->getUnit(i)->setOrder("Idle");
        }

        // Пересчёт энергии за ход
        int energyBalance = 0;
        for (auto& b : buildings) {
            if (b.isBuilt) {
                if (b.type == Building::Type::PowerPlant) energyBalance += 100;
                else if (b.type == Building::Type::Barracks) energyBalance -= 20;
                else if (b.type == Building::Type::Hospital) energyBalance -= 20;
                else if (b.type == Building::Type::Laboratory) energyBalance -= 30;
            }
        }

        currentEnergy += energyBalance;

        if (currentEnergy > maxEnergy) currentEnergy = maxEnergy;
        else if (currentEnergy < 0) {
            currentEnergy = 0;
            std::cout << "[Mediator] WARNING: Energy depleted!\n";
        }
    }

    int buildBuilding(const std::string& type, int x, int y) {
        if (x < 0 || x >= field->getWidth() || y < 0 || y >= field->getHeight()) {
            std::cout << "[Mediator] Coordinates out of map bounds!\n";
            return -1;
        }

        if (!isCellFree(x, y)) {
            std::cout << "[Mediator] Cell (" << x << "," << y << ") is occupied! Choose a free cell.\n";
            return -1;
        }

        int costWood = 0, costOre = 0;
        Building::Type bType = Building::Type::Barracks;

        if (type == "Barracks" || type == "barracks") { bType = Building::Type::Barracks; costWood = 100; costOre = 50; }
        else if (type == "Hospital" || type == "hospital") { bType = Building::Type::Hospital; costWood = 150; costOre = 100; }
        else if (type == "Lab" || type == "lab") { bType = Building::Type::Laboratory; costWood = 200; costOre = 150; }
        else if (type == "PowerPlant" || type == "powerplant") { bType = Building::Type::PowerPlant; costWood = 250; costOre = 200; }
        else if (type == "Warehouse" || type == "warehouse") { bType = Building::Type::Warehouse; costWood = 120; costOre = 80; }
        else return -1;

        if (base->getWood() < costWood || base->getOre() < costOre) {
            std::cout << "[Mediator] Not enough resources to build " << type << "!\n";
            return -1;
        }

        base->addWood(-costWood);
        base->addOre(-costOre);

        buildings.emplace_back(nextBuildingId, bType, x, y);
        std::cout << "[Mediator] Started building " << type << " at (" << x << "," << y << ") [ID: " << nextBuildingId << ", 2 turns left]\n";
        return nextBuildingId++;
    }

    void removeBuilding(int id) {
        auto it = std::remove_if(buildings.begin(), buildings.end(), [id](const Building& b) { return b.id == id; });
        if (it != buildings.end()) buildings.erase(it, buildings.end());
    }

    void researchTech(const std::string& tech) {
        if (labLevel == 0) {
            std::cout << "[Mediator] Need a built Laboratory to research!\n";
            return;
        }

        if (isResearchActive(tech)) {
            std::cout << "[Mediator] Research '" << tech << "' is already in progress!\n";
            return;
        }

        if (tech == "ArmorUp" && labLevel >= 1 && !armorResearchDone) {
            research.emplace_back("ArmorUp", 2);
            std::cout << "[Mediator] Started research: Armor Up (2 turns left).\n";
        }
        else if (tech == "DamageUp" && labLevel >= 2 && !damageResearchDone) {
            research.emplace_back("DamageUp", 2);
            std::cout << "[Mediator] Started research: Damage Up (2 turns left).\n";
        }
        else if (tech == "SpeedUp" && labLevel >= 3 && !speedResearchDone) {
            research.emplace_back("SpeedUp", 2);
            std::cout << "[Mediator] Started research: Speed Up (2 turns left).\n";
        }
        else {
            std::cout << "[Mediator] Cannot research this yet (Requires higher Lab Level or already completed).\n";
        }
    }

    void allocateResources(const std::string& res, int amount) {
        if (amount <= 0) {
            std::cout << "[Mediator] Amount must be positive.\n";
            return;
        }

        if (res == "wood") {
            if (!hasBuilding(Building::Type::PowerPlant)) {
                std::cout << "[Mediator] Need a built PowerPlant to convert Wood to Energy!\n";
                return;
            }
            if (base->getWood() >= amount) {
                base->addWood(-amount);
                currentEnergy += amount;
                if (currentEnergy > maxEnergy) currentEnergy = maxEnergy;
                std::cout << "[Mediator] Converted " << amount << " Wood into Energy. Current Energy: "
                    << currentEnergy << "/" << maxEnergy << "\n";
            }
            else {
                std::cout << "[Mediator] Not enough Wood!\n";
            }
        }
        else if (res == "ore") {
            if (!hasBuilding(Building::Type::Warehouse)) {
                std::cout << "[Mediator] Need a built Warehouse to exchange Metal for Wood!\n";
                return;
            }
            if (base->getOre() >= amount) {
                base->addOre(-amount);
                base->addWood(amount);
                std::cout << "[Mediator] Exchanged " << amount << " Metal into Wood via Warehouse. Current Wood: "
                    << base->getWood() << "/" << base->getMaxResources() << "\n";
            }
            else {
                std::cout << "[Mediator] Not enough Metal!\n";
            }
        }
        else {
            std::cout << "[Mediator] Unknown resource type (use 'wood' or 'ore').\n";
        }
    }
    int produceUnit(const std::string& type, int x, int y) {
        bool nearBarracks = isBuildingNear(Building::Type::Barracks, x, y);
        bool nearBase = (std::abs(base->getX() - x) <= 1 && std::abs(base->getY() - y) <= 1);
        if (!nearBarracks && !nearBase) {
            std::cout << "[Mediator] Can only produce units next to a Barracks or the Base!\n";
            return -1;
        }
        std::string realType = type;
        if (type == "infantry") realType = "Berserk";
        if (realType == "Archangel" && labLevel < 4) { std::cout << "[Mediator] Requires Lab Level 4!\n"; return -1; }
        if (realType == "Titan" && labLevel < 5) { std::cout << "[Mediator] Requires Lab Level 5!\n"; return -1; }
        if (base->hasResources(realType)) {
            base->payResources(realType);
            int id = nextUnitId++;
            Unit* u = factory->createUnit(realType, id, x, y);
            if (u) {
                u->setBase(base);
                if (armorResearchDone) u->buffArmor(20);
                if (damageResearchDone) u->buffDamage(30);
                if (speedResearchDone) u->buffSpeed(1);
                if (field->addUnit(u)) {
                    std::cout << "[Mediator] Unit " << realType << " (ID:" << id << ") produced.\n";
                    return id;
                }
            }
        }
        std::cout << "[Mediator] Failed to produce unit (check resources or free cell).\n";
        return -1;
    }
    void removeUnit(int id) {
        int idx = findUnitIndex(id);
        if (idx != -1) field->removeUnit(idx);
    }
    void moveUnit(int id, int x, int y) {
        int idx = findUnitIndex(id);
        if (idx == -1) return;
        Unit* u = field->getUnit(idx);
        if (!u->hasMoves()) {
            std::cout << "[Mediator] Unit " << id << " has no movement points left this turn!\n";
            return;
        }
        TerrainProxy terrain = field->getTerrainProxy(x, y);
        if (!terrain.isPassable(*u)) {
            std::cout << "[Mediator] Terrain is impassable for " << u->getType() << "!\n";
            return;
        }
        if (field->moveUnit(idx, x, y)) {
            u->spendMove(1);
            u->setOrder("Move to (" + std::to_string(x) + "," + std::to_string(y) + ")");
            std::cout << "[Mediator] Unit " << id << " moved to (" << x << "," << y
                << "). Moves left: " << u->getMovesLeft() << "/" << u->getSpeed() << "\n";
        }
        else {
            std::cout << "[Mediator] Cannot move unit (blocked or out of bounds).\n";
        }
    }
    void undoMoveUnit(int id, int oldX, int oldY) {
        int idx = findUnitIndex(id);
        if (idx != -1) field->moveUnit(idx, oldX, oldY);
    }
    void gatherResources(int id) {
        int idx = findUnitIndex(id);
        if (idx == -1) return;
        Unit* u = field->getUnit(idx);
        TerrainProxy t = field->getTerrainProxy(u->getX(), u->getY());
        std::string tName = t.getName();
        std::string uType = u->getType();
        if (tName == "Forest") {
            int amount = 50;
            if (uType == "BeaverMutant") amount *= 3;
            base->addWood(amount);
            u->setOrder("Gather Wood");
            std::cout << "[Mediator] " << uType << " gathered Wood. Storage: "
                << base->getWood() << "/" << base->getMaxResources() << "\n";
        }
        else if (tName == "Mountains") {
            int amount = 30;
            if (uType == "Kobold") amount *= 3;
            base->addOre(amount);
            u->setOrder("Mine Ore");
            std::cout << "[Mediator] " << uType << " mined Metal. Storage: "
                << base->getOre() << "/" << base->getMaxResources() << "\n";
        }
        else {
            std::cout << "[Mediator] Cannot gather here.\n";
        }
    }
    void undoGather(int id, const std::string& type, int amount) {
        if (type == "wood") base->addWood(-amount);
        if (type == "ore") base->addOre(-amount);
    }
    void healUnit(int id) {
        int idx = findUnitIndex(id);
        if (idx != -1) {
            Unit* u = field->getUnit(idx);
            if (!isBuildingNear(Building::Type::Hospital, u->getX(), u->getY())) {
                std::cout << "[Mediator] Unit must be next to a Hospital to heal!\n";
                return;
            }
            u->getHealth().healToMax();
            u->setOrder("Heal at Hospital");
            std::cout << "[Mediator] Unit " << id << " fully healed.\n";
        }
    }
    void attack(int attackerId, int targetId) {
        int aIdx = findUnitIndex(attackerId);
        int dIdx = findUnitIndex(targetId);
        if (aIdx != -1 && dIdx != -1) {
            if (aIdx == dIdx) return;
            Unit* atk = field->getUnit(aIdx);
            Unit* def = field->getUnit(dIdx);
            atk->attackUnit(*def);
            atk->setOrder("Attack Unit ID:" + std::to_string(targetId));
            std::cout << "[Mediator] " << atk->getType() << " attacked " << def->getType()
                << " (Target HP: " << def->getHealth().getValue() << "/" << def->getHealth().getMaxValue() << ")\n";
        }
        else {
            std::cout << "[Mediator] Attacker or target unit not found!\n";
        }
    }
    void attackBuilding(int attackerId, int buildingId) {
        int aIdx = findUnitIndex(attackerId);
        if (aIdx == -1) {
            std::cout << "[Mediator] Attacker unit not found!\n";
            return;
        }
        Unit* atk = field->getUnit(aIdx);
        for (auto it = buildings.begin(); it != buildings.end(); ++it) {
            if (it->id == buildingId) {
                int dmg = atk->getAttack();
                it->hp -= dmg;
                if (it->hp < 0) it->hp = 0;
                atk->setOrder("Attack Building ID:" + std::to_string(buildingId));
                std::cout << "[Mediator] " << atk->getType() << " attacked " << it->getTypeName()
                    << " (ID:" << buildingId << ") for " << dmg << " damage! "
                    << "(Building HP: " << it->hp << "/" << it->maxHp << ")\n";
                if (it->hp == 0) {
                    if (it->type == Building::Type::Warehouse && it->isBuilt) {
                        base->increaseStorageLimit(-1000);
                        std::cout << "[Mediator] Warehouse destroyed! Storage limit decreased to "
                            << base->getMaxResources() << ".\n";
                    }
                    std::cout << "[Mediator] Building " << it->getTypeName() << " (ID:" << buildingId << ") was destroyed!\n";
                    buildings.erase(it);
                }
                return;
            }
        }
        std::cout << "[Mediator] Building with ID " << buildingId << " not found!\n";
    }
    void printStatus() {
        int maxRes = base->getMaxResources();
        std::cout << "\n=== GAME STATUS (TURN " << currentTurn << ") ===\n";
        std::cout << "Resources: " << base->getWood() << "/" << maxRes << " Wood, "
            << base->getOre() << "/" << maxRes << " Metal, "
            << base->getGold() << "/" << maxRes << " Gold\n";
        std::cout << "Energy: " << currentEnergy << "/" << maxEnergy << "\n";
        std::cout << "Lab Level: " << labLevel << "/5\n";
        std::cout << "Completed Research: Armor[" << (armorResearchDone ? "X" : " ") << "] Damage[" << (damageResearchDone ? "X" : " ")
            << "] Speed[" << (speedResearchDone ? "X" : " ") << "]\n";
        std::cout << "Active Research (" << research.size() << "):\n";
        for (const auto& r : research) {
            std::cout << "  - " << r.name << " (" << r.turnsLeft << " turns left)\n";
        }
        std::cout << "Units (" << field->size() << "):\n";
        for (std::size_t i = 0; i < field->size(); ++i) {
            Unit* u = field->getUnit(i);
            std::cout << "  - ID:" << u->getId() << " " << u->getType()
                << " HP:" << u->getHealth().getValue() << "/" << u->getHealth().getMaxValue()
                << " ARM:" << u->getArmor()
                << " ATK:" << u->getAttack()
                << " MOVES:" << u->getMovesLeft() << "/" << u->getSpeed()
                << " ORDER:[" << u->getOrder() << "]"
                << " (" << u->getX() << "," << u->getY() << ")\n";
        }
        std::cout << "Buildings (" << buildings.size() << "):\n";
        for (auto& b : buildings) {
            std::cout << "  - " << b.getTypeName() << " (ID:" << b.id << ")"
                << " HP:" << b.hp << "/" << b.maxHp
                << " at (" << b.x << "," << b.y << ") "
                << (b.isBuilt ? "[BUILT]" : "[BUILDING: " + std::to_string(b.buildTimeLeft) + " turns left]") << "\n";
        }
        std::cout << "==============================\n";
    }
    int getUnitX(int id) { int i = findUnitIndex(id); return i != -1 ? field->getUnit(i)->getX() : -1; }
    int getUnitY(int id) { int i = findUnitIndex(id); return i != -1 ? field->getUnit(i)->getY() : -1; }
};