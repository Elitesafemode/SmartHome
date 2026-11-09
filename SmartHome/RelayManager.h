#pragma once
#include <Arduino.h>
#include "Pins.h"
#include "Models.h"

class RelayManager {
private:
    Models::RelayState state;

    void writeRelay(uint8_t pin, bool enabled) {
        digitalWrite(pin, enabled ? Pins::RELAY_ON : Pins::RELAY_OFF);
    }

public:
    void begin() {
        // پیش‌نویسی سطح خاموش قبل از خروجی‌کردن پایه‌ها؛ ایمنی سخت‌افزاری مستقل همچنان لازم است.
        digitalWrite(Pins::RELAY_FAN, Pins::RELAY_OFF);
        digitalWrite(Pins::RELAY_YARD, Pins::RELAY_OFF);
        digitalWrite(Pins::RELAY_GAS_VALVE, Pins::RELAY_OFF);
        digitalWrite(Pins::RELAY_HEATING, Pins::RELAY_OFF);

        pinMode(Pins::RELAY_FAN, OUTPUT);
        pinMode(Pins::RELAY_YARD, OUTPUT);
        pinMode(Pins::RELAY_GAS_VALVE, OUTPUT);
        pinMode(Pins::RELAY_HEATING, OUTPUT);
        allOff();
    }

    void setFan(bool enabled) {
        writeRelay(Pins::RELAY_FAN, enabled);
        state.fan = enabled;
    }

    void setYardLight(bool enabled) {
        writeRelay(Pins::RELAY_YARD, enabled);
        state.yardLight = enabled;
    }

    void setGasValve(bool enabled) {
        writeRelay(Pins::RELAY_GAS_VALVE, enabled);
        state.gasValve = enabled;
    }

    void setHeating(bool enabled) {
        writeRelay(Pins::RELAY_HEATING, enabled);
        state.heating = enabled;
    }

    void allOff() {
        setFan(false);
        setYardLight(false);
        setGasValve(false);
        setHeating(false);
    }

    const Models::RelayState& getState() const {
        return state;
    }
};
