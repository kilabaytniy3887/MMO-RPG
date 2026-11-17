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
    int movesLeft;
    std::string currentOrder;
    Base* ownerBase;

public:
    Unit(int id, int x, int y, int hp, int arm, int atk)
        : id(id), x(x), y(y), health(hp), armor(arm), attack(atk),
        tempDamageBoost(0), dmgMult(1), gold(0), speed(1), movesLeft(1),
        currentOrder("Idle"), ownerBase(nullptr) {
    }

    virtual ~Unit() = default;

    int getId() const { return id; }
    int getX() const { return x; }
    int getY() const { return y; }
    Health& getHealth() { return health; }
    const Health& getHealth() const { return health; }

    int getArmor() const { return armor.getValue(); }
    int getSpeed() const { return speed; }
    int getMovesLeft() const { return movesLeft; }
    bool hasMoves() const { return movesLeft > 0; }
    void spendMove(int cost = 1) { movesLeft = (movesLeft - cost > 0) ? movesLeft - cost : 0; }
    void resetMoves() { movesLeft = speed; }

    std::string getOrder() const { return currentOrder; }
    void setOrder(const std::string& order) { currentOrder = order; }

    int getAttack() const { return (attack.getValue() + tempDamageBoost) * dmgMult; }

    void setTempDamageBoost(int b) { tempDamageBoost += b; }
    void setBase(Base* b) { ownerBase = b; }
    Base* getBase() const { return ownerBase; }

    int getGold() const { return gold; }
    void addGold(int amount) { gold += amount; }
    int getDamageMultiplier() const { return dmgMult; }
    void setDamageMultiplier(int mult) { dmgMult = mult; }

    void move(int newX, int newY) { x = newX; y = newY; }

    void buffArmor(int amount) { armor.increase(amount); }
    void buffDamage(int amount) { attack.increase(amount); }
    void buffSpeed(int amount) {
        speed += amount;
        movesLeft += amount;
    }
    void buffHealth(int amount) { health.increaseMax(amount); }

    void takeDamage(int dmg, double defenseMultiplier = 1.0) {
        int effectiveArmor = static_cast<int>(armor.getValue() * defenseMultiplier);
        int finalDmg = dmg - effectiveArmor;
        if (finalDmg < 0) finalDmg = 0;

        health.decrease(finalDmg);
        notify({ EventType::Damaged, id, "Unit damaged" });
        if (health.isZero()) {
            notify({ EventType::Destroyed, id, "Unit destroyed" });
        }
    }

    void heal(int amount) { health.increase(amount); }

    int attackUnit(Unit& target, double defenseMultiplier = 1.0) {
        int rawDamage = getAttack();
        target.takeDamage(rawDamage, defenseMultiplier);
        return rawDamage;
    }

    virtual std::string getType() const = 0;
    virtual Unit* clone() const = 0;
};

class Damage : public Unit {
public:
    Damage(int id, int x, int y, int hp, int arm, int atk) : Unit(id, x, y, hp, arm, atk) {}
};

class Mage : public Damage {
public:
    Mage(int id, int x, int y) : Damage(id, x, y, 90, 5, 80) {}
    std::string getType() const override { return "Mage"; }
    Unit* clone() const override { return new Mage(*this); }
};

class Berserk : public Damage {
public:
    Berserk(int id, int x, int y) : Damage(id, x, y, 130, 15, 60) {}
    std::string getType() const override { return "Berserk"; }
    Unit* clone() const override { return new Berserk(*this); }
};

class Tank : public Unit {
public:
    Tank(int id, int x, int y, int hp, int arm, int atk) : Unit(id, x, y, hp, arm, atk) {}
};

class Paladin : public Tank {
public:
    Paladin(int id, int x, int y) : Tank(id, x, y, 350, 5, 25) {}
    std::string getType() const override { return "Paladin"; }
    Unit* clone() const override { return new Paladin(*this); }
};

class Vampir : public Tank {
public:
    Vampir(int id, int x, int y) : Tank(id, x, y, 200, 10, 20) {}
    std::string getType() const override { return "Vampir"; }
    Unit* clone() const override { return new Vampir(*this); }
};

class Flying : public Unit {
public:
    Flying(int id, int x, int y, int hp, int arm, int atk) : Unit(id, x, y, hp, arm, atk) {
        speed = 2;
        movesLeft = 2;
    }
};

class Pegas : public Flying {
public:
    Pegas(int id, int x, int y) : Flying(id, x, y, 170, 10, 60) {}
    std::string getType() const override { return "Pegas"; }
    Unit* clone() const override { return new Pegas(*this); }
};

class DarkKnight : public Flying {
public:
    DarkKnight(int id, int x, int y) : Flying(id, x, y, 200, 20, 50) {}
    std::string getType() const override { return "DarkKnight"; }
    Unit* clone() const override { return new DarkKnight(*this); }
};

class Archangel : public Flying {
public:
    Archangel(int id, int x, int y) : Flying(id, x, y, 400, 30, 120) {}
    std::string getType() const override { return "Archangel"; }
    Unit* clone() const override { return new Archangel(*this); }
};

class Titan : public Tank {
public:
    Titan(int id, int x, int y) : Tank(id, x, y, 800, 50, 80) {}
    std::string getType() const override { return "Titan"; }
    Unit* clone() const override { return new Titan(*this); }
};

class SpecialBattalion : public Unit {
public:
    SpecialBattalion(int id, int x, int y, int hp, int arm, int atk) : Unit(id, x, y, hp, arm, atk) {}
};

class Kobold : public SpecialBattalion {
public:
    Kobold(int id, int x, int y) : SpecialBattalion(id, x, y, 100, 5, 15) {}
    std::string getType() const override { return "Kobold"; }
    Unit* clone() const override { return new Kobold(*this); }
};

class BeaverMutant : public SpecialBattalion {
public:
    BeaverMutant(int id, int x, int y) : SpecialBattalion(id, x, y, 120, 10, 10) {}
    std::string getType() const override { return "BeaverMutant"; }
    Unit* clone() const override { return new BeaverMutant(*this); }
};
