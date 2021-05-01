// SPDX-License-Identifier: GPL-2.0-only
/* Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.
   Copyright (C) 2021 Will Page <compenguy@gmail.com>

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").
*/

#ifndef GAMEPAD_CONTROLLER_H
#define GAMEPAD_CONTROLLER_H

#include "hidgamepad.h"

class GamepadController : public GamepadReportParser {
  public:
    GamepadController(USBHost &usb) : hostGamepad(&usb), lastRead(true) {
        hostGamepad.SetReportParser(0, this);
    };

    bool getLastBuf(uint8_t packet[8]) {
        if (!lastRead) {
            for (int i = 0; i < 8; i++)
                packet[i] = lastBuf[i];
            lastRead = true;
        } else {
        }
        return lastRead;
    }

  protected:
    virtual void OnAnalogAxisChange(uint8_t id, GAMEPADAXISAINFO *);
    virtual void OnDigitalAxisChange(uint8_t id, GAMEPADAXISDINFO *);
    virtual void OnButtonChange(uint8_t id, GAMEPADBUTTONINFO *);
    virtual void OnUnknownReport(uint32_t bufsize, uint8_t *buf);

    virtual void NotifyChange(GAMEPADINFO *info);

  private:
    HIDGamepad hostGamepad;
    uint8_t lastBuf[8];
    bool lastRead;
};

#endif
