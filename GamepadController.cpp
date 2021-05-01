#include "GamepadController.h"

extern "C" {
void __gamepadControllerEmptyCallback() {}
}

void unhandledInput(uint32_t bufsize, uint8_t *buf)
    __attribute__((weak, alias("__gamepadControllerEmptyCallback")));

void GamepadController::OnUnknownReport(uint32_t bufsize, uint8_t *buf) {
    unhandledInput(bufsize, buf);
}
