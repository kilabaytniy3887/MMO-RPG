#pragma once
#include <string>
#include "Attributes.h"

class DamageFactory;
class TankFactory;
class FlyingFactory;

class Unit {
protected:
    int x, y;
    Health health;
    Armor armor;
    Attack attack;

    Unit(int x, int y, int hp, int arm, int atk);

public:
    virtual ~Unit() = default;

    int getX() const { return x; }
    int getY() const { return y; }
    const Health& getHealth() const { return health; }
    const Armor& getArmor()  const { return armor; }
    const Attack& getAttack() const { return attack; }

    void move(int newX, int newY) { x = newX; y = newY; }

    int attackUnit(Unit& target) {
        int damage = attack.calculateDamage(target.getArmor());
        target.health.decrease(damage);
        return damage;
    }

    virtual std::string getType() const = 0;
    virtual Unit* clone() const = 0;
};

class Damage : public Unit {
protected:
    Damage(int x, int y, int hp, int arm, int atk) : Unit(x, y, hp, arm, atk) {}
};

class Mage : public Damage {
private:
    Mage(int x, int y) : Damage(x, y, 100, 10, 20) {}
public:
    friend class DamageFactory;
    std::string getType() const override { return "Mage"; }
    Unit* clone() const override { return new Mage(*this); }
};

class Berserk : public Damage {
private:
    Berserk(int x, int y) : Damage(x, y, 80, 15, 15) {}
public:
    friend class DamageFactory;
    std::string getType() const override { return "Berserk"; }
    Unit* clone() const override { return new Berserk(*this); }
};

class Tank : public Unit {
protected:
    Tank(int x, int y, int hp, int arm, int atk) : Unit(x, y, hp, arm, atk) {}
};

class Paladin : public Tank {
private:
    Paladin(int x, int y) : Tank(x, y, 60, 5, 25) {}
public:
    friend class TankFactory;
    std::string getType() const override { return "Paladin"; }
    Unit* clone() const override { return new Paladin(*this); }
};

class Vampir : public Tank {
private:
    Vampir(int x, int y) : Tank(x, y, 90, 10, 20) {}
public:
    friend class TankFactory;
    std::string getType() const override { return "Vampir"; }
    Unit* clone() const override { return new Vampir(*this); }
};

class Flying : public Unit {
protected:
    Flying(int x, int y, int hp, int arm, int atk) : Unit(x, y, hp, arm, atk) {}
};

class Pegas : public Flying {
private:
    Pegas(int x, int y) : Flying(x, y, 120, 20, 30) {}
public:
    friend class FlyingFactory;
    std::string getType() const override { return "Pegas"; }
    Unit* clone() const override { return new Pegas(*this); }
};

class DarkKnight : public Flying {
private:
    DarkKnight(int x, int y) : Flying(x, y, 80, 10, 25) {}
public:
    friend class FlyingFactory;
    std::string getType() const override { return "Dark Knight"; }
    Unit* clone() const override { return new DarkKnight(*this); }
};