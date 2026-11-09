#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include <mbedtls/sha256.h>
#include <esp_system.h>
#include "Models.h"
#include "Settings.h"
#include "EventManager.h"

class AuthManager {
private:
    Preferences preferences;
    Models::LoginState loginState;
    Models::Session sessions[Settings::MAX_ACTIVE_SESSIONS];
    String passwordHash;
    String passwordSalt;
    bool initialized = false;

    String generateSalt() {
        char buffer[33];
        for (int i = 0; i < 16; ++i) {
            const uint8_t value = static_cast<uint8_t>(esp_random() & 0xFF);
            sprintf(buffer + (i * 2), "%02x", value);
        }
        buffer[32] = '\0';
        return String(buffer);
    }

    String hashPassword(const String& password, const String& salt) {
        const String input = salt + password;
        uint8_t hash[32];
        mbedtls_sha256(reinterpret_cast<const unsigned char*>(input.c_str()),
                       input.length(), hash, 0);
        char output[65];
        for (int i = 0; i < 32; ++i) sprintf(output + (i * 2), "%02x", hash[i]);
        output[64] = '\0';
        return String(output);
    }

    int findSession(const String& token) const {
        if (token.length() == 0) return -1;
        for (int i = 0; i < Settings::MAX_ACTIVE_SESSIONS; ++i)
            if (sessions[i].active && sessions[i].token == token) return i;
        return -1;
    }

    int findFreeSession() const {
        for (int i = 0; i < Settings::MAX_ACTIVE_SESSIONS; ++i)
            if (!sessions[i].active) return i;
        return -1;
    }

    String generateToken() {
        String token;
        token.reserve(32);
        for (int i = 0; i < 4; ++i) {
            char buffer[9];
            sprintf(buffer, "%08lx", static_cast<unsigned long>(esp_random()));
            token += buffer;
        }
        return token;
    }

    void expireSessions(EventManager& events) {
        const uint32_t now = millis();
        for (int i = 0; i < Settings::MAX_ACTIVE_SESSIONS; ++i) {
            if (!sessions[i].active) continue;
            if (now - sessions[i].lastActivity >= Settings::SESSION_TIMEOUT_MS) {
                sessions[i].active = false;
                sessions[i].token = "";
                events.addEvent(Models::EventType::SESSION_EXPIRED, "نشست کاربر منقضی شد");
            }
        }
    }

    bool savePassword(const String& password, const String& salt) {
        const String hash = hashPassword(password, salt);
        if (preferences.putString("passhash", hash) == 0) return false;
        if (preferences.putString("salt", salt) == 0) return false;
        preferences.putBool("configured", true);
        passwordHash = hash;
        passwordSalt = salt;
        return true;
    }

public:
    void begin() {
        initialized = preferences.begin("smarthome", false);
        if (!initialized) return;

        if (preferences.getBool("configured", false)) {
            passwordHash = preferences.getString("passhash", "");
            passwordSalt = preferences.getString("salt", "");
        }

        loginState.failedAttempts = 0;
        loginState.loginBlocked = false;
        loginState.lockoutStartedAt = 0;

        for (int i = 0; i < Settings::MAX_ACTIVE_SESSIONS; ++i) {
            sessions[i].active = false;
            sessions[i].token = "";
        }
    }

    bool isInitialized() const { return initialized; }
    bool isSetupRequired() const { return passwordHash.length() == 0; }

    bool setupPassword(const String& password, const String& confirmation, EventManager& events) {
        if (!initialized || !isSetupRequired()) return false;
        if (password.length() < Settings::MIN_PASSWORD_LENGTH || password != confirmation) return false;

        const String salt = generateSalt();
        if (!savePassword(password, salt)) return false;

        events.addEvent(Models::EventType::INITIAL_SETUP_COMPLETED, "حساب مدیر ایجاد شد");
        return true;
    }

    bool login(const String& password, const String& deviceInfo, String& token, EventManager& events) {
        if (!initialized || isSetupRequired()) return false;

        const uint32_t now = millis();
        if (loginState.loginBlocked) {
            if (now - loginState.lockoutStartedAt < Settings::LOGIN_LOCKOUT_MS) return false;
            loginState.loginBlocked = false;
            loginState.failedAttempts = 0;
        }

        if (hashPassword(password, passwordSalt) != passwordHash) {
            if (loginState.failedAttempts < 255) loginState.failedAttempts++;
            events.addEvent(Models::EventType::LOGIN_FAILED, "تلاش ناموفق برای ورود");
            if (loginState.failedAttempts >= Settings::MAX_LOGIN_ATTEMPTS) {
                loginState.loginBlocked = true;
                loginState.lockoutStartedAt = now;
                events.addEvent(Models::EventType::LOGIN_BLOCKED, "ورود به مدت ۶۰ ثانیه مسدود شد");
            }
            return false;
        }

        const int slot = findFreeSession();
        if (slot < 0) return false;

        loginState.failedAttempts = 0;
        loginState.loginBlocked = false;
        token = generateToken();
        sessions[slot].token = token;
        sessions[slot].deviceInfo = deviceInfo.substring(0, 100);
        sessions[slot].lastActivity = now;
        sessions[slot].active = true;

        events.addEvent(Models::EventType::LOGIN_SUCCESS, "ورود مدیر موفق بود");
        return true;
    }

    bool validateSession(const String& token, EventManager& events) {
        if (!initialized || token.length() == 0) return false;
        expireSessions(events);
        const int slot = findSession(token);
        if (slot < 0) return false;
        sessions[slot].lastActivity = millis();
        return true;
    }

    bool changePassword(const String& currentPassword, const String& newPassword,
                        const String& confirmation, EventManager& events) {
        if (!initialized || isSetupRequired()) return false;
        if (hashPassword(currentPassword, passwordSalt) != passwordHash) return false;
        if (newPassword.length() < Settings::MIN_PASSWORD_LENGTH ||
            newPassword != confirmation || newPassword == currentPassword) return false;

        if (!savePassword(newPassword, generateSalt())) return false;
        events.addEvent(Models::EventType::PASSWORD_CHANGED, "رمز مدیر تغییر کرد");
        return true;
    }

    void logout(const String& token, EventManager& events) {
        const int slot = findSession(token);
        if (slot < 0) return;
        sessions[slot].active = false;
        sessions[slot].token = "";
        events.addEvent(Models::EventType::LOGOUT, "خروج از داشبورد انجام شد");
    }

    void logoutOtherSessions(const String& currentToken, EventManager& events) {
        for (int i = 0; i < Settings::MAX_ACTIVE_SESSIONS; ++i) {
            if (sessions[i].active && sessions[i].token != currentToken) {
                sessions[i].active = false;
                sessions[i].token = "";
                events.addEvent(Models::EventType::SESSION_REVOKED, "نشست دستگاه دیگر لغو شد");
            }
        }
    }

    void logoutAll(EventManager& events) {
        for (int i = 0; i < Settings::MAX_ACTIVE_SESSIONS; ++i) {
            sessions[i].active = false;
            sessions[i].token = "";
        }
        events.addEvent(Models::EventType::SESSION_REVOKED, "تمام نشست‌های فعال لغو شدند");
    }

    uint8_t getActiveSessionCount() const {
        uint8_t count = 0;
        for (int i = 0; i < Settings::MAX_ACTIVE_SESSIONS; ++i)
            if (sessions[i].active) ++count;
        return count;
    }

    const Models::Session* getSessions() const { return sessions; }

    bool isLoginBlocked() const {
        return loginState.loginBlocked &&
               millis() - loginState.lockoutStartedAt < Settings::LOGIN_LOCKOUT_MS;
    }

    uint32_t getLockoutRemainingSeconds() const {
        if (!isLoginBlocked()) return 0;
        const uint32_t elapsed = millis() - loginState.lockoutStartedAt;
        const uint32_t remaining = Settings::LOGIN_LOCKOUT_MS - elapsed;
        return (remaining + 999) / 1000;
    }
};
