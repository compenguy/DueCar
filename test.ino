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
    snprintf(outbuf,
             sizeof(outbuf),
             "Gamepad packet: %02x %02x %02x %02x %02x %02x %02x %02x",
             (int)packet[0],
             (int)packet[1],
             (int)packet[2],
             (int)packet[3],
             (int)packet[4],
             (int)packet[5],
             (int)packet[6],
             (int)packet[7]);
    Serial.println((char*)outbuf);
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
