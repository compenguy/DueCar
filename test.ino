// USBHost - Version: Latest

#include "GamepadController.h"
#include <cstdio>

/*

*/

// Initialize USB Controller
USBHost usb;

// Attach mouse controller to USB
GamepadController gamepad(usb);

char outbuf[64];

// This function intercepts unknown gamepad reports
void unhandledInput() {
    uint8_t packet[8];

    if (gamepad.getLastBuf(packet)) {
        Serial.print("Gamepad packet: ");
        for (int i = 0; i < sizeof(packet); i++) {
            Serial.print(packet[i], HEX);
        }
        Serial.println();
    }
};

void setup() {
    Serial.begin(9600);
    Serial.println("Program started");
    delay(200);
}

void loop() {
    // Process USB tasks
    usb.Task();
}
