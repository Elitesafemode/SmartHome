#pragma once
#include <Arduino.h>
#include "Models.h"
#include "Settings.h"
#include "RelayManager.h"
#include "EventManager.h"
#include "BuzzerManager.h"

class AutomationEngine {
private:
    Models::AutomationSettings config;
    bool gasValveLockedClosed = true;
    bool gasWasDetected = false;
    bool smartModeEnabled = false;
    bool gasAlertActive = false;

    void logSmartMode(bool enabled, EventManager& events) {
        events.addEvent(Models::EventType::SMART_MODE_CHANGED,
                        enabled ? "حالت هوشمند فعال شد" : "حالت هوشمند غیرفعال شد");
    }

    void updateFan(float temperature, Models::SensorStatus status, RelayManager& relays) {
        if (status != Models::SensorStatus::READY) {
            relays.setFan(false);
            return;
        }
        if (!relays.getState().fan && temperature >= config.fanOnTemperature)
            relays.setFan(true);
        else if (relays.getState().fan && temperature <= config.fanOffTemperature)
            relays.setFan(false);
    }

    void updateHeating(float temperature, Models::SensorStatus status, RelayManager& relays) {
        if (status != Models::SensorStatus::READY) {
            relays.setHeating(false);
            return;
        }
        if (!relays.getState().heating && temperature <= config.heatingOnTemperature)
            relays.setHeating(true);
        else if (relays.getState().heating && temperature >= config.heatingOffTemperature)
            relays.setHeating(false);
    }

    void updateYardLight(bool ldrRaw, Models::SensorStatus status, RelayManager& relays) {
        if (status != Models::SensorStatus::READY) {
            relays.setYardLight(false);
            return;
        }
        relays.setYardLight(ldrRaw == config.ldrDarkLevel);
    }

    void updateGasSafety(const Models::SensorData& sensors, RelayManager& relays,
                         EventManager& events, BuzzerManager& buzzer) {
        if (sensors.mq2Status != Models::SensorStatus::READY) {
            if (relays.getState().gasValve) {
                relays.setGasValve(false);
                events.addEvent(Models::EventType::GAS_DETECTED,
                                "شیر گاز به دلیل نامعتبر بودن وضعیت سنسور بسته شد");
            }
            gasValveLockedClosed = true;
            return;
        }

        if (sensors.gasDetected) {
            gasValveLockedClosed = true;
            relays.setGasValve(false);
            gasAlertActive = true;

            if (!gasWasDetected) {
                events.addEvent(Models::EventType::GAS_DETECTED,
                                "گاز تشخیص داده شد؛ شیر گاز بسته شد");
                buzzer.gasAlarm();
            }
            gasWasDetected = true;
            return;
        }

        if (gasWasDetected) {
            gasWasDetected = false;
            gasAlertActive = false;
            gasValveLockedClosed = true;
            events.addEvent(Models::EventType::GAS_CLEARED,
                            "تشخیص گاز متوقف شد؛ شیر همچنان بسته است");
        }
    }

public:
    AutomationEngine() {
        config.fanOnTemperature = Settings::FAN_ON_TEMP;
        config.fanOffTemperature = Settings::FAN_OFF_TEMP;
        config.heatingOnTemperature = Settings::HEATING_ON_TEMP;
        config.heatingOffTemperature = Settings::HEATING_OFF_TEMP;
        config.ldrDarkLevel = Settings::LDR_DARK_LEVEL;
        config.smartModeEnabled = false;
        config.notificationsEnabled = true;
    }

    void setSmartMode(bool enabled, EventManager& events) {
        if (smartModeEnabled == enabled) return;
        smartModeEnabled = enabled;
        config.smartModeEnabled = enabled;
        logSmartMode(enabled, events);
    }

    bool isSmartModeEnabled() const { return smartModeEnabled; }
    bool isGasValveLocked() const { return gasValveLockedClosed; }
    bool isGasAlertActive() const { return gasAlertActive; }

    bool setSettings(const Models::AutomationSettings& newSettings) {
        if (newSettings.fanOffTemperature >= newSettings.fanOnTemperature) return false;
        if (newSettings.heatingOnTemperature <= newSettings.heatingOffTemperature) return false;
        config = newSettings;
        smartModeEnabled = newSettings.smartModeEnabled;
        return true;
    }

    const Models::AutomationSettings& getSettings() const { return config; }

    bool requestGasValveOpen(bool userConfirmed, const Models::SensorData& sensors,
                             RelayManager& relays, EventManager& events) {
        if (!userConfirmed || sensors.mq2Status != Models::SensorStatus::READY ||
            sensors.gasDetected || gasAlertActive) return false;

        gasValveLockedClosed = false;
        relays.setGasValve(true);
        events.addEvent(Models::EventType::RELAY_CHANGED,
                        "فرمان بازگشایی دستی شیر گاز صادر شد");
        return true;
    }

    void update(const Models::SensorData& sensors, RelayManager& relays,
                EventManager& events, BuzzerManager& buzzer) {
        updateGasSafety(sensors, relays, events, buzzer);

        if (!smartModeEnabled) return;

        updateFan(sensors.temperature, sensors.dhtStatus, relays);
        updateHeating(sensors.temperature, sensors.dhtStatus, relays);
        updateYardLight(sensors.ldrState, sensors.ldrStatus, relays);
    }
};
