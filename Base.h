#pragma once
#include "Observer.h"
#include "UnitFactory.h"
#include "Unit.h"
#include "Logger.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <string>

class Base : public IObserver {
private:
    int x, y;
    int health;
    int maxUnits;
    int currentUnits;
    int defenseLevel;
    int gold, ore, wood;
    int maxResourceStorage;
    std::vector<Unit*> units;
    std::vector<int> destroyedUnitIds;
    int nextUnitId;
    UnitFactory* factory;
    LoggingProxy* logger = nullptr;

public:
    Base(int startX, int startY, int hp, int maxU, int def,
        int g, int o, int w, UnitFactory* f, int maxStorage = 1500)
        : x(startX), y(startY), health(hp), maxUnits(maxU), currentUnits(0),
        defenseLevel(def), gold(g), ore(o), wood(w), maxResourceStorage(maxStorage),
        nextUnitId(1), factory(f) {
    }

    ~Base() {
        for (Unit* u : units) delete u;
        units.clear();
    }

    void setLogger(LoggingProxy* log) { logger = log; }

    int getX() const { return x; }
    int getY() const { return y; }
    int getGold() const { return gold; }
    int getOre()  const { return ore; }
    int getWood() const { return wood; }
    int getMaxResources() const { return maxResourceStorage; }
    int getHealth() const { return health; }
    int getMaxUnits() const { return maxUnits; }
    int getCurrentUnits() const { return currentUnits; }

    void increaseStorageLimit(int amount) { maxResourceStorage += amount; }

    void addGold(int a) {
        gold += a;
        if (gold > maxResourceStorage) gold = maxResourceStorage;
        if (gold < 0) gold = 0;
    }

    void addOre(int a) {
        ore += a;
        if (ore > maxResourceStorage) ore = maxResourceStorage;
        if (ore < 0) ore = 0;
    }

    void addWood(int a) {
        wood += a;
        if (wood > maxResourceStorage) wood = maxResourceStorage;
        if (wood < 0) wood = 0;
    }

    void addResources(int amount) { addGold(amount); }

    static int goldCost(const std::string& t) {
        if (t == "Mage") return 100; if (t == "Berserk") return 80; if (t == "Paladin") return 150;
        if (t == "Vampir") return 120; if (t == "Pegas") return 200; if (t == "DarkKnight") return 250;
        if (t == "Archangel") return 300; if (t == "Titan") return 400; if (t == "Kobold") return 60;
        if (t == "BeaverMutant") return 60; return -1;
    }
    static int oreCost(const std::string& t) {
        if (t == "Mage") return 20; if (t == "Berserk") return 30; if (t == "Paladin") return 100;
        if (t == "Vampir") return 80; if (t == "Pegas") return 50; if (t == "DarkKnight") return 150;
        if (t == "Archangel") return 200; if (t == "Titan") return 300; if (t == "Kobold") return 20;
        if (t == "BeaverMutant") return 10; return -1;
    }
    static int woodCost(const std::string& t) {
        if (t == "Mage") return 50; if (t == "Berserk") return 40; if (t == "Paladin") return 30;
        if (t == "Vampir") return 60; if (t == "Pegas") return 100; if (t == "DarkKnight") return 80;
        if (t == "Archangel") return 150; if (t == "Titan") return 200; if (t == "Kobold") return 10;
        if (t == "BeaverMutant") return 20; return -1;
    }

    bool hasResources(const std::string& t) const {
        int cg = goldCost(t), co = oreCost(t), cw = woodCost(t);
        return cg >= 0 && gold >= cg && ore >= co && wood >= cw;
    }

    bool payResources(const std::string& t) {
        if (!hasResources(t)) return false;
        gold -= goldCost(t); ore -= oreCost(t); wood -= woodCost(t);
        return true;
    }

    bool isAdjacent(int ux, int uy) const {
        int dx = std::abs(ux - x); int dy = std::abs(uy - y);
        return dx <= 1 && dy <= 1 && !(dx == 0 && dy == 0);
    }

    Unit* spawnUnit(const std::string& type, int ux, int uy) {
        Unit* u = factory->createUnit(type, nextUnitId++, ux, uy);
        if (u) u->setBase(this);
        return u;
    }

    void createUnit(const std::string& type, int unitX, int unitY) {
        if (currentUnits >= maxUnits) {
            std::cout << "Base: Unit limit reached!\n"; return;
        }
        Unit* newUnit = factory->createUnit(type, nextUnitId++, unitX, unitY);
        if (newUnit) {
            newUnit->setBase(this);
            newUnit->setLogger(logger);
            newUnit->subscribe(this);
            units.push_back(newUnit);
            currentUnits++;

            if (logger) {
                std::stringstream ss;
                ss << *this << " created unit " << type << " (ID=" << newUnit->getId() << ")";
                logger->log("INFO", "BASE", ss.str());
            }

            std::cout << "Base: Created unit " << type << " (ID: " << newUnit->getId() << ")\n";
            newUnit->notify({ EventType::Created, newUnit->getId(), "Created" });
        }
    }

    void onUnitEvent(const UnitEvent& event) override {
        if (event.type == EventType::Destroyed) {
            if (currentUnits > 0) currentUnits--;
            destroyedUnitIds.push_back(event.unitId);
            auto it = std::remove_if(units.begin(), units.end(),
                [event](Unit* u) { return u->getId() == event.unitId; });
            units.erase(it, units.end());
        }
    }

    Unit* getUnit(int index) {
        if (index >= 0 && index < (int)units.size()) return units[index];
        return nullptr;
    }

    friend std::ostream& operator<<(std::ostream& os, const Base& b) {
        os << "Base[X:" << b.getX() << " Y:" << b.getY() << " Units:" << b.getCurrentUnits() << "]";
        return os;
    }
};