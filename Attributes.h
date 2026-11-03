#pragma once

class Health {
private:
    int value;
public:
    explicit Health(int v = 0) : value(v > 0 ? v : 0) {}

    int getValue() const { return value; }

    void decrease(int amount) {
        if (amount <= 0) return;
        value -= amount;
        if (value < 0) value = 0;
    }
    void increase(int amount) {
        if (amount > 0) value += amount;
    }
    bool isZero() const { return value == 0; }
};

class Armor {
private:
    int value;
public:
    explicit Armor(int v = 0) : value(v > 0 ? v : 0) {}

    int getValue() const { return value; }

    int reduceDamage(int damage) const {
        if (damage <= 0) return 0;
        int reduced = damage - value;
        return reduced > 0 ? reduced : 0;
    }
};

class Attack {
private:
    int value;
public:
    explicit Attack(int v = 0) : value(v > 0 ? v : 0) {}

    int getValue() const { return value; }

    int calculateDamage(const Armor& targetArmor) const {
        return targetArmor.reduceDamage(value);
    }
};