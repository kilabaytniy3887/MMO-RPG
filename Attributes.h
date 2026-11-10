#pragma once
#include <iostream>

class Health {
private:
    int value;
public:
    Health(int v) : value(v > 0 ? v : 0) {}
    int getValue() const { return value; }
    void decrease(int amount) {
        value -= amount;
        if (value < 0) value = 0;
    }
    void increase(int amount) {
        value += amount;
    }
    bool isZero() const { return value == 0; }
};

class Armor {
private:
    int value;
public:
    Armor(int v) : value(v > 0 ? v : 0) {}
    int getValue() const { return value; }
    int reduceDamage(int damage) const {
        int reduced = damage - value;
        return reduced > 0 ? reduced : 0;
    }
};

class Attack {
private:
    int value;
public:
    Attack(int v) : value(v > 0 ? v : 0) {}
    int getValue() const { return value; }
    int calculateDamage(const Armor& targetArmor) const {
        return targetArmor.reduceDamage(value);
    }
};