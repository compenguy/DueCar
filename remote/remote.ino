// SPDX-License-Identifier: MIT
// Copyright 2021 Will Page <compenguy@gmail.com>
// USBHost - Version: Latest

#include "GamepadController.h"
#include <cstdio>

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
    Serial.println("Program started");
    delay(200);
}

void loop() {
    // Process USB tasks
    usb.Task();
}
