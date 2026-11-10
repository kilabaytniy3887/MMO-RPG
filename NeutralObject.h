#pragma once
#include "Unit.h"
#include "Base.h"
#include <iostream>
#include <memory>
#include <cmath>

class IInteractionStrategy {
public:
    virtual ~IInteractionStrategy() = default;
    virtual void apply(Unit& unit) = 0;
};

class HealthRestoreStrategy : public IInteractionStrategy {
public:
    void apply(Unit& unit) override { unit.getHealth().increase(9999); }
};

class AttackBoostStrategy : public IInteractionStrategy {
public:
    void apply(Unit& unit) override { unit.setTempDamageBoost(15); }
};

class DamageStrategy : public IInteractionStrategy {
public:
    void apply(Unit& unit) override {
        int dmg = static_cast<int>(std::ceil(unit.getHealth().getValue() / 2.0));
        unit.takeDamage(dmg);
    }
};

class ResourceStrategy : public IInteractionStrategy {
public:
    void apply(Unit& unit) override {
        if (unit.getBase()) unit.getBase()->addResources(200);
    }
};

class NeutralObject {
protected:
    std::shared_ptr<IInteractionStrategy> strategy;
public:
    NeutralObject(std::shared_ptr<IInteractionStrategy> strat) : strategy(strat) {}
    virtual ~NeutralObject() = default;
    virtual void interact(Unit& unit) = 0;
    virtual char getSymbol() const { return ' '; }
    virtual int getColor() const { return 7; } // По умолчанию белый
};

class Altar : public NeutralObject {
public:
    Altar() : NeutralObject(nullptr) {}
    void interact(Unit& unit) override {
        std::cout << "Altar: You feel a surge of power!\n";
    }
    char getSymbol() const override { return 'I'; }
    int getColor() const override { return 13; } // Фиолетовый
};

class EnergySource : public NeutralObject { // Фонтан здоровья
public:
    EnergySource() : NeutralObject(nullptr) {}
    void interact(Unit& unit) override {
        std::cout << "Fountain: Unit fully healed!\n";
        unit.heal(9999);
    }
    char getSymbol() const override { return 'I'; }
    int getColor() const override { return 12; } // Красный
};

class Trap : public NeutralObject {
public:
    Trap() : NeutralObject(nullptr) {}
    void interact(Unit& unit) override {
        std::cout << "Trap: Unit lost half of its health!\n";
        unit.getHealth().decrease(unit.getHealth().getValue() / 2);
    }
    char getSymbol() const override { return ' '; } // Невидимый
};

class TreasureChest : public NeutralObject {
public:
    TreasureChest() : NeutralObject(nullptr) {}
    void interact(Unit& unit) override {
        std::cout << "Chest: Unit found 200 gold!\n";
        unit.addGold(200);
    }
    char getSymbol() const override { return ' '; } // Невидимый
};

class Castle : public NeutralObject {
public:
    Castle() : NeutralObject(nullptr) {}
    void interact(Unit& unit) override {
        std::cout << "Castle: This is your base.\n";
    }
    char getSymbol() const override { return 'A'; }
    int getColor() const override { return 6; } // Коричневый
};

inline void operator+(Unit& unit, TreasureChest& chest) { chest.interact(unit); }
inline void operator-(Unit& unit, Trap& trap) { trap.interact(unit); }
inline void operator*(Unit& unit, Altar& altar) { altar.interact(unit); }
inline void operator/(Unit& unit, EnergySource& source) { source.interact(unit); }