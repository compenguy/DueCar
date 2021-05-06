
#include "blecentral.h"
bool Central::isUnknown() { return state == Central::state_t::cUnknown; }

bool Central::isReady() { return state == Central::state_t::cReady; }

bool Central::isConnecting() {
    Serial.println("[Central::isConnecting] Start");
    if (state == Central::state_t::cConnecting) {
        Serial.println("[Central::isConnecting] Waiting for connection...");
        if (modem.waitConnected(10100)) {
            Serial.println("[Central::isConnecting] Connected/not connecting.");
            state = Central::state_t::cConnected;
            return false;
        } else {
            Serial.println("[Central::isConnecting] Connecting.");
            return true;
        }
    }
    Serial.println("[Central::isConnecting] Not connecting.");
    return false;
}

bool Central::isConnected() {
    Serial.println("[Central::isConnected] Start");
    if (state == Central::state_t::cConnected) {
        return true;
    }
    Serial.println(
        "[Central::isConnected] If connecting, wait for connection...");
    if (state == Central::state_t::cConnecting && modem.waitConnected(10100)) {
        Serial.println("[Central::isConnected] Was connecting, now connected.");
        state = Central::state_t::cConnected;
        return true;
    }
    Serial.println("[Central::isConnected] Not connected.");
    return false;
}

bool Central::ready() {
    Serial.println("[Central::ready] Start");
    if (isReady()) {
        Serial.println("[Central::ready] Already ready");
        return true;
    }
    if (isUnknown()) {
        Serial.println("[Central::ready] Transitioning from Unknown to Ready. "
                       "Issuing 'hangup'...");
        if (!disconnect() && !modem.reset()) {
            Serial.println("[Central::ready] Hangup and reset failed.  What do "
                           "we do now?");
            return false;
        }
    }

    if (isConnecting() || isConnected()) {
        Serial.println("[Central::ready] Transitioning from "
                       "Connecting/Connected to Ready.");
        if (!disconnect()) {
            return false;
        }
    }

    Serial.println(
        "[Central::ready] Resetting modem configuration to defaults...");
    if (!modem.resetConfiguration()) {
        Serial.println("[Central::ready] Failed to reset modem configuration "
                       "to defaults.");
        state = Central::state_t::cUnknown;
        return false;
    }
    Serial.println("[Central::ready] Switching modem to central mode...");
    if (!modem.makeCentral()) {
        Serial.println(
            "[Central::ready] Failed to switch modem to central mode.");
        state = Central::state_t::cUnknown;
        return false;
    }

    Serial.println("[Central::ready] End (Success)");
    state = Central::state_t::cReady;
    return true;
}

bool Central::connectLast() {
    Serial.println("[Central::connectLast] Start");
    if (isUnknown()) {
        Serial.println("[Central::connectLast] Failed... state Unknown.");
        return false;
    }

    if (isConnecting() || isConnected()) {
        Serial.println("[Central::connectLast] Failed... already "
                       "connecting/connected. Disconnecting...");
        modem.disconnect();
        return false;
    }

    Serial.println("[Central::connectLast] Connecting to last known device...");
    Modem::response_t response;
    if (modem.reconnect(response, false) &&
        response == Modem::response_t::cReconnecting) {
        Serial.println("[Central::connectLast] Connecting...");
        state = Central::state_t::cConnecting;
        return true;
    }
    Serial.print("[Central::connectLast] Failed (response code ");
    Serial.print((uint8_t)response, HEX);
    Serial.println(")");
    modem.disconnect();
    return false;
}

bool Central::connectId(uint8_t id) {
    Serial.println("[Central::connectId] Start");
    if (isUnknown()) {
        Serial.println("[Central::connectId] Failed... state Unknown.");
        return false;
    }

    if (isConnecting() || isConnected()) {
        Serial.println("[Central::connectId] Failed... already "
                       "connecting/connected. Disconnecting...");
        modem.disconnect();
        return false;
    }

    Serial.println(
        "[Central::connectId] Connecting to device by discovery id...");
    Modem::response_t response;
    if (modem.connectId(id, response, false) &&
        response == Modem::response_t::cConnecting) {
        Serial.println("[Central::connectId] Connecting...");
        state = Central::state_t::cConnecting;
        return true;
    }
    Serial.print("[Central::connectId] Failed (response code ");
    Serial.print((uint8_t)response, HEX);
    Serial.println(")");
    modem.disconnect();
    return false;
}

bool Central::connectAddress(const String &mac) {
    Serial.println("[Central::connectAddress] Start");
    if (isUnknown()) {
        Serial.println("[Central::connectAddress] Failed... state Unknown.");
        return false;
    }

    if (isConnecting() || isConnected()) {
        Serial.println("[Central::connectAddress] Failed... already "
                       "connecting/connected. Disconnecting...");
        modem.disconnect();
        return false;
    }

    Serial.println(
        "[Central::connectAddress] Connecting to device by MAC address...");
    Modem::response_t response;
    if (modem.connectAddress(mac, response, false) &&
        response == Modem::response_t::cConnecting) {
        Serial.println("[Central::connectAddress] Connecting...");
        state = Central::state_t::cConnecting;
        return true;
    }
    Serial.print("[Central::connectAddress] Failed (response code ");
    Serial.print((uint8_t)response, HEX);
    Serial.println(")");
    modem.disconnect();
    return false;
}

bool Central::disconnect() {
    Serial.println("[Central::disconnect] Disconnecting...");
    state = Central::state_t::cReady;
    return modem.disconnect();
}
