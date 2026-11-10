#pragma once
#include <string>
#include "Attributes.h"
#include "Observer.h"

class Base;

class Unit : public IObservable {
protected:
    int id;
    int x, y;
    Health health;
    Armor armor;
    Attack attack;
    int tempDamageBoost;
    int dmgMult;
    int gold;
    int speed;
    Base* ownerBase;

public:
    Unit(int id, int x, int y, int hp, int arm, int atk);

    virtual ~Unit() = default;

    int getId() const { return id; }
    int getX() const { return x; }
    int getY() const { return y; }
    Health& getHealth() { return health; }
    const Health& getHealth() const { return health; }

    int getSpeed() const { return speed; }

    int getAttack() const { return (attack.getValue() + tempDamageBoost) * dmgMult; }

    void setTempDamageBoost(int b) { tempDamageBoost += b; }
    void setBase(Base* b) { ownerBase = b; }
    Base* getBase() const { return ownerBase; }

    int getGold() const { return gold; }
    void addGold(int amount) { gold += amount; }
    void spendGold(int amount) { if (gold >= amount) gold -= amount; }
    int getDamageMultiplier() const { return dmgMult; }
    void setDamageMultiplier(int mult) { dmgMult = mult; }

    void move(int newX, int newY) { x = newX; y = newY; }

    void takeDamage(int dmg) {
        int finalDmg = armor.reduceDamage(dmg);
        health.decrease(finalDmg);
        notify({ EventType::Damaged, id, "Unit damaged" });
        if (health.isZero()) {
            notify({ EventType::Destroyed, id, "Unit destroyed" });
        }
    }

    void heal(int amount) { health.increase(amount); }

    int attackUnit(Unit& target) {
        int rawDamage = getAttack();
        target.takeDamage(rawDamage);
        return rawDamage;
    }

    virtual std::string getType() const = 0;
    virtual Unit* clone() const = 0;
};


class Damage : public Unit {
public:
    Damage(int id, int x, int y, int hp, int arm, int atk);
};

class Mage : public Damage {
public:
    Mage(int id, int x, int y);
    std::string getType() const override { return "Mage"; }
    Unit* clone() const override { return new Mage(*this); }
};

class Berserk : public Damage {
public:
    Berserk(int id, int x, int y);
    std::string getType() const override { return "Berserk"; }
    Unit* clone() const override { return new Berserk(*this); }
};


class Tank : public Unit {
public:
    Tank(int id, int x, int y, int hp, int arm, int atk);
};

class Paladin : public Tank {
public:
    Paladin(int id, int x, int y);
    std::string getType() const override { return "Paladin"; }
    Unit* clone() const override { return new Paladin(*this); }
};

class Vampir : public Tank {
public:
    Vampir(int id, int x, int y);
    std::string getType() const override { return "Vampir"; }
    Unit* clone() const override { return new Vampir(*this); }
};

class Flying : public Unit {
public:
    Flying(int id, int x, int y, int hp, int arm, int atk);
};

class Pegas : public Flying {
public:
    Pegas(int id, int x, int y);
    std::string getType() const override { return "Pegas"; }
    Unit* clone() const override { return new Pegas(*this); }
};

class DarkKnight : public Flying {
public:
    DarkKnight(int id, int x, int y);
    std::string getType() const override { return "Dark Knight"; }
    Unit* clone() const override { return new DarkKnight(*this); }
};