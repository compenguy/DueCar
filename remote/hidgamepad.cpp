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
#include "hidgamepad.h"

/**
 * \brief HIDGamepad class constructor.
 */
HIDGamepad::HIDGamepad(USBHost *p)
    : HID(p), pRptParser(NULL), qNextPollTime(0), bPollEnable(false) {
    Initialize();

    if (pUsb)
        pUsb->RegisterDeviceClass(this);
}

/**
 * \brief Initialize HIDGamepad class.
 */
void HIDGamepad::Initialize() {
    for (uint32_t i = 0; i < totalEndpoints; ++i) {
        epInfo[i].deviceEpNum = 0;
        epInfo[i].hostPipeNum = 0;
        epInfo[i].maxPktSize = (i) ? 0 : 8;
        epInfo[i].epAttribs = 0;
        epInfo[i].bmNakPower = (i) ? USB_NAK_NOWAIT : USB_NAK_MAX_POWER;
    }

    bNumEP = 1;
    bNumIface = 0;
    bConfNum = 0;
}

/**
 * \brief Initialize connection to an HID device.
 *
 * \param parent USB device address of the Parent device.
 * \param port USB device base address.
 * \param lowspeed USB device speed.
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HIDGamepad::Init(uint32_t parent, uint32_t port, uint32_t lowspeed) {
    const uint32_t constBufSize = sizeof(USB_DEVICE_DESCRIPTOR);

    uint8_t buf[constBufSize];
    uint32_t rcode = 0;
    UsbDevice *p = 0;
    EpInfo *oldep_ptr = 0;
    uint32_t len = 0;

    uint32_t num_of_conf = 0; // number of configurations

    AddressPool &addrPool = pUsb->GetAddressPool();

    TRACE_USBHOST(printf("HIDGamepad::Init\r\n");)

    if (bAddress)
        return USB_ERROR_CLASS_INSTANCE_ALREADY_IN_USE;

    // Get pointer to pseudo device with address 0 assigned
    p = addrPool.GetUsbDevicePtr(0);

    if (!p)
        return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

    if (!p->epinfo) {
        TRACE_USBHOST(printf("HIDGamepad::Init : epinfo is null!\r\n");)
        return USB_ERROR_EPINFO_IS_NULL;
    }

    // Save old pointer to EP_RECORD of address 0
    oldep_ptr = p->epinfo;

    // Temporary assign new pointer to epInfo to p->epinfo in order to avoid
    // toggle inconsistence
    p->epinfo = epInfo;

    p->lowspeed = lowspeed;

    // Get device descriptor
    rcode = pUsb->getDevDescr(0, 0, 8, (uint8_t *)buf);

    if (!rcode)
        len = (buf[0] > constBufSize) ? constBufSize : buf[0];

    if (rcode) {
        // Restore p->epinfo
        p->epinfo = oldep_ptr;

        goto FailGetDevDescr;
    }

    // Restore p->epinfo
    p->epinfo = oldep_ptr;

    // Allocate new address according to device class
    bAddress = addrPool.AllocAddress(parent, false, port);

    if (!bAddress)
        return USB_ERROR_OUT_OF_ADDRESS_SPACE_IN_POOL;

    // Extract Max Packet Size from the device descriptor
    epInfo[0].maxPktSize =
        (uint8_t)((USB_DEVICE_DESCRIPTOR *)buf)->bMaxPacketSize0;

    // Assign new address to the device
    rcode = pUsb->setAddr(0, 0, bAddress);

    if (rcode) {
        p->lowspeed = false;
        addrPool.FreeAddress(bAddress);
        bAddress = 0;
        TRACE_USBHOST(
            printf("HIDBoot::Init : setAddr failed with rcode %lu\r\n", rcode);)
        return rcode;
    }

    TRACE_USBHOST(
        printf("HIDBoot::Init : device address is now %lu\r\n", bAddress);)

    p->lowspeed = false;

    p = addrPool.GetUsbDevicePtr(bAddress);

    if (!p)
        return USB_ERROR_ADDRESS_NOT_FOUND_IN_POOL;

    p->lowspeed = lowspeed;

    if (len)
        rcode = pUsb->getDevDescr(bAddress, 0, len, (uint8_t *)buf);

    if (rcode)
        goto FailGetDevDescr;

    num_of_conf = ((USB_DEVICE_DESCRIPTOR *)buf)->bNumConfigurations;

    // Assign epInfo to epinfo pointer
    rcode = pUsb->setEpInfoEntry(bAddress, 1, epInfo);

    if (rcode)
        goto FailSetDevTblEntry;

    TRACE_USBHOST(
        printf("HIDGamepad::Init : number of configuration is %lu\r\n",
               num_of_conf);)

    for (uint32_t i = 0; i < num_of_conf; ++i) {
        ConfigDescParser<USB_CLASS_HID, HID_NO_INTF_SUBCLASS, HID_PROTOCOL_NONE,
                         CP_MASK_COMPARE_ALL>
            confDescrParser(this);

        rcode = pUsb->getConfDescr(bAddress, 0, i, &confDescrParser);

        if (bNumEP > 1)
            break;
    }

    if (bNumEP < 2)
        return USB_DEV_CONFIG_ERROR_DEVICE_NOT_SUPPORTED;

    TRACE_USBHOST(printf("HIDGamepad::Init : bAddress: %lu\r\n", bAddress);)
    TRACE_USBHOST(printf("HIDGamepad::Init : bNumEP: %lu\r\n", bNumEP);)

    // Assign epInfo to epinfo pointer
    rcode = pUsb->setEpInfoEntry(bAddress, bNumEP, epInfo);

    TRACE_USBHOST(printf("HIDGamepad::Init : bConfNum: %lu\r\n", bConfNum);)

    // Set Configuration Value
    rcode = pUsb->setConf(bAddress, 0, bConfNum);

    if (rcode)
        goto FailSetConfDescr;

    TRACE_USBHOST(printf("HIDGamepad::Init : bIfaceNum: %lu\r\n", bIfaceNum);)

    rcode = SetProtocol(bIfaceNum, HID_PROTOCOL_NONE);

    if (rcode)
        goto FailSetProtocol;

    TRACE_USBHOST(
        printf("HIDGamepad::Init : HID device configured successfully\r\n");)

    bPollEnable = true;
    return 0;

FailGetDevDescr:
    TRACE_USBHOST(printf("HIDGamepad::Init getDevDescr : ");)
    goto Fail;

FailSetDevTblEntry:
    TRACE_USBHOST(printf("HIDGamepad::Init setDevTblEn : ");)
    goto Fail;

FailSetProtocol:
    TRACE_USBHOST(printf("HIDGamepad::Init SetProto : ");)
    goto Fail;

FailSetConfDescr:
    TRACE_USBHOST(printf("HIDGamepad::Init setConf : ");)
    goto Fail;

Fail:
    TRACE_USBHOST(printf("error code: %lu\r\n", rcode);)
    Release();
    return rcode;
}

/**
 * \brief Extract interrupt-IN endpoint information from configuration
 * descriptor.
 *
 * \param conf Configuration number.
 * \param iface Interface number.
 * \param alt Alternate setting.
 * \param proto Protocol version used.
 * \param pep Pointer to endpoint descriptor.
 */
void HIDGamepad::EndpointXtract(uint32_t conf, uint32_t iface, uint32_t alt,
                                uint32_t proto,
                                const USB_ENDPOINT_DESCRIPTOR *pep) {
    // If the first configuration satisfies, the others are not considered.
    if (bNumEP > 1 && conf != bConfNum)
        return;

    bConfNum = conf;
    bIfaceNum = iface;

    uint32_t index = 0;
    uint32_t pipe = 0;

    if ((pep->bmAttributes & 0x03) == 3 &&
        (pep->bEndpointAddress & 0x80) == 0x80) {
        index = epInterruptInIndex;

        // Fill in the endpoint info structure
        epInfo[index].deviceEpNum = (pep->bEndpointAddress & 0x0F);
        epInfo[index].maxPktSize = (uint8_t)pep->wMaxPacketSize;
        epInfo[index].epAttribs = 0;

        TRACE_USBHOST(
            printf("HIDGamepad::EndpointXtract : Found new endpoint\r\n");)
        TRACE_USBHOST(
            printf("HIDGamepad::EndpointXtract : deviceEpNum: %lu\r\n",
                   epInfo[index].deviceEpNum);)
        TRACE_USBHOST(printf("HIDGamepad::EndpointXtract : maxPktSize: %lu\r\n",
                             epInfo[index].maxPktSize);)
        TRACE_USBHOST(
            printf("HIDGamepad::EndpointXtract : index: %lu\r\n", index);)

        // Ensure pipe allocation is okay
        pipe = UHD_Pipe_Alloc(
            bAddress, epInfo[index].deviceEpNum, UOTGHS_HSTPIPCFG_PTYPE_INTRPT,
            UOTGHS_HSTPIPCFG_PTOKEN_IN, epInfo[index].maxPktSize, 10,
            UOTGHS_HSTPIPCFG_PBK_1_BANK);
        if (pipe == 0) {
            TRACE_USBHOST(printf("HIDGamepad::EndpointXtract : Pipe allocation "
                                 "failure\r\n");)
            // Enumeration failed
            return;
        }

        epInfo[index].hostPipeNum = pipe;

        bNumEP++;
    }
}

/**
 * \brief Release USB allocated resources (pipes and address).
 *
 * \note Release call is made from USBHost.task() on disconnection events.
 * \note Release call is made from Init() on enumeration failure.
 *
 * \return Always 0.
 */
uint32_t HIDGamepad::Release() {
    // Free allocated host pipes
    UHD_Pipe_Free(epInfo[epInterruptInIndex].hostPipeNum);

    // Free allocated USB address
    pUsb->GetAddressPool().FreeAddress(bAddress);

    bConfNum = 0;
    bIfaceNum = 0;
    bNumEP = 1;
    bAddress = 0;
    qNextPollTime = 0;
    bPollEnable = false;

    return 0;
}

/**
 * \brief Poll USB device activity.
 *
 * \note Poll call is periodically made from USBHost.task().
 *
 * \return 0 on success, error code otherwise.
 */
uint32_t HIDGamepad::Poll() {
    uint32_t rcode = 0;

    if (!bPollEnable)
        return 0;

    if (qNextPollTime <= millis()) {
        qNextPollTime = millis() + 10;

        const uint32_t const_buff_len = 16;
        uint8_t buf[const_buff_len];

        uint32_t read = epInfo[epInterruptInIndex].maxPktSize;

        rcode = pUsb->inTransfer(
            bAddress, epInfo[epInterruptInIndex].deviceEpNum, &read, buf);

        if (rcode) {
            return rcode;
        }

        if (pRptParser) {
            pRptParser->Parse((HID *)this, 0, (uint32_t)read, buf);
        }
    }

    return rcode;
}

/**
 * \brief Parse HID gamepad report.
 *
 * \param hid HID device pointer.
 * \param is_rpt_id True if this is a report ID.
 * \param len Buffer length.
 * \param buf Buffer containing report data.
 */
void GamepadReportParser::Parse(HID *hid, bool is_rpt_id, uint32_t len,
                                uint8_t *buf) {
    OnUnknownReport(len, buf);
};
