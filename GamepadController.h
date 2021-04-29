#ifndef GAMEPAD_CONTROLLER_H
#define GAMEPAD_CONTROLLER_H

#include "hidgamepad.h"

class GamepadController : public GamepadReportParser
{
public:
  GamepadController(USBHost &usb) : hostGamepad(&usb), lastRead(true) {
    hostGamepad.SetReportParser(0, this);
  };

  bool getLastBuf(uint8_t packet[8]) {
    if (!lastRead) {
      for (int i = 0; i < 8; i++) packet[i] = lastBuf[i];
      lastRead = true;
    };
    return lastRead;
  }

protected:
  virtual void OnUnknownReport (uint32_t bufsize, uint8_t *buf);


private:
  HIDGamepad hostGamepad;
  uint8_t lastBuf[8];
  bool lastRead;
};

#endif
