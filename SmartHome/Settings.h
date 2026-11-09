#pragma once
#include <Arduino.h>

namespace Settings {
// فلگ سوییچ بین حالت لوکال و ابری
// false = فقط هات اسپات لوکال مثل قبل
// true = اتصال به اینترنت و بروکر MQTT رانفلر
constexpr bool USE_CLOUD_MQTT = true;

// مشخصات وای فای اینترنت برای حالت ابری
constexpr char WIFI_STA_SSID[] = "YOUR_WIFI_SSID";
constexpr char WIFI_STA_PASSWORD[] = "YOUR_WIFI_PASSWORD";

// مشخصات بروکر Mosquitto در رانفلر
constexpr char MQTT_HOST[] = "mosquitto-lyo-smarthome.runflare.cloud";
constexpr uint16_t MQTT_PORT = 1883;
constexpr char MQTT_USER[] = "mosquitto";
constexpr char MQTT_PASS[] = "fPViRAJDnqzluin35M6v";

// تاپیک های ارتباطی MQTT
constexpr char MQTT_TOPIC_REQ[] = "smarthome/api/request";
constexpr char MQTT_TOPIC_RES[] = "smarthome/api/response";
constexpr char MQTT_TOPIC_STATUS[] = "smarthome/status";
constexpr unsigned long MQTT_STATUS_INTERVAL_MS = 1500;

constexpr char AP_SSID[] = "SmartHome-Final";
constexpr char AP_PASSWORD[] = "12345678";
constexpr uint16_t WEB_SERVER_PORT = 80;

constexpr unsigned long SENSOR_READ_INTERVAL_MS = 2000;
constexpr unsigned long DHT_READ_INTERVAL_MS = 2000;
constexpr unsigned long PIR_WARMUP_MS = 30000;
constexpr unsigned long MQ2_WARMUP_MS = 60000;
constexpr unsigned long DHT_STALE_TIMEOUT_MS = 10000;

constexpr float FAN_ON_TEMP = 30.0f;
constexpr float FAN_OFF_TEMP = 28.0f;
constexpr float HEATING_ON_TEMP = 18.0f;
constexpr float HEATING_OFF_TEMP = 20.0f;

constexpr unsigned long DOOR_OPEN_WARNING_MS = 10000;
constexpr unsigned long DOOR_REMINDER_INTERVAL_MS = 30000;
constexpr unsigned long SECURITY_EVENT_DEBOUNCE_MS = 300;
constexpr unsigned long PIR_EVENT_COOLDOWN_MS = 2000;

constexpr unsigned long DASHBOARD_REFRESH_MS = 1500;

constexpr uint8_t MAX_LOGIN_ATTEMPTS = 5;
constexpr unsigned long LOGIN_LOCKOUT_MS = 60000;
constexpr unsigned long SESSION_TIMEOUT_MS = 30UL * 60UL * 1000UL;
constexpr uint8_t MAX_ACTIVE_SESSIONS = 8;
constexpr uint8_t MIN_PASSWORD_LENGTH = 10;

constexpr uint16_t MAX_EVENT_HISTORY = 100;
constexpr unsigned long RAIN_STABILITY_MS = 3000;

constexpr uint16_t RAIN_DRY_RAW = 4095;
constexpr uint16_t RAIN_WET_RAW = 0;

constexpr bool LDR_DARK_LEVEL = false;
}