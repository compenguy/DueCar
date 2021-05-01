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
    Serial.print("Gamepad packet: ");
    for (int i = 0; i < bufsize; i++) {
        char fmtbuf[6];
        snprintf(fmtbuf, sizeof(fmtbuf), "%02x ", buf[i]);
        Serial.print(fmtbuf);
    }
    Serial.println();
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
