#pragma once
#include <Arduino.h>
#include "Models.h"
#include "Settings.h"

class EventManager {
private:
    Models::EventRecord events[Settings::MAX_EVENT_HISTORY];
    uint16_t nextIndex = 0;
    uint16_t eventCount = 0;
    uint32_t nextEventId = 1;

public:
    void addEvent(Models::EventType type, const String& message, const String& details = "") {
        Models::EventRecord& event = events[nextIndex];
        event.id = nextEventId++;
        event.uptimeSeconds = millis() / 1000;
        event.type = type;
        event.message = message;
        event.details = details;

        nextIndex = (nextIndex + 1) % Settings::MAX_EVENT_HISTORY;
        if (eventCount < Settings::MAX_EVENT_HISTORY) eventCount++;
    }

    uint16_t getCount() const { return eventCount; }

    const Models::EventRecord* getEventAt(uint16_t index) const {
        if (index >= eventCount) return nullptr;
        int32_t actualIndex = static_cast<int32_t>(nextIndex) - 1 - index;
        while (actualIndex < 0) actualIndex += Settings::MAX_EVENT_HISTORY;
        return &events[actualIndex];
    }

    void clear() {
        nextIndex = 0;
        eventCount = 0;
        nextEventId = 1;
    }
};
