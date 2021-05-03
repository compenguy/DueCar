// SPDX-License-Identifier: MIT
// Based on the HM-18/HM-19 datasheet
// HM-18 HM-19_en-V1.pdf

#ifndef BLEUART_H_INCLUDED
#define BLEUART_H_INCLUDED

#include <Arduino.h>
#include <stdint.h>

#define BLE_MAX_DISCOVERED_DEVICES 12

namespace Ble {
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

enum advertising_t {
    cAdvertisingScanResponseConnectable = 0,
    cOnlyAllowLastDevice,
    cAdvertisingScanResponse,
    cAdvertising
};

enum power_t {
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

enum role_t { cPeripheral = 0, cCentral };

enum talk_t {
    cWriteWithoutResponse = 0,
    cWriteWithResponse,
    cWriteResponseOptional
};

struct device_t {
    uint8_t id;
    uint8_t mac[6];
    String name;
};

struct beacon_t {
    uint8_t id;
    uint8_t factory_id[8];
    uint8_t uuid[32];
    uint32_t majorVersion;
    uint32_t minorVersion;
    uint16_t power;
    uint8_t mac[12];
    uint32_t rssi;
};

namespace ModulePin {
enum output_state_t {
    cPio2LowPio3Low = 0,
    cPio2LowPio3High,
    cPio2HighPio3Low,
    cPio2HighPio3High
};

enum system_key_t { cCancel = 0, cFactoryReset };
}; // namespace ModulePin

namespace Connection {
enum response_t {
    cReconnecting,
    cConnecting,
    cError,
    cFail,
    cNoAddress,
    cOther
};
};

namespace Uart {
enum mode_t {
    cSendAndReceive = 0,
    cSendOnlyRemoteConfigures,
    cSendOnlyRemoteQueries,
};

enum rate_t {
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

enum parity_t { cNone = 0, cEven, cOdd };

enum stop_t { cOneBit = 0, cTwoBits };
}; // namespace Uart

class Modem {
  private:
    Stream &stream;
    device_t devices[BLE_MAX_DISCOVERED_DEVICES];
    beacon_t beacons[BLE_MAX_DISCOVERED_DEVICES];

  public:
    Modem(Stream &stream) : stream(stream){};

    String issueCommand(String &);
    String issueCommand(const char *);

    String issueQuery(String &, String &);
    String issueQuery(const char *, const char *);
    String issueGet(String &);
    String issueGet(const char *);
    String issueSet(String &, String &);
    String issueSet(const char *, const char *);
    bool issueSetChecked(String &, String &);

    String disconnect();
    String getAddress();

    adv_interval_t getAdvertisingInterval();
    bool setAdvertisingInterval(adv_interval_t);

    advertising_t getAdvertisingType();
    bool setAdvertisingType(advertising_t);

    ModulePin::output_state_t getConnectedModulePinOutputState();
    bool setConnectedModulePinOutputState(ModulePin::output_state_t);

    ModulePin::output_state_t getInitialModulePinOutputState();
    bool setInitialModulePinOutputState(ModulePin::output_state_t);

    Uart::rate_t getBaudRate();
    bool setBaudRate(Uart::rate_t);

    String getCharacteristic();
    bool setCharacteristic(String &);

    ll_connect_interval_t getMinimumLinkLayerConnectionInterval();
    bool setMinimumLinkLayerConnectionInterval(ll_connect_interval_t);

    ll_connect_interval_t getMaximumLinkLayerConnectionInterval();
    bool setMaximumLinkLayerConnectionInterval(ll_connect_interval_t);

    // AT+COLA? -> OK+Get:[0-4]
    // meanings of values not explained in datasheet
    uint8_t getLinkLayerConnectionSlaveLatency(); // AT+COLA? -> OK+Get:[0-4]
    // AT+COLA? -> OK+Set:[0-4]
    // meanings of values not explained in datasheet
    bool setLinkLayerConnectionSlaveLatency(uint8_t);

    // AT+COSU? -> OK+Get:[0-6]
    // Effect of command unclear, description is duplicate of another command
    uint8_t getUnknownInterval();
    // AT+COSU[0-6] -> OK+Set:[0-6]
    // Effect of command unclear, description is duplicate of another command
    bool setUnknownInterval(uint8_t);

    bool getUpdateConnection();     // AT+COUP? -> OK+Get:[0-1]
    bool setUpdateConnection(bool); // AT+COUP[0-1] -> OK+Set:[0-1]

    bool clearLastConnectedDeviceAddress();    // AT+CLEAR -> OK+CLEAR
    Connection::response_t reconnect();        // AT+CONNL -> OK+CONN[LEFN]
    Connection::response_t connectId(uint8_t); // AT+CONN[0-5] -> OK+CONN[AEF]
    Connection::response_t
    connectAddress(String &); // AT+CON[0-F]{6} -> OK+CONN[AEF]
    // TODO: connectRandom() AT+CO1??? -> OK+CO11[AEF]

    /*
    void discoverDevices(); // AT+DISC? -> OK+DIS[C0-2]([SE]|[0-F]{6})
    uint8_t devicesCount();
    device_t getDevice(uint8_t);

    void discoverBeacons(); // AT+DISI? -> OK+DISC????
    uint8_t beaconsCount();
    beacon_t getBeacon(uint8_t);

    bool characteristic2Enabled(); // AT+FFE2? -> OK+Get:[0-1]
    void enableCharacteristic2(bool); // AT+FFE2[0-1] -> OK+Set:[0-1]

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

    Uart::mode_t getUartMode(); // AT+MODE? -> OK+Get:[0-2]
    void setUartMode(Uart::mode_t); // AT+MODE[0-2] -> OK+Set:[0-2]

    bool notificationsEnabled(); // AT+NOTI? -> OK+Get:[0-1]
    void enableNotifications(bool); // AT+NOTI[0-1] -> OK+Set:[0-1]
    bool getNotifyAddressOnDisconnect(); // AT+NOTP? -> OK+Get:[0-1]
    void setNotifyAddressOnDisconnect(bool); // AT+NOTP[0-1] -> OK+Set:[0-1]

    String getModuleName(); // AT+NAME? -> OK+NAME\w{1,13}
    void setModuleName(String&); // AT+NAME\w{1,13} -> OK+Set\w{1,13}

    Uart::parity_t getParity(); // AT+PARI? -> OK+Get:[0-2]
    void setParity(Uart::parity_t); // AT+PARI[0-2] -> OK+Set:[0-2]

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

    ModulePin::system_key_t getSystemKeySetting(); // AT+SYSK? -> OK+Get:[0-1]
    void setSystemKeySetting(ModulePin::system_key_t); // AT+SYSK[0-1] ->
    OK+Set:[0-1]

    Uart::stop_t getStopBit(); // AT+STOP? -> OK+Get:[0-1]
    void setStopBit(Uart::stop_t); // AT+STOP[0-1] -> OK+Set:[0-1]

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
};

}; // namespace Ble

#endif /* BLEUART_H_INCLUDED */
