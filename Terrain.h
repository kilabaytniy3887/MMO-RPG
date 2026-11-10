#pragma once
#include "Unit.h"
#include <iostream>
#include <memory>

class Terrain {
public:
    virtual ~Terrain() = default;
    virtual bool isPassable(const Unit& unit) const = 0;
    virtual double getSpeedModifier(const Unit& unit) const = 0;
    virtual double getDefenseBonus(const Unit& unit) const = 0;
    virtual std::string getName() const = 0;
};

class Plains : public Terrain {
public:
    bool isPassable(const Unit&) const override { return true; }
    double getSpeedModifier(const Unit&) const override { return 1.0; }
    double getDefenseBonus(const Unit&) const override { return 0.0; }
    std::string getName() const override { return "Plains"; }
};

class Forest : public Terrain {
public:
    bool isPassable(const Unit& unit) const override {
        // Тяжёлые танки не проходят через лес
        if (unit.getType() == "Paladin" || unit.getType() == "Vampir") return false;
        return true;
    }
    double getSpeedModifier(const Unit& unit) const override {
        if (unit.getType() == "Mage" || unit.getType() == "Berserk") return 0.5;
        return 1.0;
    }
    double getDefenseBonus(const Unit&) const override { return 0.2; }
    std::string getName() const override { return "Forest"; }
};

class Mountains : public Terrain {
public:
    bool isPassable(const Unit& unit) const override {
        // По горам могут ходить только те, у кого скорость 2 и более
        return unit.getSpeed() >= 2;
    }
    double getSpeedModifier(const Unit&) const override { return 0.5; }
    double getDefenseBonus(const Unit&) const override { return 0.3; }
    std::string getName() const override { return "Mountains"; }
};

class Swamp : public Terrain {
public:
    bool isPassable(const Unit& unit) const override {
        // По болоту могут ходить все, у кого скорость 1 и более
        return unit.getSpeed() >= 1;
    }
    double getSpeedModifier(const Unit&) const override { return 0.5; } // Болото понижает скорость до x0.5
    double getDefenseBonus(const Unit&) const override { return -0.1; }
    std::string getName() const override { return "Swamp"; }
};

class TerrainProxy : public Terrain {
private:
    std::shared_ptr<Terrain> realTerrain;
public:
    TerrainProxy(std::shared_ptr<Terrain> t) : realTerrain(t) {}

    bool isPassable(const Unit& unit) const override {
        bool passable = realTerrain->isPassable(unit);
        std::cout << "[Proxy] Passability check for " << unit.getType()
            << " on " << realTerrain->getName() << ": "
            << (passable ? "Yes" : "No") << "\n";
        return passable;
    }
    double getSpeedModifier(const Unit& unit) const override {
        double speed = realTerrain->getSpeedModifier(unit);
        std::cout << "[Proxy] Speed on " << realTerrain->getName() << " = " << speed << "\n";
        return speed;
    }
    double getDefenseBonus(const Unit& unit) const override {
        return realTerrain->getDefenseBonus(unit);
    }
    std::string getName() const override {
        return realTerrain->getName();
    }
};