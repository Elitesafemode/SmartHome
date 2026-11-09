#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "Settings.h"
#include "Models.h"
#include "SensorManager.h"
#include "RelayManager.h"
#include "BuzzerManager.h"
#include "SecurityManager.h"
#include "AutomationEngine.h"
#include "EventManager.h"
#include "AuthManager.h"
#include "WebDashboard.h"

class ApiManager {
private:
    WebServer server;
    WiFiClient wifiClient;
    PubSubClient mqttClient;
    unsigned long lastMqttReconnectAttempt = 0;

    SensorManager* sensors = nullptr;
    RelayManager* relays = nullptr;
    BuzzerManager* buzzer = nullptr;
    SecurityManager* security = nullptr;
    AutomationEngine* automation = nullptr;
    EventManager* events = nullptr;
    AuthManager* auth = nullptr;

    String activeToken = "";
    String activeBody = "";
    bool isMqttContext = false;
    int lastStatusCode = 200;
    String lastResponseJson = "";

    String getToken() {
        if (isMqttContext) return activeToken;
        if (!server.hasHeader("Authorization")) return "";
        const String header = server.header("Authorization");
        return header.startsWith("Bearer ") ? header.substring(7) : "";
    }

    void sendJson(JsonDocument& doc, int code = 200) {
        String output;
        serializeJson(doc, output);
        if (isMqttContext) {
            lastStatusCode = code;
            lastResponseJson = output;
        } else {
            server.send(code, "application/json; charset=utf-8", output);
        }
    }

    void sendError(int code, const char* message) {
        JsonDocument response;
        response["success"] = false;
        response["message"] = message;
        sendJson(response, code);
    }

    bool parseRequest(JsonDocument& doc) {
        const String rawBody = isMqttContext ? activeBody : (server.hasArg("plain") ? server.arg("plain") : "");
        if (rawBody.length() == 0) {
            sendError(400, "Empty request body");
            return false;
        }
        const DeserializationError error = deserializeJson(doc, rawBody);
        if (error) {
            sendError(400, "Invalid JSON");
            return false;
        }
        return true;
    }

    bool dependenciesReady() {
        return sensors && relays && buzzer && security && automation && events && auth;
    }

    bool requireAuth() {
        if (!dependenciesReady() || !auth->isInitialized()) {
            sendError(503, "Server not ready");
            return false;
        }
        if (!auth->validateSession(getToken(), *events)) {
            sendError(401, "Unauthorized");
            return false;
        }
        return true;
    }

    void handleRoot() {
        server.send_P(200, "text/html; charset=utf-8", DASHBOARD_HTML);
    }

    void handleSetupStatus() {
        if (!auth || !auth->isInitialized()) {
            sendError(503, "Authentication storage is unavailable");
            return;
        }
        JsonDocument response;
        response["success"] = true;
        response["setupRequired"] = auth->isSetupRequired();
        response["loginBlocked"] = auth->isLoginBlocked();
        response["lockoutRemainingSeconds"] = auth->getLockoutRemainingSeconds();
        sendJson(response);
    }

    void handleSetup() {
        if (!auth || !events || !auth->isInitialized()) {
            sendError(503, "Server not ready");
            return;
        }
        if (!auth->isSetupRequired()) {
            sendError(409, "Initial setup has already been completed");
            return;
        }

        JsonDocument doc;
        if (!parseRequest(doc)) return;

        const String password = doc["password"].as<String>();
        const String confirmation = doc["confirmation"].as<String>();
        if (password.length() < Settings::MIN_PASSWORD_LENGTH) {
            sendError(400, "Password must contain at least 10 characters");
            return;
        }
        if (password != confirmation) {
            sendError(400, "Password confirmation does not match");
            return;
        }
        if (!auth->setupPassword(password, confirmation, *events)) {
            sendError(500, "Could not save the administrator password");
            return;
        }

        JsonDocument response;
        response["success"] = true;
        response["message"] = "Initial setup completed. Please log in.";
        sendJson(response);
    }

    void handleLogin() {
        if (!auth || !events || !auth->isInitialized()) {
            sendError(503, "Server not ready");
            return;
        }
        if (auth->isSetupRequired()) {
            sendError(409, "Initial setup is required");
            return;
        }
        if (auth->isLoginBlocked()) {
            sendError(429, "Login is temporarily blocked");
            return;
        }

        JsonDocument doc;
        if (!parseRequest(doc)) return;

        const String password = doc["password"].as<String>();
        const String device = doc["device"] | "Unknown device";
        String token;

        if (!auth->login(password, device, token, *events)) {
            if (auth->isLoginBlocked()) {
                sendError(429, "Too many attempts. Try again later.");
            } else {
                sendError(401, "Login failed");
            }
            return;
        }

        JsonDocument response;
        response["success"] = true;
        response["token"] = token;
        sendJson(response);
    }

    void handleLogout() {
        if (!requireAuth()) return;
        auth->logout(getToken(), *events);
        JsonDocument response;
        response["success"] = true;
        sendJson(response);
    }

    void handlePasswordChange() {
        if (!requireAuth()) return;
        JsonDocument doc;
        if (!parseRequest(doc)) return;

        const String currentPassword = doc["currentPassword"].as<String>();
        const String newPassword = doc["newPassword"].as<String>();
        const String confirmation = doc["confirmation"].as<String>();

        if (newPassword.length() < Settings::MIN_PASSWORD_LENGTH) {
            sendError(400, "New password must contain at least 10 characters");
            return;
        }
        if (newPassword != confirmation) {
            sendError(400, "Password confirmation does not match");
            return;
        }
        if (!auth->changePassword(currentPassword, newPassword, confirmation, *events)) {
            sendError(400, "Current password is incorrect or new password is invalid");
            return;
        }

        auth->logoutOtherSessions(getToken(), *events);
        JsonDocument response;
        response["success"] = true;
        response["message"] = "Password changed. Other sessions were signed out.";
        sendJson(response);
    }

    void handleSessions() {
        if (!requireAuth()) return;
        JsonDocument response;
        response["success"] = true;
        JsonArray list = response["sessions"].to<JsonArray>();
        const Models::Session* sessions = auth->getSessions();
        const uint32_t now = millis();

        for (uint8_t i = 0; i < Settings::MAX_ACTIVE_SESSIONS; ++i) {
            if (!sessions[i].active) continue;
            JsonObject item = list.add<JsonObject>();
            item["device"] = sessions[i].deviceInfo;
            item["lastActivitySecondsAgo"] = (now - sessions[i].lastActivity) / 1000;
            item["current"] = sessions[i].token == getToken();
        }
        sendJson(response);
    }

    void handleLogoutOthers() {
        if (!requireAuth()) return;
        auth->logoutOtherSessions(getToken(), *events);
        JsonDocument response;
        response["success"] = true;
        sendJson(response);
    }

    void handleStatus() {
        if (!requireAuth()) return;
        const Models::SensorData& s = sensors->getData();
        const Models::RelayState& r = relays->getState();

        JsonDocument response;
        response["success"] = true;

        JsonObject sensor = response["sensors"].to<JsonObject>();
        sensor["temperature"] = s.temperature;
        sensor["humidity"] = s.humidity;
        sensor["pirMotion"] = s.pirMotion;
        sensor["gasDetected"] = s.gasDetected;
        sensor["mainDoorOpen"] = s.mainDoorOpen;
        sensor["garageDoorOpen"] = s.garageDoorOpen;
        sensor["rainRaw"] = s.rainRaw;
        sensor["rainPercent"] = s.rainPercent;
        sensor["rainLevel"] = static_cast<uint8_t>(s.rainLevel);
        sensor["rainDetected"] = s.rainDetected;
        sensor["ldrState"] = s.ldrState;
        sensor["pirStatus"] = static_cast<uint8_t>(s.pirStatus);
        sensor["mq2Status"] = static_cast<uint8_t>(s.mq2Status);
        sensor["dhtStatus"] = static_cast<uint8_t>(s.dhtStatus);
        sensor["rainStatus"] = static_cast<uint8_t>(s.rainStatus);
        sensor["ldrStatus"] = static_cast<uint8_t>(s.ldrStatus);

        JsonObject relay = response["relays"].to<JsonObject>();
        relay["fan"] = r.fan;
        relay["yardLight"] = r.yardLight;
        relay["gasValve"] = r.gasValve;
        relay["heating"] = r.heating;

        response["smartMode"] = automation->isSmartModeEnabled();
        response["securityMode"] = static_cast<uint8_t>(security->getMode());
        response["securityAlert"] = security->isAlertActive();
        response["gasAlert"] = automation->isGasAlertActive();
        response["gasValveLocked"] = automation->isGasValveLocked();
        response["uptimeSeconds"] = millis() / 1000;
        sendJson(response);
    }

    void handleHistory() {
        if (!requireAuth()) return;
        JsonDocument response;
        response["success"] = true;
        JsonArray history = response["events"].to<JsonArray>();

        for (uint16_t i = 0; i < events->getCount(); ++i) {
            const Models::EventRecord* event = events->getEventAt(i);
            if (!event) continue;
            JsonObject item = history.add<JsonObject>();
            item["id"] = event->id;
            item["uptimeSeconds"] = event->uptimeSeconds;
            item["type"] = static_cast<uint8_t>(event->type);
            item["message"] = event->message;
            item["details"] = event->details;
        }
        sendJson(response);
    }

    void handleRelay() {
        if (!requireAuth()) return;
        if (automation->isSmartModeEnabled()) {
            sendError(403, "Manual relay control is disabled while Smart Mode is active");
            return;
        }

        JsonDocument doc;
        if (!parseRequest(doc)) return;
        if (!doc["name"].is<const char*>() || !doc["enabled"].is<bool>()) {
            sendError(400, "Relay name and enabled state are required");
            return;
        }

        const String name = doc["name"].as<String>();
        const bool enabled = doc["enabled"].as<bool>();

        if (name == "fan") relays->setFan(enabled);
        else if (name == "yardLight") relays->setYardLight(enabled);
        else if (name == "heating") relays->setHeating(enabled);
        else {
            sendError(400, "Invalid relay");
            return;
        }

        events->addEvent(Models::EventType::RELAY_CHANGED, "وضعیت رله تغییر کرد", name);
        JsonDocument response;
        response["success"] = true;
        sendJson(response);
    }

    void handleSecurityMode() {
        if (!requireAuth()) return;
        JsonDocument doc;
        if (!parseRequest(doc)) return;
        const int mode = doc["mode"] | -1;
        if (mode < 0 || mode > 2) {
            sendError(400, "Invalid security mode");
            return;
        }
        security->setMode(static_cast<Models::SecurityMode>(mode), *events, *buzzer);
        JsonDocument response;
        response["success"] = true;
        sendJson(response);
    }

    void handleSecurityReset() {
        if (!requireAuth()) return;
        security->resetAlert(*events, *buzzer);
        JsonDocument response;
        response["success"] = true;
        sendJson(response);
    }

    void handleSmartMode() {
        if (!requireAuth()) return;
        JsonDocument doc;
        if (!parseRequest(doc)) return;
        if (!doc["enabled"].is<bool>()) {
            sendError(400, "Enabled state is required");
            return;
        }
        automation->setSmartMode(doc["enabled"].as<bool>(), *events);
        JsonDocument response;
        response["success"] = true;
        sendJson(response);
    }

    void handleGasOpen() {
        if (!requireAuth()) return;
        JsonDocument doc;
        if (!parseRequest(doc)) return;
        const bool confirmed = doc["confirmed"] | false;
        const bool result = automation->requestGasValveOpen(
            confirmed, sensors->getData(), *relays, *events);

        if (!result) {
            sendError(403, "Gas valve opening rejected by safety checks");
            return;
        }
        JsonDocument response;
        response["success"] = true;
        sendJson(response);
    }

    void handleMqttMessage(char* topic, byte* payload, unsigned int length) {
        String message;
        message.reserve(length + 1);
        for (unsigned int i = 0; i < length; i++) {
            message += static_cast<char>(payload[i]);
        }

        JsonDocument reqDoc;
        if (deserializeJson(reqDoc, message)) return;

        const String reqId = reqDoc["reqId"] | "";
        const String path = reqDoc["path"] | "";
        activeToken = reqDoc["token"] | "";
        activeBody = "";
        if (!reqDoc["body"].isNull()) {
            serializeJson(reqDoc["body"], activeBody);
        }

        isMqttContext = true;
        lastStatusCode = 200;
        lastResponseJson = "{}";

        if (path == "/api/setup-status") handleSetupStatus();
        else if (path == "/api/setup") handleSetup();
        else if (path == "/api/login") handleLogin();
        else if (path == "/api/logout") handleLogout();
        else if (path == "/api/auth/password") handlePasswordChange();
        else if (path == "/api/auth/sessions") handleSessions();
        else if (path == "/api/auth/logout-others") handleLogoutOthers();
        else if (path == "/api/status") handleStatus();
        else if (path == "/api/history") handleHistory();
        else if (path == "/api/relay") handleRelay();
        else if (path == "/api/security/mode") handleSecurityMode();
        else if (path == "/api/security/reset") handleSecurityReset();
        else if (path == "/api/automation/smart-mode") handleSmartMode();
        else if (path == "/api/gas/open") handleGasOpen();
        else sendError(404, "Endpoint not found");

        isMqttContext = false;

        JsonDocument resEnvelope;
        resEnvelope["reqId"] = reqId;
        resEnvelope["status"] = lastStatusCode;
        resEnvelope["payload"] = lastResponseJson;

        String out;
        serializeJson(resEnvelope, out);
        mqttClient.publish(Settings::MQTT_TOPIC_RES, out.c_str());
    }

    void updateMqtt() {
        if (!Settings::USE_CLOUD_MQTT || WiFi.status() != WL_CONNECTED) return;

        if (!mqttClient.connected()) {
            unsigned long now = millis();
            if (now - lastMqttReconnectAttempt > 3000) {
                lastMqttReconnectAttempt = now;
                String clientId = "ESP32-SmartHome-" + String( static_cast<uint32_t>(ESP.getEfuseMac()), HEX );
                if (mqttClient.connect(clientId.c_str(), Settings::MQTT_USER, Settings::MQTT_PASS)) {
                    mqttClient.subscribe(Settings::MQTT_TOPIC_REQ);
                    Serial.println("Connected to Runflare Mosquitto Broker");
                }
            }
        } else {
            mqttClient.loop();
        }
    }

public:
    ApiManager() : server(Settings::WEB_SERVER_PORT), mqttClient(wifiClient) {}

    void begin(SensorManager& sensorManager, RelayManager& relayManager,
               BuzzerManager& buzzerManager, SecurityManager& securityManager,
               AutomationEngine& automationEngine, EventManager& eventManager,
               AuthManager& authManager) {
        sensors = &sensorManager;
        relays = &relayManager;
        buzzer = &buzzerManager;
        security = &securityManager;
        automation = &automationEngine;
        events = &eventManager;
        auth = &authManager;

        const char* headerKeys[] = {"Authorization"};
        server.collectHeaders(headerKeys, 1);

        server.on("/", HTTP_GET, [this]() { handleRoot(); });
        server.on("/api/setup-status", HTTP_GET, [this]() { handleSetupStatus(); });
        server.on("/api/setup", HTTP_POST, [this]() { handleSetup(); });
        server.on("/api/login", HTTP_POST, [this]() { handleLogin(); });
        server.on("/api/logout", HTTP_POST, [this]() { handleLogout(); });
        server.on("/api/auth/password", HTTP_POST, [this]() { handlePasswordChange(); });
        server.on("/api/auth/sessions", HTTP_GET, [this]() { handleSessions(); });
        server.on("/api/auth/logout-others", HTTP_POST, [this]() { handleLogoutOthers(); });
        server.on("/api/status", HTTP_GET, [this]() { handleStatus(); });
        server.on("/api/history", HTTP_GET, [this]() { handleHistory(); });
        server.on("/api/relay", HTTP_POST, [this]() { handleRelay(); });
        server.on("/api/security/mode", HTTP_POST, [this]() { handleSecurityMode(); });
        server.on("/api/security/reset", HTTP_POST, [this]() { handleSecurityReset(); });
        server.on("/api/automation/smart-mode", HTTP_POST, [this]() { handleSmartMode(); });
        server.on("/api/gas/open", HTTP_POST, [this]() { handleGasOpen(); });

        server.onNotFound([this]() { sendError(404, "Endpoint not found"); });
        server.begin();

        if (Settings::USE_CLOUD_MQTT) {
            mqttClient.setBufferSize(16384);
            mqttClient.setServer(Settings::MQTT_HOST, Settings::MQTT_PORT);
            mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
                this->handleMqttMessage(topic, payload, length);
            });
        }
    }

    void update() {
        server.handleClient();
        updateMqtt();
    }
};