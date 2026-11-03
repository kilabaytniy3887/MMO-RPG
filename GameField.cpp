#include "GameField.h"
#include <iostream>
#include <cctype>
#include <utility>

GameField::GameField(int w, int h, int maxU)
    : width(w), height(h), maxUnits(maxU) {
}

GameField::~GameField() { clear(); }

void GameField::clear() {
    for (Unit* u : units) delete u;
    units.clear();
}

GameField::GameField(const GameField& other)
    : width(other.width), height(other.height), maxUnits(other.maxUnits) {
    units.reserve(other.units.size());
    for (Unit* u : other.units) {
        units.push_back(u->clone());
    }
}

GameField& GameField::operator=(const GameField& other) {
    if (this != &other) {
        GameField tmp(other);
        std::swap(width, tmp.width);
        std::swap(height, tmp.height);
        std::swap(maxUnits, tmp.maxUnits);
        units.swap(tmp.units);
    }
    return *this;
}

GameField::GameField(GameField&& other) noexcept
    : width(other.width),
    height(other.height),
    maxUnits(other.maxUnits),
    units(std::move(other.units)) {
}

GameField& GameField::operator=(GameField&& other) noexcept {
    if (this != &other) {
        clear();
        width = other.width;
        height = other.height;
        maxUnits = other.maxUnits;
        units = std::move(other.units);
    }
    return *this;
}

bool GameField::addUnit(Unit* unit) {
    if (!unit) return false;

    if (units.size() >= static_cast<std::size_t>(maxUnits)) {
        std::cout << "Превышен лимит юнитов на поле!\n";
        delete unit;
        return false;
    }
    if (unit->getX() < 0 || unit->getX() >= width ||
        unit->getY() < 0 || unit->getY() >= height) {
        std::cout << "Юнит вне границ поля!\n";
        delete unit;
        return false;
    }
    for (Unit* u : units) {
        if (u->getX() == unit->getX() && u->getY() == unit->getY()) {
            std::cout << "Клетка уже занята!\n";
            delete unit;
            return false;
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

Unit* GameField::getUnit(std::size_t index) const {
    return index < units.size() ? units[index] : nullptr;
}

void GameField::populateField(UnitFactory& factory,
    const std::string& type, int x, int y) {
    Unit* newUnit = factory.createUnit(type, x, y);
    if (!newUnit) {
        std::cout << "Фабрика не смогла создать юнит типа: " << type << "\n";
        return;
    }
    addUnit(newUnit);
}

void GameField::printField() const {
    std::vector<std::vector<char>> grid(
        height, std::vector<char>(width, ' '));

    for (Unit* u : units) {
        const int ux = u->getX();
        const int uy = u->getY();

        if (ux < 0 || ux >= width || uy < 0 || uy >= height) continue;

        char symbol = '?';
        const std::string type = u->getType();

        if (type == "Mage")        symbol = 'M';
        else if (type == "Berserk")     symbol = 'B';
        else if (type == "Paladin")     symbol = 'P';
        else if (type == "Vampir")      symbol = 'V';
        else if (type == "Pegas")       symbol = 'H';
        else if (type == "Dark Knight") symbol = 'D';
        else if (!type.empty()) {
            symbol = static_cast<char>(
                toupper(static_cast<unsigned char>(type[0])));
        }

        grid[uy][ux] = symbol;
    }

    std::cout << "  ";
    for (int i = 0; i < width; ++i) std::cout << "X ";
    std::cout << '\n';

    for (int i = 0; i < height; ++i) {
        std::cout << "X ";
        for (int j = 0; j < width; ++j) {
            std::cout << grid[i][j] << ' ';
        }
        std::cout << "X\n";
    }

    std::cout << "  ";
    for (int i = 0; i < width; ++i) std::cout << "X ";
    std::cout << '\n';
}