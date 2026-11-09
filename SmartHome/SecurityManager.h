#pragma once
#include <Arduino.h>
#include "Pins.h"
#include "Settings.h"
#include "Models.h"
#include "EventManager.h"
#include "BuzzerManager.h"

class SecurityManager {
private:
    Models::SecurityMode currentMode = Models::SecurityMode::DISARMED;
    bool securityAlertActive = false;

    bool previousMainDoorOpen = false;
    bool previousGarageDoorOpen = false;
    bool previousPirMotion = false;
    bool firstUpdate = true;
    bool modeChanged = false;

    bool mainDoorWarningTriggered = false;
    bool garageDoorWarningTriggered = false;
    uint32_t mainDoorOpenedAt = 0;
    uint32_t garageDoorOpenedAt = 0;
    uint32_t lastMainDoorReminderAt = 0;
    uint32_t lastGarageDoorReminderAt = 0;

    void triggerSecurityAlert(const String& reason, EventManager& events, BuzzerManager& buzzer) {
        if (securityAlertActive) return;

        securityAlertActive = true;
        events.addEvent(Models::EventType::SECURITY_ALERT, "هشدار امنیتی فعال شد", reason);
        buzzer.tripleBeep();
        digitalWrite(Pins::WARNING_LED, HIGH);
    }

    void updateDoor(
        bool isOpen, bool& previousState, bool& warningTriggered,
        uint32_t& openedAt, uint32_t& lastReminderAt,
        const String& doorName, Models::EventType openedEvent,
        Models::EventType closedEvent, EventManager& events,
        BuzzerManager& buzzer, uint32_t now
    ) {
        if (isOpen != previousState) {
            previousState = isOpen;

            if (isOpen) {
                openedAt = now;
                lastReminderAt = 0;
                warningTriggered = false;
                events.addEvent(openedEvent, doorName + " باز شد");

                if (currentMode != Models::SecurityMode::DISARMED)
                    triggerSecurityAlert(doorName + " باز شد", events, buzzer);
            } else {
                warningTriggered = false;
                openedAt = 0;
                lastReminderAt = 0;
                events.addEvent(closedEvent, doorName + " بسته شد");
            }
        }

        if (!isOpen) return;

        if (!warningTriggered && now - openedAt >= Settings::DOOR_OPEN_WARNING_MS) {
            warningTriggered = true;
            lastReminderAt = now;
            events.addEvent(Models::EventType::DOOR_OPEN_WARNING,
                            doorName + " بیش از ۱۰ ثانیه باز مانده است");
            buzzer.shortBeep();
        } else if (warningTriggered &&
                   now - lastReminderAt >= Settings::DOOR_REMINDER_INTERVAL_MS) {
            lastReminderAt = now;
            buzzer.shortBeep();
        }
    }

public:
    void begin() {
        pinMode(Pins::WARNING_LED, OUTPUT);
        digitalWrite(Pins::WARNING_LED, LOW);
        currentMode = Models::SecurityMode::DISARMED;
        securityAlertActive = false;
        firstUpdate = true;
        modeChanged = false;
    }

    void setMode(Models::SecurityMode newMode, EventManager& events, BuzzerManager&) {
        if (newMode == currentMode) return;
        currentMode = newMode;
        modeChanged = true;
        events.addEvent(Models::EventType::SECURITY_MODE_CHANGED, "حالت امنیتی تغییر کرد");
        // هشدار فعال با تغییر حالت پاک نمی‌شود؛ فقط بازنشانی دستی مجاز است.
    }

    void resetAlert(EventManager& events, BuzzerManager& buzzer) {
        if (!securityAlertActive) return;
        securityAlertActive = false;
        digitalWrite(Pins::WARNING_LED, LOW);
        buzzer.stop(2); // هشدار گاز با اولویت بالاتر متوقف نمی‌شود.
        events.addEvent(Models::EventType::SECURITY_RESET, "هشدار امنیتی بازنشانی شد");
    }

    void update(const Models::SensorData& sensors, EventManager& events, BuzzerManager& buzzer) {
        const uint32_t now = millis();

        if (firstUpdate) {
            previousMainDoorOpen = sensors.mainDoorOpen;
            previousGarageDoorOpen = sensors.garageDoorOpen;
            previousPirMotion = sensors.pirMotion;
            if (sensors.mainDoorOpen) mainDoorOpenedAt = now;
            if (sensors.garageDoorOpen) garageDoorOpenedAt = now;
            firstUpdate = false;
        }

        updateDoor(sensors.mainDoorOpen, previousMainDoorOpen,
                   mainDoorWarningTriggered, mainDoorOpenedAt,
                   lastMainDoorReminderAt, "درب اصلی",
                   Models::EventType::MAIN_DOOR_OPENED,
                   Models::EventType::MAIN_DOOR_CLOSED,
                   events, buzzer, now);

        updateDoor(sensors.garageDoorOpen, previousGarageDoorOpen,
                   garageDoorWarningTriggered, garageDoorOpenedAt,
                   lastGarageDoorReminderAt, "درب پارکینگ",
                   Models::EventType::GARAGE_DOOR_OPENED,
                   Models::EventType::GARAGE_DOOR_CLOSED,
                   events, buzzer, now);

        if (modeChanged) {
            modeChanged = false;
            if (currentMode != Models::SecurityMode::DISARMED) {
                if (sensors.mainDoorOpen)
                    triggerSecurityAlert("درب اصلی هنگام فعال‌سازی سیستم باز بود", events, buzzer);
                if (sensors.garageDoorOpen)
                    triggerSecurityAlert("درب پارکینگ هنگام فعال‌سازی سیستم باز بود", events, buzzer);
                if (currentMode == Models::SecurityMode::AWAY &&
                    sensors.pirStatus == Models::SensorStatus::READY &&
                    sensors.pirMotion) {
                    triggerSecurityAlert("حرکت هنگام فعال‌سازی حالت Away تشخیص داده شد", events, buzzer);
                    events.addEvent(Models::EventType::PIR_MOTION, "حرکت هنگام فعال‌سازی Away تشخیص داده شد");
                }
            }
        }

        if (currentMode == Models::SecurityMode::AWAY &&
            sensors.pirStatus == Models::SensorStatus::READY &&
            sensors.pirMotion && !previousPirMotion) {
            triggerSecurityAlert("حرکت توسط سنسور PIR تشخیص داده شد", events, buzzer);
            events.addEvent(Models::EventType::PIR_MOTION, "حرکت توسط PIR تشخیص داده شد");
        }

        previousPirMotion = sensors.pirMotion;
        digitalWrite(Pins::WARNING_LED, securityAlertActive ? HIGH : LOW);
    }

    Models::SecurityMode getMode() const { return currentMode; }
    bool isAlertActive() const { return securityAlertActive; }
};
