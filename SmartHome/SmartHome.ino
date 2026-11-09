#include <Arduino.h>
#include <WiFi.h>

#include "Pins.h"
#include "Settings.h"
#include "Models.h"
#include "SensorManager.h"
#include "RelayManager.h"
#include "BuzzerManager.h"
#include "EventManager.h"
#include "SecurityManager.h"
#include "AutomationEngine.h"
#include "AuthManager.h"
#include "ApiManager.h"

SensorManager sensorManager;
RelayManager relayManager;
BuzzerManager buzzerManager;
EventManager eventManager;
SecurityManager securityManager;
AutomationEngine automationEngine;
AuthManager authManager;
ApiManager apiManager;

void setup() {
    Serial.begin(115200);
    delay(300);

    relayManager.begin();
    buzzerManager.begin();

    sensorManager.begin();
    securityManager.begin();
    authManager.begin();

    eventManager.addEvent(
        Models::EventType::SYSTEM_START,
        "سیستم خانه هوشمند راه‌اندازی شد"
    );

    if (Settings::USE_CLOUD_MQTT) {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(Settings::AP_SSID, Settings::AP_PASSWORD);
        WiFi.begin(Settings::WIFI_STA_SSID, Settings::WIFI_STA_PASSWORD);

        Serial.println("Connecting to Wi-Fi for Cloud MQTT mode...");
        unsigned long startAttempt = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
            delay(500);
            Serial.print(".");
        }
        Serial.println();
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("Connected to Wi-Fi IP: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("Wi-Fi connection timeout. Will retry in background.");
        }
    } else {
        WiFi.mode(WIFI_AP);
        const bool apStarted = WiFi.softAP(
            Settings::AP_SSID,
            Settings::AP_PASSWORD
        );

        if (apStarted) {
            Serial.println();
            Serial.println("Smart Home access point started.");
            Serial.print("SSID: ");
            Serial.println(Settings::AP_SSID);
            Serial.print("Dashboard: http://");
            Serial.println(WiFi.softAPIP());
        } else {
            Serial.println("ERROR: Could not start Wi-Fi access point.");
        }
    }

    apiManager.begin(
        sensorManager,
        relayManager,
        buzzerManager,
        securityManager,
        automationEngine,
        eventManager,
        authManager
    );
}

void loop() {
    sensorManager.update(eventManager);

    automationEngine.update(
        sensorManager.getData(),
        relayManager,
        eventManager,
        buzzerManager
    );

    securityManager.update(
        sensorManager.getData(),
        eventManager,
        buzzerManager
    );

    buzzerManager.update();
    apiManager.update();

    delay(2);
}