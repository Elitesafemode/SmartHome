
#pragma once
#include <Arduino.h>
#include "Pins.h"

class BuzzerManager {
private:
    bool buzzerActive = false;
    bool buzzerState = false;

    uint32_t lastToggleAt = 0;
    uint32_t intervalMs = 0;

    uint8_t remainingToggles = 0;
    uint8_t activePriority = 0;

    static constexpr uint16_t BUZZER_FREQUENCY = 2000;

    void buzzerOn() {
        tone(Pins::BUZZER, BUZZER_FREQUENCY);
        buzzerState = true;
    }

    void buzzerOff() {
        noTone(Pins::BUZZER);
        buzzerState = false;
    }

public:
    void begin() {
        pinMode(Pins::BUZZER, OUTPUT);
        noTone(Pins::BUZZER);

        buzzerActive = false;
        buzzerState = false;
        remainingToggles = 0;
        activePriority = 0;
    }

    // اولویت‌ها:
    // یادآوری درب = 1
    // هشدار امنیتی = 2
    // هشدار گاز = 3

    void startPattern(uint32_t interval, uint8_t toggles, uint8_t priority) {
        if (interval == 0 || toggles == 0) return;

        if (buzzerActive && priority <= activePriority) return;

        intervalMs = interval;
        remainingToggles = toggles;
        activePriority = priority;

        buzzerActive = true;
        lastToggleAt = millis();

        buzzerOn();
    }

    void shortBeep() {
        startPattern(100, 1, 1);
    }

    void tripleBeep() {
        startPattern(150, 5, 2);
    }

    void gasAlarm() {
        startPattern(120, 9, 3);
    }

    void update() {
        if (!buzzerActive) return;

        if (millis() - lastToggleAt < intervalMs) return;

        lastToggleAt = millis();

        if (buzzerState) {
            buzzerOff();
        } else {
            buzzerOn();
        }

        if (remainingToggles > 0) {
            remainingToggles--;
        }

        if (remainingToggles == 0) {
            stop();
        }
    }

    // هشدارهایی با اولویت بالاتر متوقف نمی‌شوند.
    void stop(uint8_t priorityLimit = 255) {
        if (buzzerActive && activePriority > priorityLimit) return;

        buzzerActive = false;
        remainingToggles = 0;
        activePriority = 0;

        buzzerOff();
    }

    bool isActive() const {
        return buzzerActive;
    }

    uint8_t getPriority() const {
        return activePriority;
    }
};
