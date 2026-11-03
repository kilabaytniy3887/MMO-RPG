#pragma once
#include "Unit.h"
#include <string>

// ОБОСНОВАНИЕ ПАТТЕРНА ФАБРИЧНЫЙ МЕТОД
// Абстрактная фабрика определяет общий интерфейс для создания объектов.
// Это позволяет классу GameField не зависеть от конкретных классов юнитов (Mage, Paladin и т.д.).
// Если мы захотим добавить новый тип (например, "Лучник"), нам не придется менять код GameField,
// мы просто создадим новую фабрику. Это соблюдает принцип OCP.

class UnitFactory {
public:
    virtual ~UnitFactory() = default;
    virtual Unit* createUnit(const std::string& type, int x, int y) = 0;
};

class DamageFactory : public UnitFactory {
public:
    Unit* createUnit(const std::string& type, int x, int y) override;
};

class TankFactory : public UnitFactory {
public:
    Unit* createUnit(const std::string& type, int x, int y) override;
};

class FlyingFactory : public UnitFactory {
public:
    Unit* createUnit(const std::string& type, int x, int y) override;
};