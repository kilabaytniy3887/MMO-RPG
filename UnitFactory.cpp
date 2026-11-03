#include "UnitFactory.h"

Unit* DamageFactory::createUnit(const std::string& type, int x, int y) {
    if (type == "Mage") return new Mage(x, y);
    if (type == "Berserk") return new Berserk(x, y);
    return nullptr;
}

Unit* TankFactory::createUnit(const std::string& type, int x, int y) {
    if (type == "Paladin") return new Paladin(x, y);
    if (type == "Vampir") return new Vampir(x, y);
    return nullptr;
}

Unit* FlyingFactory::createUnit(const std::string& type, int x, int y) {
    if (type == "Pegas") return new Pegas(x, y);
    if (type == "Dark Knight") return new DarkKnight(x, y);
    return nullptr;
}