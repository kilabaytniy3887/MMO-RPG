#include "GameField.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <utility>
#include <Windows.h>

GameField::GameField(int w, int h, int maxU)
    : width(w), height(h), maxUnits(maxU) {
    initTerrain(w, h);
}

GameField::~GameField() { clear(); }

void GameField::clear() {
    for (Unit* u : units) delete u;
    units.clear();
}

GameField::GameField(const GameField& other)
    : width(other.width), height(other.height), maxUnits(other.maxUnits),
    terrainMap(other.terrainMap) {
    units.reserve(other.units.size());
    for (Unit* u : other.units) units.push_back(u->clone());
}

GameField& GameField::operator=(const GameField& other) {
    if (this != &other) {
        GameField tmp(other);
        std::swap(width, tmp.width);
        std::swap(height, tmp.height);
        std::swap(maxUnits, tmp.maxUnits);
        units.swap(tmp.units);
        terrainMap.swap(tmp.terrainMap);
    }
    return *this;
}

GameField::GameField(GameField&& other) noexcept
    : width(other.width), height(other.height), maxUnits(other.maxUnits),
    units(std::move(other.units)),
    terrainMap(std::move(other.terrainMap)),
    objects(std::move(other.objects)) {
    other.units.clear();
}

GameField& GameField::operator=(GameField&& other) noexcept {
    if (this != &other) {
        clear();
        width = other.width;
        height = other.height;
        maxUnits = other.maxUnits;
        units = std::move(other.units);
        terrainMap = std::move(other.terrainMap);
        objects = std::move(other.objects);
        other.units.clear();
    }
    return *this;
}

bool GameField::addUnit(Unit* unit) {
    if (!unit) return false;
    if (units.size() >= static_cast<std::size_t>(maxUnits)) {
        std::cout << "Превышен лимит юнитов на поле!\n";
        delete unit; return false;
    }
    if (unit->getX() < 0 || unit->getX() >= width ||
        unit->getY() < 0 || unit->getY() >= height) {
        std::cout << "Юнит вне границ поля!\n";
        delete unit; return false;
    }
    for (Unit* u : units) {
        if (u->getX() == unit->getX() && u->getY() == unit->getY()) {
            std::cout << "Клетка уже занята!\n";
            delete unit; return false;
        }
    }
    units.push_back(unit);
    return true;
}

void GameField::removeUnit(std::size_t index) {
    if (index < units.size()) {
        delete units[index];
        units.erase(units.begin() + static_cast<std::ptrdiff_t>(index));
    }
}

bool GameField::moveUnit(std::size_t index, int newX, int newY) {
    if (index >= units.size()) return false;
    if (newX < 0 || newX >= width || newY < 0 || newY >= height) return false;
    for (std::size_t i = 0; i < units.size(); ++i) {
        if (i == index) continue;
        Unit* other = units[i];
        if (other->getX() == newX && other->getY() == newY) return false;
    }
    units[index]->move(newX, newY);
    return true;
}

Unit* GameField::getUnit(std::size_t index) const {
    return index < units.size() ? units[index] : nullptr;
}

void GameField::populateField(UnitFactory& factory, const std::string& type,
    int id, int x, int y) {
    Unit* newUnit = factory.createUnit(type, id, x, y);
    if (!newUnit) {
        std::cout << "Фабрика не смогла создать юнит типа: " << type << "\n";
        return;
    }
    if (x >= 0 && x < width && y >= 0 && y < height) {
        if (!terrainMap[y][x]->isPassable(*newUnit)) {
            std::cout << "Ошибка: " << type << " не может находиться на этом ландшафте!\n";
            delete newUnit; return;
        }
    }
    addUnit(newUnit);
}

void GameField::initTerrain(int w, int h) {
    width = w; height = h;
    terrainMap.assign(h, std::vector<std::shared_ptr<Terrain>>(w, std::make_shared<Plains>()));
}

void GameField::setTerrain(int x, int y, std::shared_ptr<Terrain> t) {
    if (y >= 0 && y < height && x >= 0 && x < width) terrainMap[y][x] = t;
}

TerrainProxy GameField::getTerrainProxy(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return TerrainProxy(std::make_shared<Plains>());
    }
    return TerrainProxy(terrainMap[y][x]);
}

void GameField::addObject(int x, int y, std::unique_ptr<NeutralObject> obj) {
    objects[{x, y}] = std::move(obj);
}

NeutralObject* GameField::getObjectAt(int x, int y) const {
    auto it = objects.find({ x, y });
    return (it != objects.end()) ? it->second.get() : nullptr;
}

void GameField::removeObject(int x, int y) {
    objects.erase({ x, y });
}

void GameField::printField() const {

}

void GameField::printMap(int heroX, int heroY, std::string heroType) const {
    const char* RESET = "\033[0m";
    const char* GRAY = "\033[90m";
    const char* ORANGE = "\033[38;5;208m";
    const char* GREEN = "\033[32m";
    const char* YELLOW = "\033[93m";
    const char* WHITE = "\033[97m";
    const char* PURPLE = "\033[95m";
    const char* RED = "\033[91m";
    const char* BROWN = "\033[38;5;130m";

    std::cout << "    ";
    for (int j = 0; j < width; ++j) {
        std::cout << (j % 10) << " ";
    }
    std::cout << "\n";

    std::cout << "    ";
    for (int j = 0; j < width; ++j) {
        std::cout << "X ";
    }
    std::cout << "\n";

    for (int i = 0; i < height; ++i) {
        std::cout << (i % 10) << " X ";

        for (int j = 0; j < width; ++j) {
            Unit* uHere = nullptr;
            for (Unit* u : units) {
                if (u->getX() == j && u->getY() == i) {
                    uHere = u; break;
                }
            }

            if (uHere) {
                char sym = '?';
                if (uHere->getType() == "Mage") sym = 'M';
                else if (uHere->getType() == "Berserk") sym = 'B';
                else if (uHere->getType() == "Paladin") sym = 'P';
                else if (uHere->getType() == "Vampir") sym = 'V';
                else if (uHere->getType() == "Pegas") sym = 'H';
                else if (uHere->getType() == "Dark Knight") sym = 'D';
                std::cout << WHITE << sym << RESET << " ";
            }
            else if (getObjectAt(j, i) && getObjectAt(j, i)->getSymbol() != ' ') {
                NeutralObject* obj = getObjectAt(j, i);
                if (obj->getColor() == 13) std::cout << PURPLE << obj->getSymbol() << RESET << " ";
                else if (obj->getColor() == 12) std::cout << RED << obj->getSymbol() << RESET << " ";
                else if (obj->getColor() == 6) std::cout << BROWN << obj->getSymbol() << RESET << " ";
                else std::cout << obj->getSymbol() << " ";
            }
            else if (i == heroY && j == heroX) {
                char sym = '?';
                if (heroType == "Mage") sym = 'M';
                else if (heroType == "Berserk") sym = 'B';
                else if (heroType == "Paladin") sym = 'P';
                else if (heroType == "Vampir") sym = 'V';
                else if (heroType == "Pegas") sym = 'H';
                else if (heroType == "Dark Knight") sym = 'D';
                std::cout << WHITE << sym << RESET << " ";
            }
            else if (terrainMap[i][j]) {
                std::string tName = terrainMap[i][j]->getName();
                if (tName == "Forest") std::cout << ORANGE << "Y" << RESET << " ";
                else if (tName == "Mountains") std::cout << GRAY << "*" << RESET << " ";
                else if (tName == "Swamp") std::cout << GREEN << "*" << RESET << " ";
                else std::cout << YELLOW << "*" << RESET << " ";
            }
            else {
                std::cout << "  ";
            }
        }
        std::cout << "X\n";
    }

    std::cout << "    ";
    for (int j = 0; j < width; ++j) {
        std::cout << "X ";
    }
    std::cout << "\n";
}