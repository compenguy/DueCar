
#include "blecentral.h"
bool Central::isUnknown() { return state == Central::state_t::cUnknown; }

bool Central::isReady() { return state == Central::state_t::cReady; }

bool Central::isConnecting() {
    Serial.println("isConnecting()");
    if (state == Central::state_t::cConnecting) {
        Serial.println("Waiting for connection...");
        if (modem.waitConnected()) {
            Serial.println("Connected/not connecting.");
            state = Central::state_t::cConnected;
            return false;
        } else {
            Serial.println("Connecting.");
            return true;
        }
    }
    Serial.println("Not connecting.");
    return false;
}

bool Central::isConnected() {
    Serial.println("isConnected()");
    if (state == Central::state_t::cConnected) {
        return true;
    }
    Serial.println("If connecting, wait for connection...");
    if (state == Central::state_t::cConnecting && modem.waitConnected()) {
        Serial.println("Was connecting, now connected.");
        state = Central::state_t::cConnected;
        return true;
    }
    Serial.println("Not connected.");
    return false;
}

bool Central::ready() {
    Serial.println("ready()");
    if (isReady()) {
        Serial.println("Trivially complete.");
        return true;
    }
    if (isConnecting() || isConnected()) {
        Serial.println("Transitioning from Connecting/Connected to Ready.");
        return disconnect();
    }

    Serial.println("Resetting modem configuration to defaults...");
    if (!modem.resetConfiguration()) {
        Serial.println("Failed to reset modem configuration to defaults.");
        state = Central::state_t::cUnknown;
        return false;
    }
    Serial.println("Switching modem to central mode...");
    if (!modem.makeCentral()) {
        Serial.println("Failed to switch modem to central mode.");
        state = Central::state_t::cUnknown;
        return false;
    }

    Serial.println("Ready.");
    state = Central::state_t::cReady;
    return true;
}

bool Central::connectLast() {
    Serial.println("connectLast()");
    if (isUnknown()) {
        Serial.println("Failed... state Unknown.");
        return false;
    }

    if (isConnecting() || isConnected()) {
        Serial.println(
            "Failed... already connecting/connected. Disconnecting...");
        modem.disconnect();
        return false;
    }

    Serial.println("Connecting to last known device...");
    Modem::response_t response;
    if (modem.reconnect(response, false) &&
        response == Modem::response_t::cReconnecting) {
        Serial.println("Connecting...");
        state = Central::state_t::cConnecting;
        return true;
    }
    Serial.print("Failed (response code ");
    Serial.print((uint8_t)response, HEX);
    Serial.println(")");
    modem.disconnect();
    return false;
}

bool Central::connectId(uint8_t id) {
    Serial.println("connectId()");
    if (isUnknown()) {
        Serial.println("Failed... state Unknown.");
        return false;
    }

    if (isConnecting() || isConnected()) {
        Serial.println(
            "Failed... already connecting/connected. Disconnecting...");
        modem.disconnect();
        return false;
    }

    Serial.println("Connecting to device by discovery id...");
    Modem::response_t response;
    if (modem.connectId(id, response, false) &&
        response == Modem::response_t::cConnecting) {
        Serial.println("Connecting...");
        state = Central::state_t::cConnecting;
        return true;
    }
    Serial.print("Failed (response code ");
    Serial.print((uint8_t)response, HEX);
    Serial.println(")");
    modem.disconnect();
    return false;
}

bool Central::connectAddress(const String &mac) {
    Serial.println("connectAddress()");
    if (isUnknown()) {
        Serial.println("Failed... state Unknown.");
        return false;
    }

    if (isConnecting() || isConnected()) {
        Serial.println(
            "Failed... already connecting/connected. Disconnecting...");
        modem.disconnect();
        return false;
    }

    Serial.println("Connecting to device by MAC address...");
    Modem::response_t response;
    if (modem.connectAddress(mac, response, false) &&
        response == Modem::response_t::cConnecting) {
        Serial.println("Connecting...");
        state = Central::state_t::cConnecting;
        return true;
    }
    Serial.print("Failed (response code ");
    Serial.print((uint8_t)response, HEX);
    Serial.println(")");
    modem.disconnect();
    return false;
}

bool Central::disconnect() {
    Serial.println("Disconnecting...");
    state = Central::state_t::cReady;
    return modem.disconnect();
}
