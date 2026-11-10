#include "Unit.h"

Unit::Unit(int id, int x, int y, int hp, int arm, int atk)
    : id(id), x(x), y(y), health(hp), armor(arm), attack(atk),
    tempDamageBoost(0), dmgMult(1), gold(0), speed(1), ownerBase(nullptr) {
}

Damage::Damage(int id, int x, int y, int hp, int arm, int atk)
    : Unit(id, x, y, hp, arm, atk) {
}

Tank::Tank(int id, int x, int y, int hp, int arm, int atk)
    : Unit(id, x, y, hp, arm, atk) {
}

Flying::Flying(int id, int x, int y, int hp, int arm, int atk)
    : Unit(id, x, y, hp, arm, atk) {
    speed = 2;
}


Mage::Mage(int id, int x, int y)
    : Damage(id, x, y, 90, 5, 80) {
}

Berserk::Berserk(int id, int x, int y)
    : Damage(id, x, y, 130, 15, 60) {
}

Paladin::Paladin(int id, int x, int y)
    : Tank(id, x, y, 350, 5, 25) {
}

Vampir::Vampir(int id, int x, int y)
    : Tank(id, x, y, 200, 10, 20) {
}

Pegas::Pegas(int id, int x, int y)
    : Flying(id, x, y, 170, 10, 60) {
}

DarkKnight::DarkKnight(int id, int x, int y)
    : Flying(id, x, y, 200, 20, 50) {
}