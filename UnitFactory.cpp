#include "UnitFactory.h"

Unit* DamageFactory::createUnit(const std::string& type, int id, int x, int y) {
    if (type == "Mage")    return new Mage(id, x, y);
    if (type == "Berserk") return new Berserk(id, x, y);
    return nullptr;
}

Unit* TankFactory::createUnit(const std::string& type, int id, int x, int y) {
    if (type == "Paladin") return new Paladin(id, x, y);
    if (type == "Vampir")  return new Vampir(id, x, y);
    if (type == "Titan")   return new Titan(id, x, y);
    return nullptr;
}

Unit* FlyingFactory::createUnit(const std::string& type, int id, int x, int y) {
    if (type == "Pegas")       return new Pegas(id, x, y);
    if (type == "DarkKnight")  return new DarkKnight(id, x, y);
    if (type == "Archangel")   return new Archangel(id, x, y);
    return nullptr;
}

Unit* AllUnitsFactory::createUnit(const std::string& type, int id, int x, int y) {
    if (Unit* u = damage.createUnit(type, id, x, y)) return u;
    if (Unit* u = tank.createUnit(type, id, x, y))   return u;
    if (Unit* u = fly.createUnit(type, id, x, y))    return u;

    if (type == "Kobold")        return new Kobold(id, x, y);
    if (type == "BeaverMutant")  return new BeaverMutant(id, x, y);

    return nullptr;
}