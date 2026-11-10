#pragma once
#include "Observer.h"
#include "UnitFactory.h"
#include "Unit.h"
#include <vector>
#include <iostream>
#include <algorithm>

class Base : public IObserver {
private:
    int x, y;
    int health;
    int maxUnits;
    int currentUnits;
    int defenseLevel;
    int resources;
    std::vector<Unit*> units;
    std::vector<int> destroyedUnitIds;
    int nextUnitId;
    UnitFactory* factory;

public:
    Base(int startX, int startY, int hp, int maxU, int def, int res, UnitFactory* f)
        : x(startX), y(startY), health(hp), maxUnits(maxU), currentUnits(0),
        defenseLevel(def), resources(res), nextUnitId(1), factory(f) {
    }

    ~Base() {
        for (Unit* u : units) delete u;
        units.clear();
    }

    void addResources(int amount) { resources += amount; }

    void createUnit(const std::string& type, int unitX, int unitY) {
        if (currentUnits >= maxUnits) {
            std::cout << "Base: Unit limit reached!\n";
            return;
        }
        Unit* newUnit = factory->createUnit(type, nextUnitId++, unitX, unitY);
        if (newUnit) {
            newUnit->setBase(this);
            newUnit->subscribe(this);
            units.push_back(newUnit);
            currentUnits++;
            std::cout << "Base: Created unit " << type
                << " (ID: " << newUnit->getId() << ")\n";
            UnitEvent createEvent = { EventType::Created, newUnit->getId(), "Created" };
            newUnit->notify(createEvent);
        }
    }

    void onUnitEvent(const UnitEvent& event) override {
        if (event.type == EventType::Created) {
            std::cout << "[Observer] Base notified: Unit ID " << event.unitId << " created!\n";
        }
        else if (event.type == EventType::Destroyed) {
            std::cout << "[Observer] Base notified: Unit ID " << event.unitId << " destroyed!\n";
            currentUnits--;
            destroyedUnitIds.push_back(event.unitId);
            auto it = std::remove_if(units.begin(), units.end(),
                [event](Unit* u) { return u->getId() == event.unitId; });
            units.erase(it, units.end());
        }
        else if (event.type == EventType::Damaged) {
            std::cout << "[Observer] Base notified: Unit ID " << event.unitId << " took damage.\n";
        }
    }

    Unit* getUnit(int index) {
        if (index >= 0 && index < (int)units.size()) return units[index];
        return nullptr;
    }
};