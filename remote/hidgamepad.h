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

#ifndef HIDGAMEPAD_H_INCLUDED
#define HIDGAMEPAD_H_INCLUDED

#include "Arduino.h"
#include "Usb.h"
#include "confdescparser.h"
#include "hid.h"
#include "usb_ch9.h"
#include <stdint.h>

/**
 * \brief GAMEPADREPORTTYPE definition.
 */
enum GAMEPADREPORTTYPE {
    GAMEPAD_AXIS_D,
    GAMEPAD_AXIS_A,
    GAMEPAD_BUTTON,
};

/**
 * \brief GAMEPADAXISDINFO definition.
 */
struct GAMEPADAXISDINFO {
    uint8_t bmAxisD;
};

/**
 * \brief GAMEPADAXISAINFO definition.
 */
struct GAMEPADAXISAINFO {
    uint8_t bmAxisA;
};

/**
 * \brief GAMEPADBUTTONINFO definition.
 */
struct GAMEPADBUTTONINFO {
    bool bmButtonState;
};

/**
 * \brief GAMEPADINFO definition.
 */
struct GAMEPADINFO {
    uint8_t id;
    uint8_t usage;
    GAMEPADREPORTTYPE type;
    union {
        GAMEPADAXISDINFO axis_d;
        GAMEPADAXISAINFO axis_a;
        GAMEPADBUTTONINFO button;
    } value;
};

/**
 * \class MouseReportParser definition.
 */
class GamepadReportParser : public HIDReportParser {
  public:
    virtual void Parse(HID *hid, bool is_rpt_id, uint32_t len, uint8_t *buf);

  protected:
    virtual void OnAnalogAxisChange(uint8_t id, GAMEPADAXISAINFO *){};
    virtual void OnDigitalAxisChange(uint8_t id, GAMEPADAXISDINFO *){};
    virtual void OnButtonChange(uint8_t id, GAMEPADBUTTONINFO *){};
    virtual void OnUnknownReport(uint32_t bufsize, uint8_t *buf){};

    GAMEPADINFO prevState[32];
};

#define totalEndpoints 2

#define HID_MAX_HID_CLASS_DESCRIPTORS 5

#define HID_NO_INTF_SUBCLASS 0x00

#define HID_PROTOCOL_NONE 0x00

/**
 * \class HIDGamepad definition.
 */
class HIDGamepad : public HID {
    EpInfo epInfo[totalEndpoints];

    HIDReportParser *pRptParser;

    uint32_t bConfNum;      // configuration number
    uint32_t bIfaceNum;     // Interface Number
    uint32_t bNumIface;     // number of interfaces in the configuration
    uint32_t bNumEP;        // total number of EP in the configuration
    uint32_t qNextPollTime; // next poll time
    bool bPollEnable;       // poll enable flag

    void Initialize();

    virtual HIDReportParser *GetReportParser(uint32_t id) {
        return pRptParser;
    };

  public:
    HIDGamepad(USBHost *p);

    virtual bool SetReportParser(uint32_t id, HIDReportParser *prs) {
        pRptParser = prs;
        return true;
    };

    // USBDeviceConfig implementation
    virtual uint32_t Init(uint32_t parent, uint32_t port, uint32_t lowspeed);
    virtual uint32_t Release();
    virtual uint32_t Poll();
    virtual uint32_t GetAddress() { return bAddress; };

    // UsbConfigXtracter implementation
    virtual void EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt,
                                uint32_t proto,
                                const USB_ENDPOINT_DESCRIPTOR *ep);
};

#endif /* HIDGAMEPAD_H_INCLUDED */
