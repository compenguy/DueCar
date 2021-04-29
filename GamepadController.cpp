#include "GamepadController.h"

extern "C" {
void __gamepadControllerEmptyCallback() { }
}

void unhandledInput() __attribute__ ((weak, alias("__gamepadControllerEmptyCallback")));

void GamepadController::OnUnknownReport(uint32_t bufsize, uint8_t *buf) {
  for(int i = 0; i < bufsize && i < 8; i++) {
    lastBuf[i] = buf[i];
  }
  lastRead = false;
}
