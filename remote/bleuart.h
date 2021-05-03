// SPDX-License-Identifier: MIT
// Based on the HM-18/HM-19 datasheet
// HM-18 HM-19_en-V1.pdf

#ifndef BLEUART_H_INCLUDED
#define BLEUART_H_INCLUDED

#include <Arduino.h>
#include <stdint.h>

#define BLE_MAX_DISCOVERED_DEVICES 12

class Modem {
  public:
    enum class adv_interval_t {
        c100ms = 0,
        c152_5ms,
        c211_25ms,
        c318_75ms,
        c417_5ms,
        c546_25ms,
        c760ms,
        c852_5ms,
        c1022_5ms,
        c1285ms,
        c2000ms,
        c3000ms,
        c4000ms,
    };

    enum class advertising_t {
        cAdvertisingScanResponseConnectable = 0,
        cOnlyAllowLastDevice,
        cAdvertisingScanResponse,
        cAdvertising
    };

    enum class power_t {
        cN18dbm = 0,
        cN12dbm,
        cN6dbm,
        cN3dbm,
        cN2dbm,
        cN1dbm,
        c0dbm,
        cP3dbm
    };

    enum class ll_connect_interval_t {
        c7_5ms = 0,
        c10ms,
        c15ms,
        c20ms,
        c25ms,
        c30ms,
        c35ms,
        c40ms,
        c45ms,
        c4000ms
    };

    enum class role_t { cPeripheral = 0, cCentral };

    enum class talk_t {
        cWriteWithoutResponse = 0,
        cWriteWithResponse,
        cWriteResponseOptional
    };

    struct device_t {
        uint8_t id;
        String mac;
        String name;
    };

    struct beacon_t {
        uint8_t id;
        String factory_id;
        String uuid;
        String majorVersion;
        String minorVersion;
        String power;
        String mac;
        String rssi;
    };

    enum class output_state_t {
        cPio2LowPio3Low = 0,
        cPio2LowPio3High,
        cPio2HighPio3Low,
        cPio2HighPio3High
    };

    enum class system_key_t { cCancel = 0, cFactoryReset };

    enum class response_t {
        cReconnecting,
        cConnecting,
        cError,
        cFail,
        cNoAddress,
        cOther
    };

    enum class mode_t {
        cSendAndReceive = 0,
        cSendOnlyRemoteConfigures,
        cSendOnlyRemoteQueries,
    };

    enum class rate_t {
        c1200 = 0,
        c2400,
        c4800,
        c9600,
        c19200,
        c38400,
        c57600,
        c115200,
        c230400
    };

    enum class parity_t { cNone = 0, cEven, cOdd };

    enum class stop_t { cOneBit = 0, cTwoBits };

    Modem(Stream &stream) : stream(stream), deviceCount(0), beaconCount(0){};

    bool readResponse(String &);
    bool issueCommand(const String &, String &);

    bool issueQuery(const String &, const String &, String &);
    bool issueGet(const String &, String &);
    bool issueSet(const String &, const String &, String &);

    bool disconnect();
    bool getAddress(String &);

    bool getAdvertisingInterval(adv_interval_t &);
    bool setAdvertisingInterval(adv_interval_t);

    bool getAdvertisingType(advertising_t &);
    bool setAdvertisingType(advertising_t);

    bool getConnectedModulePinOutputState(output_state_t &);
    bool setConnectedModulePinOutputState(output_state_t);

    bool getInitialModulePinOutputState(output_state_t &);
    bool setInitialModulePinOutputState(output_state_t);

    bool getBaudRate(rate_t &);
    bool setBaudRate(rate_t);

    bool getCharacteristic(String &);
    bool setCharacteristic(const String &);

    bool getMinimumLinkLayerConnectionInterval(ll_connect_interval_t &);
    bool setMinimumLinkLayerConnectionInterval(ll_connect_interval_t);

    bool getMaximumLinkLayerConnectionInterval(ll_connect_interval_t &);
    bool setMaximumLinkLayerConnectionInterval(ll_connect_interval_t);

    // AT+COLA? -> OK+Get:[0-4]
    // meanings of values not explained in datasheet
    bool
    getLinkLayerConnectionSlaveLatency(uint8_t &); // AT+COLA? -> OK+Get:[0-4]
    // AT+COLA? -> OK+Set:[0-4]
    // meanings of values not explained in datasheet
    bool setLinkLayerConnectionSlaveLatency(uint8_t);

    // AT+COSU? -> OK+Get:[0-6]
    // Effect of command unclear, description is duplicate of another command
    bool getUnknownInterval(uint8_t &);
    // AT+COSU[0-6] -> OK+Set:[0-6]
    // Effect of command unclear, description is duplicate of another command
    bool setUnknownInterval(uint8_t);

    bool getUpdateConnection(bool &);
    bool setUpdateConnection(bool);

    bool clearLastConnectedDeviceAddress();
    bool reconnect(response_t &);
    bool connectId(uint8_t, response_t &);
    bool connectAddress(const String &, response_t &);
    // TODO: connectRandom() AT+CO1??? -> OK+CO11[AEF]

    bool discoverDevices(); // AT+DISC? -> OK+DIS[C0-2]([SE]|[0-F]{6})
    uint8_t devicesCount();
    bool getDevice(uint8_t, device_t &);

    bool discoverBeacons(); // AT+DISI? -> OK+DISC????
    uint8_t beaconsCount();
    bool getBeacon(uint8_t, beacon_t &);

    /*
    bool characteristic2Enabled(bool&); // AT+FFE2? -> OK+Get:[0-1]
    bool enableCharacteristic2(bool); // AT+FFE2[0-1] -> OK+Set:[0-1]

    // AT+FLOW? not supported yet as of V1 datasheet
    // AT+FLOW[0-1] not supported yet as of V1 datasheet

    bool highTxGainEnabled(); // AT+GAIT? -> OK+Get:[0-1]
    void enableHighTxGain(bool); // AT+GAIT[0-1] -> OK+Set:[0-1]

    bool highRxGainEnabled(); // AT+GAIN? -> OK+Get:[0-1]
    void enableHighRxGain(bool); // AT+GAIN[0-1] -> OK+Set:[0-1]

    String help(); // AT+HELP? -> String

    bool manualStartupEnabled(); // AT+IMME? -> OK+Get:[0-1]
    void enableManualStartup(bool); // AT+IMME[0-1] -> OK+Set:[0-1]

    bool beaconEnabled(); // AT+IBEA? -> OK+Get:[0-1]
    void enableBeacon(bool); // AT+IBEA[0-1] -> OK+Set:[0-1]

    String getBeaconUuid(); // AT+IBE0? -> OK+Get:[0-F]{4}, AT+IBE1, AT+IBE2,
    AT+IBE3 void setBeaconUuid(String&); // AT+IBE0[0-F]{4} -> OK+Set:[0-F]{4},
    AT+IBE1, AT+IBE2, AT+IBE3

    uint16_t getBeaconMajorVersion(); // AT+MARJ? -> OK+Get:[0-F]{4}
    void setBeaconMajorVersion(uint16_t); // AT+MARJ[0-F]{4} -> OK+Set:[0-F]{4}

    uint16_t getBeaconMinorVersion(); // AT+MINO? -> OK+Get:[0-F]{4}
    void setBeaconMinorVersion(uint16_t); // AT+MINO[0-F]{4} -> OK+Set:[0-F]{4}

    uint8_t getBeaconMeasuredPower(); // AT+MEAS? -> OK+Get:[0-F]{2}
    void setBeaconMeasuredPower(uint8_t); // AT+MEAS[0-F]{2} -> OK+Set:[0-F]{2}

    mode_t getUartMode(); // AT+MODE? -> OK+Get:[0-2]
    void setUartMode(mode_t); // AT+MODE[0-2] -> OK+Set:[0-2]

    bool notificationsEnabled(); // AT+NOTI? -> OK+Get:[0-1]
    void enableNotifications(bool); // AT+NOTI[0-1] -> OK+Set:[0-1]
    bool getNotifyAddressOnDisconnect(); // AT+NOTP? -> OK+Get:[0-1]
    void setNotifyAddressOnDisconnect(bool); // AT+NOTP[0-1] -> OK+Set:[0-1]

    String getModuleName(); // AT+NAME? -> OK+NAME\w{1,13}
    void setModuleName(String&); // AT+NAME\w{1,13} -> OK+Set\w{1,13}

    parity_t getParity(); // AT+PARI? -> OK+Get:[0-2]
    void setParity(parity_t); // AT+PARI[0-2] -> OK+Set:[0-2]

    bool getPio(uint8_t); // AT+PIO[1-B]? -> OK+PIO[1-B]:[0-1]
    void setPio(uint8_t, bool); // AT+PIO[1-B][0-1] -> OK+PIO[1-B]:[0-1]

    bool autoSleepEnabled(); // AT+PWRM? -> OK+Get:[0-1]
    void enableAutoSleep(bool); // AT+PWRM[0-1] -> OK+Set:[0-1]

    Ble::power_t getModulePower(); // AT+POWE? -> OK+Get:[0-7]
    void setModulePower(Ble::power_t); // AT+POWE[0-7] -> OK+Set:[0-7]

    bool reliableAdvertisingEnabled(); // AT+RELI? -> OK+Get:[0-1]
    void enableReliableAdvertising(bool); // AT+RELI[0-1] -> OK+Set:[0-1]

    void resetConfiguration(); // AT+RENEW -> OK+RENEW
    void reset(); // AT+RESET -> OK+RESET

    Ble::role_t getRole(); // AT+ROLE? -> OK+Get:[0-1]
    void setRole(Ble::role_t); // AT+ROLE[0-1] -> OK+Set:[0-1]

    String getLastConnected(); // AT+RADD? -> OK+RADD:[0-F]{6}

    Ble::talk_t getTalkMethod(); // AT+RESP? -> OK+Get:[0-2]
    void setTalkMethod(Ble::talk_t); // AT+RESP[0-2] -> OK+Set:[0-2]

    system_key_t getSystemKeySetting(); // AT+SYSK? -> OK+Get:[0-1]
    void setSystemKeySetting(system_key_t); // AT+SYSK[0-1] ->
    OK+Set:[0-1]

    stop_t getStopBit(); // AT+STOP? -> OK+Get:[0-1]
    void setStopBit(stop_t); // AT+STOP[0-1] -> OK+Set:[0-1]

    void sleep(); // AT+SLEEP -> OK+SLEEP

    void start(); // AT+START -> OK+START

    uint8_t getScanDuration(); // AT+SCAN? -> OK+Get[1-5]
    void setScanDuration(uint8_t); // AT+SCAN[1-5] -> OK+Set[1-5]

    bool saveLastDeviceEnabled(); // AT+SAVE? -> OK+Get:[0-1]
    void enableSaveLastDevice(bool); // AT+SAVE[0-1] -> OK+Set:[0-1]

    bool nameDiscoveryEnabled(); // AT+SHOW? -> OK+Get:[0-1]
    void enableNameDiscovery(bool); // AT+SHOW[0-1] -> OK+Set:[0-1]

    uint16_t getServiceUuid(); // AT+UUID? -> OK+Get:'0x'[0-F]{4}
    void setServiceUuid(uint16_t); // AT+UUID'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}

    String getFirmwareVersion(); // AT+VERR/AT+VERS -> String
    */

  private:
    Stream &stream;
    device_t devices[BLE_MAX_DISCOVERED_DEVICES];
    uint8_t deviceCount;
    beacon_t beacons[BLE_MAX_DISCOVERED_DEVICES];
    uint8_t beaconCount;
};

#endif /* BLEUART_H_INCLUDED */
