// SPDX-License-Identifier: MIT
// Copyright 2021 Will Page <compenguy@gmail.com>
// USBHost - Version: Latest

#include "GamepadController.h"
#include "bleuart.h"
#include <cstdio>

// Initialize BLE Uart
Modem ble(Serial1);

// Initialize USB Controller
USBHost usb;

// Attach mouse controller to USB
GamepadController gamepad(usb);

char outbuf[64];

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

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial.println("Program started");
    Serial.print("BLE Help: ");
    Serial.println(ble.help());
    String fwver;
    if (ble.getFirmwareVersion(fwver)) {
        Serial.println(fwver);
    } else {
        Serial.println("Failed querying BLE firmware version");
    }
    String address;
    if (ble.getAddress(address)) {
        Serial.println(address);
    } else {
        Serial.println("Failed querying BLE device address.");
    }
    // Give the USB port time to quiesce
    delay(200);
}

void loop() {
    // Process USB tasks
    // usb.Task();

    Serial.println("Discovering devices...");
    if (ble.discoverDevices()) {
        Serial.println("Success!");
        uint8_t count = ble.devicesCount();
        for (uint8_t i = 0; i < count; i++) {
            Modem::device_t device;
            if (ble.getDevice(i, device)) {
                Serial.print(device.id);
                Serial.print(": ");
                Serial.print(device.name);
                Serial.print("[");
                Serial.print(device.mac);
                Serial.println("]");
            } else {
                Serial.println("Invalid device id");
            }
        }
    } else {
        Serial.println("Failed.");
    }

    delay(1000);
}
