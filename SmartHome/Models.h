#pragma once
#include <Arduino.h>

namespace Models {
enum class SensorStatus : uint8_t { INITIALIZING, READY, ERROR, DISCONNECTED };
enum class SecurityMode : uint8_t { DISARMED, HOME, AWAY };
enum class RainLevel : uint8_t { CLEAR, HUMID, LIGHT_RAIN, RAINY, HEAVY_RAIN, STORM };

enum class EventType : uint8_t {
    SYSTEM_START, SYSTEM_RESTART,
    GAS_DETECTED, GAS_CLEARED,
    PIR_MOTION, SECURITY_ALERT, SECURITY_RESET,
    MAIN_DOOR_OPENED, MAIN_DOOR_CLOSED,
    GARAGE_DOOR_OPENED, GARAGE_DOOR_CLOSED,
    DOOR_OPEN_WARNING,
    RAIN_STARTED, RAIN_STOPPED, RAIN_LEVEL_CHANGED,
    SENSOR_ERROR, SENSOR_RECOVERED,
    RELAY_CHANGED, SMART_MODE_CHANGED, SECURITY_MODE_CHANGED,
    LOGIN_SUCCESS, LOGIN_FAILED, LOGIN_BLOCKED, LOGOUT,
    SESSION_EXPIRED, SESSION_REVOKED, PASSWORD_CHANGED,
    INITIAL_SETUP_COMPLETED
};

struct SensorData {
    float temperature = 0.0f;
    float humidity = 0.0f;
    bool pirMotion = false;
    bool gasDetected = false;
    bool mainDoorOpen = false;
    bool garageDoorOpen = false;
    bool rainDetected = false;
    bool ldrState = false;
    uint16_t rainRaw = 0;
    uint8_t rainPercent = 0;
    RainLevel rainLevel = RainLevel::CLEAR;
    SensorStatus dhtStatus = SensorStatus::INITIALIZING;
    SensorStatus pirStatus = SensorStatus::INITIALIZING;
    SensorStatus mq2Status = SensorStatus::INITIALIZING;
    SensorStatus rainStatus = SensorStatus::INITIALIZING;
    SensorStatus ldrStatus = SensorStatus::INITIALIZING;
};

struct RelayState {
    bool fan = false;
    bool yardLight = false;
    bool gasValve = false;
    bool heating = false;
};

struct EventRecord {
    uint32_t id = 0;
    uint32_t uptimeSeconds = 0;
    EventType type = EventType::SYSTEM_START;
    String message;
    String details;
};

struct SystemState {
    SensorData sensors;
    RelayState relays;
    SecurityMode securityMode = SecurityMode::DISARMED;
    bool smartModeEnabled = false;
    bool gasAlertActive = false;
    bool securityAlertActive = false;
    uint32_t uptimeSeconds = 0;
};

struct AutomationSettings {
    float fanOnTemperature = 30.0f;
    float fanOffTemperature = 28.0f;
    float heatingOnTemperature = 18.0f;
    float heatingOffTemperature = 20.0f;
    bool smartModeEnabled = false;
    bool notificationsEnabled = true;
    bool ldrDarkLevel = false;
};

struct LoginState {
    uint8_t failedAttempts = 0;
    bool loginBlocked = false;
    uint32_t lockoutStartedAt = 0;
};

struct Session {
    String token;
    String deviceInfo;
    uint32_t lastActivity = 0;
    bool active = false;
};
}
