// SPDX-License-Identifier: MIT
// Copyright 2021 Will Page <compenguy@gmail.com>
// USBHost - Version: Latest

#include "GamepadController.h"
#include "HM18ModemManager.h"
#include <cstdio>

// Initialize BLE Uart
Modem ble(Serial1, 17);

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

bool configureModem() {
    Serial.println("Configuring modem...");
    ble.disconnect();
    bool enable = true;
    if (!ble.notificationsEnabled(enable) || enable == false) {
        ble.enableNotifications(true) || Serial.println("Notifications disabled.");  
    }
    if (!ble.saveLastDeviceEnabled(enable) || enable == false) {
        ble.enableSaveLastDevice(true) || Serial.println("SaveLastDevice disabled.");    
    }
    if (!ble.notifyAddressOnDisconnectEnabled(enable) || enable == false ) {
        ble.enableNotifyAddressOnDisconnect(true) || Serial.println("NotifyAddressOnDisconnect disabled.");
    }
    if (!ble.nameDiscoveryEnabled(enable) || enable == false) {
        ble.enableNameDiscovery(true) || Serial.println("Name discovery disabled.");
    }
    if (!ble.highRxGainEnabled(enable) || enable == false) {
        ble.enableHighRxGain(true) || Serial.println("High antenna gain disabled.");
    }
    /*
    if (!ble.highTxGainEnabled(enable) || enable == false) {
        ble.enableHighTxGain(true);
    }
    */
    ble.setModulePower(Modem::power_t::cP3dbm) || Serial.println("High radio power disabled.");

    if (!ble.makeCentral()) {
        Serial.println("Error setting BLE in central mode");
    }
}

bool ensureDeviceConnection() {
    if (ble.isConnected()) {
        return true;
    }
    Serial.println("Not connected.");
    String lastAddress;
    Modem::response_t response;
    Serial.println("Trying last connected device...");
    if (ble.getLastConnected(lastAddress) && lastAddress.length() && ble.reconnect(response, true) && response == Modem::response_t::cReconnecting) {
        Serial.println("Auto-reconnected to last known device.");
        return true;
    }
    if (!ble.devicesCount()) {
        Serial.println("Scanning...");
        ble.discoverDevices();
    }
    if (ble.devicesCount()) {
        Serial.println("Trying first discovered device in scan...");
        if (ble.connectId(0, response, true) && response == Modem::response_t::cConnecting) {
            Serial.println("Auto-connected to first discovered device.");
            return true;
        }
    }
    Serial.println("Failed to autoconnect.");
    return false; 
}

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial.println("==============================");
    Serial.println("  P R O G R A M   S T A R T  ");
    Serial.println("==============================");
    configureModem();
    Serial.println("==============================");
}

void loop() {
    // Process USB tasks
    // usb.Task();

    if (ensureDeviceConnection()) {
        Serial.println("Writing over modem: 'f'");
        Serial1.write('f');
    } else {
        delay(10*1000);
    }
    delay(1000);
}
