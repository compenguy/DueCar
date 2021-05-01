#include "GamepadController.h"

extern "C" {
void __gamepadControllerEmptyCallback() {}
}

void unhandledInput(uint32_t bufsize, uint8_t *buf)
    __attribute__((weak, alias("__gamepadControllerEmptyCallback")));
void buttonPressed(uint8_t id)
    __attribute__((weak, alias("__gamepadControllerEmptyCallback")));
void buttonReleased(uint8_t id)
    __attribute__((weak, alias("__gamepadControllerEmptyCallback")));
void analogAxisChange(uint8_t id, uint8_t)
    __attribute__((weak, alias("__gamepadControllerEmptyCallback")));
void digitalAxisChange(uint8_t id, uint8_t)
    __attribute__((weak, alias("__gamepadControllerEmptyCallback")));

// Digital Pad X (5-7 -> left, 1-3 -> right)
uint8_t dpad_x[9] = {
    127, // Up
    255, // Up-Right
    255, // Right
    255, // Down-Right
    127, // Down
    0,   // Down-Left
    0,   // Left
    0,   // Up-Left
    127  // Center
};

// Digital Pad Y (7,0,1 -> up, 3-5 -> down)
uint8_t dpad_y[9] = {
    0,   // Up
    0,   // Up-Right
    127, // Right
    255, // Down-Right
    255, // Down
    255, // Down-Left
    127, // Left
    0,   // Up-Left
    127  // Center
};

void GamepadController::NotifyChange(GAMEPADINFO *info) {
    uint8_t id = info->id;

    if (info->usage == GAMEPAD_AXIS_A &&
        prevState[id].value.axis_a.bmAxisA != info->value.axis_a.bmAxisA) {
        OnAnalogAxisChange(id, &info->value.axis_a);
    } else if (info->usage == GAMEPAD_AXIS_D &&
               prevState[id].value.axis_d.bmAxisD !=
                   info->value.axis_d.bmAxisD) {
        OnDigitalAxisChange(id, &info->value.axis_d);
    } else if (info->usage == GAMEPAD_BUTTON &&
               prevState[id].value.button.bmButtonState !=
                   info->value.button.bmButtonState) {
        OnButtonChange(id, &info->value.button);
    }
}

void GamepadController::OnUnknownReport(uint32_t bufsize, uint8_t *buf) {

    if (bufsize != 8) {
        unhandledInput(bufsize, buf);
        return;
    }

    GAMEPADINFO tmp_info;
    // Left Analog Pad X (0->255 left->right)
    uint8_t id = 0;
    tmp_info.id = id;
    tmp_info.usage = GAMEPAD_AXIS_A;
    tmp_info.value.axis_a.bmAxisA = buf[0];
    NotifyChange(&tmp_info);

    // Left Analog Pad Y (0->255 up->down)
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_AXIS_A;
    tmp_info.value.axis_a.bmAxisA = buf[1];
    NotifyChange(&tmp_info);

    // Right Analog Pad X (0->255 left->right)
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_AXIS_A;
    tmp_info.value.axis_a.bmAxisA = buf[2];
    NotifyChange(&tmp_info);

    // Right Analog Pad Y (0->255 up->down)
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_AXIS_A;
    tmp_info.value.axis_a.bmAxisA = buf[3];
    NotifyChange(&tmp_info);

    // Digital Pad X
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_AXIS_D;
    tmp_info.value.axis_d.bmAxisD = dpad_x[buf[4] & 0x0F];
    NotifyChange(&tmp_info);

    // Digital Pad Y
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_AXIS_D;
    tmp_info.value.axis_d.bmAxisD = dpad_y[buf[4] & 0x0F];
    NotifyChange(&tmp_info);

    // Button 1/Square/Y
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[4] & 0x10;
    NotifyChange(&tmp_info);

    // Button 2/Cross/B
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[4] & 0x20;
    NotifyChange(&tmp_info);

    // Button 3/Circle/A
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[4] & 0x40;
    NotifyChange(&tmp_info);

    // Button 4/Square/X
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[4] & 0x80;
    NotifyChange(&tmp_info);

    // Button Right Analog Click
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[5] & 0x80;
    NotifyChange(&tmp_info);

    // Button Left Analog Click
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[5] & 0x40;
    NotifyChange(&tmp_info);

    // Button 10/Start
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[5] & 0x20;
    NotifyChange(&tmp_info);

    // Button 9/Select
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[5] & 0x10;
    NotifyChange(&tmp_info);

    // Button 5/L1
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[5] & 0x01;
    NotifyChange(&tmp_info);

    // Button 6/R1
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[5] & 0x02;
    NotifyChange(&tmp_info);

    // Button 7/L2
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[5] & 0x04;
    NotifyChange(&tmp_info);

    // Button 8/R2
    tmp_info.id = ++id;
    tmp_info.usage = GAMEPAD_BUTTON;
    tmp_info.value.button.bmButtonState = buf[5] & 0x08;
    NotifyChange(&tmp_info);
}

void GamepadController::OnAnalogAxisChange(uint8_t id,
                                           GAMEPADAXISAINFO *axis_a) {
    analogAxisChange(id, axis_a->bmAxisA);
    prevState[id].value.axis_a = *axis_a;
}

void GamepadController::OnDigitalAxisChange(uint8_t id,
                                            GAMEPADAXISDINFO *axis_d) {
    digitalAxisChange(id, axis_d->bmAxisD);
    prevState[id].value.axis_d = *axis_d;
}

void GamepadController::OnButtonChange(uint8_t id, GAMEPADBUTTONINFO *button) {
    if (button->bmButtonState) {
        buttonPressed(id);
    } else {
        buttonReleased(id);
    }
    prevState[id].value.button = *button;
}
