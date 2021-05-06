// SPDX-License-Identifier: MIT
// Copyright 2021 Will Page <compenguy@gmail.com>
// USBHost - Version: Latest

#include "GamepadController.h"
#include "blecentral.h"
#include "bleuart.h"
#include <cstdio>

// Initialize BLE Uart
Modem ble(Serial1);
Central blecentral(ble);

// Initialize USB Controller
USBHost usb;

// Attach mouse controller to USB
GamepadController gamepad(usb);

// This function intercepts unknown gamepad reports
void unhandledInput(uint32_t bufsize, uint8_t *buf) {
    Serial.print("Unrecognized gamepad packet: ");
    for (int i = 0; i < bufsize; i++) {
        char fmtbuf[6];
        snprintf(fmtbuf, sizeof(fmtbuf), "%02x ", buf[i]);
        Serial.print(fmtbuf);
    }
    Serial.println();
};

void buttonPressed(uint8_t id) {
    Serial.print("Button pressed: ");
    Serial.println(id, HEX);
}

void buttonReleased(uint8_t id) {
    Serial.print("Button released: ");
    Serial.println(id, HEX);
}

void analogAxisChange(uint8_t id, uint8_t value) {
    Serial.print("Axis ");
    Serial.print(id, HEX);
    Serial.print(" changed: ");
    Serial.println(value, HEX);
}

void digitalAxisChange(uint8_t id, uint8_t value) {
    Serial.print("Axis ");
    Serial.print(id, HEX);
    Serial.print(" changed: ");
    Serial.println(value, HEX);
}
/*
void configureBle() {
    Serial.println("Resetting BLE interface...");
    ble.resetConfiguration();
    Serial.println("Setting high RX/TX gain...");
    if (!ble.enableHighTxGain(true) || !ble.enableHighRxGain(true)) {
        Serial.println("Error setting high gain mode.");
    }
    Serial.println("Setting high power mode.");
    if (!ble.setModulePower(Modem::power_t::cP3dbm)) {
        Serial.println("Error switching to high power mode.");
    }
    Serial.println("Attempting to reconnect to last connected device...");
    if (ble.easyReconnect() != Modem::response_t::cReconnecting) {
        Serial.println("Error connecting to last known device.");
        connected = false;
    } else {
        Serial.println("Reconnected to device.");
        connected = true;
    }

    if (!connected) {
        Serial.println("Discovering remote devices...");
        if (!ble.discoverDevices() && ble.devicesCount()) {
            Serial.println("Error discovering devices, or no devices found.");
        } else if (!ble.getDevice(default_device_id, device)) {
            Serial.println("No device definition found for discovered device.");
        } else if (!ble.connectId(default_device_id, response) ||
                   response != Modem::response_t::cConnecting) {
            Serial.print("Unable to connect to device id ");
            Serial.print(default_device_id);
            Serial.print(": ");
            switch (response) {
            case Modem::response_t::cReconnecting:
                Serial.println(
                    "Failure condition reported, but response indicates "
                    "reconnecting to the device as normal");
                break;
            case Modem::response_t::cConnecting:
                Serial.println("Failure condition reported, but response "
                               "indicates connecting to the device as normal");
                break;
            case Modem::response_t::cError:
                Serial.println("Connect error.");
                break;
            case Modem::response_t::cFail:
                Serial.println("Connect fail.");
                break;
            case Modem::response_t::cNoAddress:
                Serial.println("No address to connect to.");
                break;
            case Modem::response_t::cOther:
                Serial.println("Unspecified error (Error error - no error for "
                               "the error).");
                break;
            }
        } else {
            Serial.print("Connected to device.");
            connected = true;
        }
    }
}
*/

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial.println("==============================");
    Serial.println("P R O G R A M   S T A R T");
    Serial.println("==============================");
    if (!blecentral.ready()) {
        Serial.println("Error setting BLE in central mode");
    }
    Serial.println("==============================");
    if (blecentral.isReady() && !blecentral.connectLast()) {
        Serial.println("Error connecting to last known client.");
        ble.discoverDevices();
        if (ble.devicesCount() && !blecentral.connectId(0)) {
            Serial.println("Found device, but unable to connect to it.");
        }
    }
    Serial.println("==============================");

    // Give the USB port time to quiesce
    delay(200);
}

void loop() {
    // Process USB tasks
    // usb.Task();

    if (blecentral.isConnected()) {
        for (uint8_t i = 0; i < 256; i++) {
            Serial.print("Writing to bleserial: ");
            Serial.println(i, HEX);
            Serial1.write(i);
            delay(100);
        }
    } else {
        Serial.println("Connection lost. Retrying...");
        blecentral.ready();
        if (blecentral.isReady() && !blecentral.connectLast()) {
            Serial.println("Error connecting to last known client.");
            ble.discoverDevices();
            if (ble.devicesCount() && !blecentral.connectId(0)) {
                Serial.println("Found device, but unable to connect to it.");
            }
        }
    }

    delay(1000);
}
