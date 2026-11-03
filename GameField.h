#pragma once
#include <vector>
#include <string>
#include <cstddef>
#include "Unit.h"
#include "UnitFactory.h"

class GameField {
private:
    int width;
    int height;
    int maxUnits;
    std::vector<Unit*> units;

public:
    GameField(int w, int h, int maxU);
    ~GameField();

    GameField(const GameField& other);
    GameField& operator=(const GameField& other);
    GameField(GameField&& other) noexcept;
    GameField& operator=(GameField&& other) noexcept;

    bool addUnit(Unit* unit);
    void removeUnit(std::size_t index);
    Unit* getUnit(std::size_t index) const;

    std::size_t size() const { return units.size(); }
    bool empty() const { return units.empty(); }
    void clear();

    int getWidth()  const { return width; }
    int getHeight() const { return height; }

    void populateField(UnitFactory& factory, const std::string& type, int x, int y);
    void printField() const;

    class GameFieldIterator {
    private:
        const GameField* field;
        std::size_t currentIndex;
    public:
        GameFieldIterator(const GameField* f, std::size_t index)
            : field(f), currentIndex(index) {
        }

        bool operator!=(const GameFieldIterator& other) const {
            return currentIndex != other.currentIndex;
        }
        Unit* operator*() const { return field->units[currentIndex]; }
        GameFieldIterator& operator++() { ++currentIndex; return *this; }
    };

    GameFieldIterator begin() const { return GameFieldIterator(this, 0); }
    GameFieldIterator end()   const { return GameFieldIterator(this, units.size()); }
};