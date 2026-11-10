#pragma once
#include "Unit.h"
#include "Base.h"
#include <iostream>
#include <memory>
#include <cmath>
#include <utility>


class IInteractionStrategy {
public:
    virtual ~IInteractionStrategy() = default;
    virtual void apply(Unit& unit) = 0;
};

class HealthRestoreStrategy : public IInteractionStrategy {
public:
    void apply(Unit& unit) override {
        unit.getHealth().increase(50);
    }
};

class AttackBoostStrategy : public IInteractionStrategy {
private:
    int boost;
public:
    explicit AttackBoostStrategy(int b = 15) : boost(b) {}
    void apply(Unit& unit) override {
        unit.setTempDamageBoost(boost);
    }
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

class GoldRewardStrategy : public IInteractionStrategy {
private:
    int amount;
public:
    explicit GoldRewardStrategy(int a = 200) : amount(a) {}
    void apply(Unit& unit) override {
        unit.addGold(amount);
    }
};

class NoOpStrategy : public IInteractionStrategy {
public:
    void apply(Unit&) override {}
};


class NeutralObject {
protected:
    std::shared_ptr<IInteractionStrategy> strategy;

public:
    explicit NeutralObject(std::shared_ptr<IInteractionStrategy> strat)
        : strategy(std::move(strat)) {
    }

    virtual ~NeutralObject() = default;

    virtual void interact(Unit& unit) {
        if (strategy) strategy->apply(unit);
    }

    virtual char getSymbol() const { return ' '; }
    virtual int getColor() const { return 7; }
};


class Altar : public NeutralObject {
public:
    Altar() : NeutralObject(std::make_shared<AttackBoostStrategy>(15)) {}

    void interact(Unit& unit) override {
        std::cout << "Altar: You feel a surge of power!\n";
        NeutralObject::interact(unit);
    }

    char getSymbol() const override { return 'I'; }
    int getColor() const override { return 13; }
};

class EnergySource : public NeutralObject {
public:
    EnergySource() : NeutralObject(std::make_shared<HealthRestoreStrategy>()) {}

    void interact(Unit& unit) override {
        std::cout << "Fountain: Unit fully healed!\n";
        NeutralObject::interact(unit);
    }

    char getSymbol() const override { return 'I'; }
    int getColor() const override { return 12; }
};

class Trap : public NeutralObject {
public:
    Trap() : NeutralObject(std::make_shared<DamageStrategy>()) {}

    void interact(Unit& unit) override {
        std::cout << "Trap: Unit lost half of its health!\n";
        NeutralObject::interact(unit);
    }

    char getSymbol() const override { return ' '; }
};

class TreasureChest : public NeutralObject {
public:
    TreasureChest() : NeutralObject(std::make_shared<GoldRewardStrategy>(200)) {}

    void interact(Unit& unit) override {
        std::cout << "Chest: Unit found 200 gold!\n";
        NeutralObject::interact(unit);
    }

    char getSymbol() const override { return ' '; }
};

class Castle : public NeutralObject {
public:
    Castle() : NeutralObject(std::make_shared<NoOpStrategy>()) {}

    void interact(Unit& unit) override {
        std::cout << "Castle: This is your base.\n";
        NeutralObject::interact(unit);
    }

    char getSymbol() const override { return 'A'; }
    int getColor() const override { return 6; }
};


inline void operator+(Unit& unit, TreasureChest& chest) { chest.interact(unit); }
inline void operator-(Unit& unit, Trap& trap) { trap.interact(unit); }
inline void operator*(Unit& unit, Altar& altar) { altar.interact(unit); }
inline void operator/(Unit& unit, EnergySource& src) { src.interact(unit); }