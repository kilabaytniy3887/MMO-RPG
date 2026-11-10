#pragma once
#include <string>
#include <vector>
#include <iostream>

enum class EventType { Created, Damaged, Destroyed };

struct UnitEvent {
    EventType type;
    int unitId;
    std::string message;
};

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onUnitEvent(const UnitEvent& event) = 0;
};

class IObservable {
protected:
    std::vector<IObserver*> observers;
public:
    virtual ~IObservable() = default;
    void subscribe(IObserver* obs) { observers.push_back(obs); }
    void notify(const UnitEvent& event) {
        for (auto* obs : observers) obs->onUnitEvent(event);
    }
};