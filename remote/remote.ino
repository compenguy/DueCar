// SPDX-License-Identifier: MIT
// Copyright 2021 Will Page <compenguy@gmail.com>
// USBHost - Version: Latest

#include "GamepadController.h"
#include "HM18ModemManager.h"
#include <cstdio>

// Initialize BLE Uart
Modem ble(Serial1, 17);
String targetAddress("FC694765A341");

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

void serial_echo_setup() {
    ble.disconnect();
    ble.resetConfiguration();
    ble.makePeripheral(true);
}

void serial_echo() {
    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }
}

void ble_app_setup() {
    Serial1.begin(38400);
    Serial1.setTimeout(100);
    Serial.println("Configuring modem...");
    ble.disconnect();
    ble.setPio(1, true);
    ble.statePinBlinks(false);
    ble.makeCentral() || Serial.println("Error setting BLE in central mode");
    ble.enableNotifications(true) || Serial.println("Notifications disabled.");
    ble.enableSaveLastDevice(true) ||
        Serial.println("SaveLastDevice disabled.");
    ble.enableNotifyAddressOnDisconnect(true) ||
        Serial.println("NotifyAddressOnDisconnect disabled.");
    ble.enableNameDiscovery(true) || Serial.println("Name discovery disabled.");
    ble.setBaudRate(Modem::rate_t::c38400) || Serial.println("Baudrate unchanged.");
    ble.enableHighRxGain(true) ||
        Serial.println("High antenna rx gain disabled.");
    // ble.enableHighTxGain(true) || Serial.println("High antenna tx gain
    // disabled.");
    ble.setModulePower(Modem::power_t::cP3dbm) ||
        Serial.println("High radio power disabled.");
}

bool ensure_ble_connection() {
    if (ble.isConnected()) {
        return true;
    }
    Serial.println("Ensuring target connection.");
    Modem::response_t response;
    if (ble.connectAddress(targetAddress, response, true) &&
        response == Modem::response_t::cConnecting) {
        Serial.println("Connected.");
        return true;
    } else {
        Serial.println("Not connected.");
        return false;
    }
}


void ble_app() {
    if (ensure_ble_connection()) {
        Serial.println("Writing over modem: 'f'");
        Serial1.write('f');
    } else {
        delay(10 * 1000);
    }
    delay(1000);
}

void setup() {
    Serial.begin(9600);
    Serial.println("==============================");
    Serial.println("  P R O G R A M   S T A R T  ");
    Serial.println("==============================");
    ble_app_setup();
    // serial_echo_setup();
    Serial.println("==============================");
}

void loop() {
    // Process USB tasks
    // usb.Task();

    ble_app();
    // serial_echo();
}
