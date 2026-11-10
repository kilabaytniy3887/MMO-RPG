#pragma once
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cstddef>
#include "Unit.h"
#include "UnitFactory.h"
#include "Terrain.h"
#include "NeutralObject.h"

class GameField {
private:
    int width;
    int height;
    int maxUnits;
    std::vector<Unit*> units;
    std::vector<std::vector<std::shared_ptr<Terrain>>> terrainMap;
    std::map<std::pair<int, int>, std::unique_ptr<NeutralObject>> objects;

public:
    GameField(int w = 10, int h = 10, int maxU = 10);
    ~GameField();

    GameField(const GameField& other);
    GameField& operator=(const GameField& other);
    GameField(GameField&& other) noexcept;
    GameField& operator=(GameField&& other) noexcept;

    bool addUnit(Unit* unit);
    void removeUnit(std::size_t index);
    bool moveUnit(std::size_t index, int newX, int newY);
    Unit* getUnit(std::size_t index) const;

    std::size_t size() const { return units.size(); }
    bool empty() const { return units.empty(); }
    void clear();

    int getWidth()  const { return width; }
    int getHeight() const { return height; }

    void populateField(UnitFactory& factory, const std::string& type,
        int id, int x, int y);
    void printField() const;
    void printMap(int heroX = -1, int heroY = -1, std::string heroType = "") const;

    void initTerrain(int w, int h);
    void setTerrain(int x, int y, std::shared_ptr<Terrain> t);
    TerrainProxy getTerrainProxy(int x, int y);

    void addObject(int x, int y, std::unique_ptr<NeutralObject> obj);
    NeutralObject* getObjectAt(int x, int y) const;
    void removeObject(int x, int y);

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