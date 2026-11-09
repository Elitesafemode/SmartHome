#pragma once
#include <Arduino.h>

namespace Pins {
constexpr uint8_t WARNING_LED = 23;
constexpr uint8_t BUZZER = 15;

constexpr uint8_t DHT_DATA = 4;
constexpr uint8_t LDR = 35;
constexpr uint8_t RAIN = 32;
constexpr uint8_t MQ2 = 34;
constexpr uint8_t PIR = 27;

constexpr uint8_t MAIN_DOOR = 25;
constexpr uint8_t GARAGE_DOOR = 26;

constexpr uint8_t RELAY_FAN = 16;
constexpr uint8_t RELAY_YARD = 17;
constexpr uint8_t RELAY_GAS_VALVE = 18;
constexpr uint8_t RELAY_HEATING = 19;

constexpr uint8_t RELAY_ON = LOW;
constexpr uint8_t RELAY_OFF = HIGH;

constexpr uint8_t GAS_DETECTED_LEVEL = LOW;
constexpr uint8_t DOOR_OPEN_LEVEL = HIGH;
constexpr uint8_t PIR_MOTION_LEVEL = HIGH;
}
