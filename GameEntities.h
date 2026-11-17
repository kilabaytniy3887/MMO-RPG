#pragma once
#include <string>
#include <iostream>

class Building {
public:
    enum class Type { Barracks, PowerPlant, Laboratory, Warehouse, Hospital };

    int id;
    Type type;
    int x, y;
    int hp, maxHp;
    bool isBuilt;
    int buildTimeLeft;

    Building(int id, Type type, int x, int y)
        : id(id), type(type), x(x), y(y), hp(100), maxHp(100), isBuilt(false), buildTimeLeft(2) {
    }

    std::string getTypeName() const {
        switch (type) {
        case Type::Barracks:    return "Barracks";
        case Type::PowerPlant:  return "PowerPlant";
        case Type::Laboratory:  return "Laboratory";
        case Type::Warehouse:   return "Warehouse";
        case Type::Hospital:    return "Hospital";
        default:                return "Unknown";
        }
    }

    void update() {
        if (!isBuilt && buildTimeLeft > 0) {
            buildTimeLeft--;
            if (buildTimeLeft == 0) {
                isBuilt = true;
                std::cout << "[World] " << getTypeName() << " (ID: " << id << ") construction finished!\n";
            }
        }
    }
};

struct Research {
    std::string name;
    int turnsLeft;

    Research(std::string n, int t) : name(n), turnsLeft(t) {}

    void update() {
        if (turnsLeft > 0) {
            turnsLeft--;
            if (turnsLeft == 0) {
                std::cout << "[World] Research complete: " << name << "!\n";
            }
        }
    }
};