#pragma once
#include <iostream>

class Health {
private:
    int value;
    int maxValue;
public:
    Health(int v = 100) : value(v > 0 ? v : 0), maxValue(v > 0 ? v : 0) {}

    int getValue() const { return value; }
    int getMaxValue() const { return maxValue; }

    void decrease(int amount) {
        value -= amount;
        if (value < 0) value = 0;
    }

    void increase(int amount) {
        value += amount;
        if (value > maxValue) value = maxValue;
    }

    void increaseMax(int amount) {
        maxValue += amount;
        value += amount;
    }

    void healToMax() { value = maxValue; }
    bool isZero() const { return value == 0; }
};

class Armor {
private:
    int value;
public:
    Armor(int v = 0) : value(v > 0 ? v : 0) {}
    int getValue() const { return value; }
    void increase(int amount) { value += amount; }
    int reduceDamage(int damage) const {
        int reduced = damage - value;
        return reduced > 0 ? reduced : 0;
    }
};

class Attack {
private:
    int value;
public:
    Attack(int v = 10) : value(v > 0 ? v : 0) {}
    int getValue() const { return value; }
    void increase(int amount) { value += amount; }
    int calculateDamage(const Armor& targetArmor) const {
        return targetArmor.reduceDamage(value);
    }
};