#pragma once
#include <Arduino.h>
#include <DHT.h>
#include "Pins.h"
#include "Settings.h"
#include "Models.h"
#include "EventManager.h"

class SensorManager {
private:
    DHT dht;
    Models::SensorData data;

    uint32_t startedAt = 0;
    uint32_t lastSensorRead = 0;
    uint32_t lastDhtRead = 0;
    uint32_t lastDhtValidAt = 0;

    uint32_t doorMainChangedAt = 0;
    uint32_t doorGarageChangedAt = 0;
    bool rawMainDoor = false;
    bool rawGarageDoor = false;
    bool stableMainDoor = false;
    bool stableGarageDoor = false;

    Models::RainLevel candidateRainLevel = Models::RainLevel::CLEAR;
    uint32_t rainCandidateSince = 0;
    bool rainCandidateInitialized = false;
    bool rainStableInitialized = false;
    bool initialized = false;

    static uint8_t rawToPercent(uint16_t raw) {
        const int32_t dry = Settings::RAIN_DRY_RAW;
        const int32_t wet = Settings::RAIN_WET_RAW;
        if (dry == wet) return 0;
        int32_t value = ((dry - static_cast<int32_t>(raw)) * 100) / (dry - wet);
        return static_cast<uint8_t>(constrain(value, 0, 100));
    }

    static Models::RainLevel percentToLevel(uint8_t percent) {
        if (percent <= 5) return Models::RainLevel::CLEAR;
        if (percent <= 15) return Models::RainLevel::HUMID;
        if (percent <= 25) return Models::RainLevel::LIGHT_RAIN;
        if (percent <= 50) return Models::RainLevel::RAINY;
        if (percent <= 70) return Models::RainLevel::HEAVY_RAIN;
        return Models::RainLevel::STORM;
    }

    static const char* rainLevelName(Models::RainLevel level) {
        switch (level) {
            case Models::RainLevel::CLEAR: return "clear";
            case Models::RainLevel::HUMID: return "humid";
            case Models::RainLevel::LIGHT_RAIN: return "light_rain";
            case Models::RainLevel::RAINY: return "rainy";
            case Models::RainLevel::HEAVY_RAIN: return "heavy_rain";
            case Models::RainLevel::STORM: return "storm";
            default: return "unknown";
        }
    }

    void updateDoors(uint32_t now) {
        const bool mainRaw = digitalRead(Pins::MAIN_DOOR) == Pins::DOOR_OPEN_LEVEL;
        const bool garageRaw = digitalRead(Pins::GARAGE_DOOR) == Pins::DOOR_OPEN_LEVEL;

        if (mainRaw != rawMainDoor) {
            rawMainDoor = mainRaw;
            doorMainChangedAt = now;
        }
        if (garageRaw != rawGarageDoor) {
            rawGarageDoor = garageRaw;
            doorGarageChangedAt = now;
        }

        if (now - doorMainChangedAt >= Settings::SECURITY_EVENT_DEBOUNCE_MS)
            stableMainDoor = rawMainDoor;
        if (now - doorGarageChangedAt >= Settings::SECURITY_EVENT_DEBOUNCE_MS)
            stableGarageDoor = rawGarageDoor;

        data.mainDoorOpen = stableMainDoor;
        data.garageDoorOpen = stableGarageDoor;
    }

    void updateRain(uint32_t now, EventManager& events) {
        data.rainRaw = analogRead(Pins::RAIN);
        data.rainPercent = rawToPercent(data.rainRaw);

        const Models::RainLevel newCandidate = percentToLevel(data.rainPercent);
        if (!rainCandidateInitialized || newCandidate != candidateRainLevel) {
            candidateRainLevel = newCandidate;
            rainCandidateSince = now;
            rainCandidateInitialized = true;
        }

        if (now - rainCandidateSince < Settings::RAIN_STABILITY_MS) {
            if (!rainStableInitialized) data.rainStatus = Models::SensorStatus::INITIALIZING;
            return;
        }

        const Models::RainLevel oldLevel = data.rainLevel;
        if (!rainStableInitialized) {
            data.rainLevel = candidateRainLevel;
            rainStableInitialized = true;
            data.rainStatus = Models::SensorStatus::READY;
            data.rainDetected = data.rainLevel != Models::RainLevel::CLEAR;
            return;
        }

        if (candidateRainLevel != oldLevel) {
            data.rainLevel = candidateRainLevel;
            data.rainDetected = data.rainLevel != Models::RainLevel::CLEAR;

            if (oldLevel == Models::RainLevel::CLEAR && data.rainDetected) {
                events.addEvent(Models::EventType::RAIN_STARTED, "رطوبت یا بارندگی تشخیص داده شد",
                                String(rainLevelName(data.rainLevel)));
            } else if (oldLevel != Models::RainLevel::CLEAR &&
                       data.rainLevel == Models::RainLevel::CLEAR) {
                events.addEvent(Models::EventType::RAIN_STOPPED, "سطح باران به حالت خشک برگشت");
            } else {
                events.addEvent(Models::EventType::RAIN_LEVEL_CHANGED, "سطح بارندگی تغییر کرد",
                                String(rainLevelName(data.rainLevel)));
            }
        }
        data.rainStatus = Models::SensorStatus::READY;
    }

public:
    SensorManager() : dht(Pins::DHT_DATA, DHT22) {}

    void begin() {
        const uint32_t now = millis();

        pinMode(Pins::LDR, INPUT);
        pinMode(Pins::RAIN, INPUT);
        pinMode(Pins::MQ2, INPUT);
        pinMode(Pins::PIR, INPUT_PULLDOWN);
        pinMode(Pins::MAIN_DOOR, INPUT_PULLUP);
        pinMode(Pins::GARAGE_DOOR, INPUT_PULLUP);

        dht.begin();

        startedAt = now;
        lastSensorRead = now - Settings::SENSOR_READ_INTERVAL_MS;
        lastDhtRead = now - Settings::DHT_READ_INTERVAL_MS;
        lastDhtValidAt = now;

        rawMainDoor = digitalRead(Pins::MAIN_DOOR) == Pins::DOOR_OPEN_LEVEL;
        rawGarageDoor = digitalRead(Pins::GARAGE_DOOR) == Pins::DOOR_OPEN_LEVEL;
        stableMainDoor = rawMainDoor;
        stableGarageDoor = rawGarageDoor;
        data.mainDoorOpen = stableMainDoor;
        data.garageDoorOpen = stableGarageDoor;
        doorMainChangedAt = now;
        doorGarageChangedAt = now;

        data.pirStatus = Models::SensorStatus::INITIALIZING;
        data.mq2Status = Models::SensorStatus::INITIALIZING;
        data.dhtStatus = Models::SensorStatus::INITIALIZING;
        data.rainStatus = Models::SensorStatus::INITIALIZING;
        data.ldrStatus = Models::SensorStatus::INITIALIZING;
        initialized = true;
    }

    void update(EventManager& events) {
        if (!initialized) return;

        const uint32_t now = millis();
        updateDoors(now);

        if (now - lastSensorRead < Settings::SENSOR_READ_INTERVAL_MS) return;
        lastSensorRead = now;

        if (now - startedAt >= Settings::PIR_WARMUP_MS) {
            data.pirMotion = digitalRead(Pins::PIR) == Pins::PIR_MOTION_LEVEL;
            data.pirStatus = Models::SensorStatus::READY;
        } else {
            data.pirMotion = false;
            data.pirStatus = Models::SensorStatus::INITIALIZING;
        }

        if (now - startedAt >= Settings::MQ2_WARMUP_MS) {
            data.gasDetected = digitalRead(Pins::MQ2) == Pins::GAS_DETECTED_LEVEL;
            data.mq2Status = Models::SensorStatus::READY;
        } else {
            data.gasDetected = false;
            data.mq2Status = Models::SensorStatus::INITIALIZING;
        }

        data.ldrState = digitalRead(Pins::LDR);
        data.ldrStatus = Models::SensorStatus::READY;

        updateRain(now, events);

        if (now - lastDhtRead >= Settings::DHT_READ_INTERVAL_MS) {
            lastDhtRead = now;
            const float temperature = dht.readTemperature();
            const float humidity = dht.readHumidity();

            if (!isnan(temperature) && !isnan(humidity) &&
                temperature >= -40.0f && temperature <= 80.0f &&
                humidity >= 0.0f && humidity <= 100.0f) {
                data.temperature = temperature;
                data.humidity = humidity;
                data.dhtStatus = Models::SensorStatus::READY;
                lastDhtValidAt = now;
            } else if (now - lastDhtValidAt >= Settings::DHT_STALE_TIMEOUT_MS) {
                data.dhtStatus = Models::SensorStatus::ERROR;
            }
        }
    }

    const Models::SensorData& getData() const { return data; }
    bool isInitialized() const { return initialized; }
};
