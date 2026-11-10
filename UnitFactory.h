#pragma once
#include "Unit.h"
#include <string>

class UnitFactory {
public:
    virtual ~UnitFactory() = default;
    virtual Unit* createUnit(const std::string& type, int id, int x, int y) = 0;
};

class DamageFactory : public UnitFactory {
public:
    Unit* createUnit(const std::string& type, int id, int x, int y) override;
};

class TankFactory : public UnitFactory {
public:
    Unit* createUnit(const std::string& type, int id, int x, int y) override;
};

class FlyingFactory : public UnitFactory {
public:
    Unit* createUnit(const std::string& type, int id, int x, int y) override;
};

class AllUnitsFactory : public UnitFactory {
private:
    DamageFactory damage;
    TankFactory   tank;
    FlyingFactory fly;
public:
    Unit* createUnit(const std::string& type, int id, int x, int y) override;
};