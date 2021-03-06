// SPDX-License-Identifier: MIT
// Based on the HM-18/HM-19 datasheet
// HM-18 HM-19_en-V1.pdf

#ifndef BLEUART_H_INCLUDED
#define BLEUART_H_INCLUDED

#include <Arduino.h>
#include <stdint.h>

#define BLE_MAX_DISCOVERED_DEVICES 16
#define BLE_STARTUP_DELAY 200
#define BLE_TIMEOUT 100
// Some devices require a longer timeout
//#define BLE_TIMEOUT 250

class Modem {
  public:
    enum class adv_interval_t : uint8_t {
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

    enum class advertising_t : uint8_t {
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

    enum class ll_connect_interval_t : uint8_t {
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

    enum class role_t : uint8_t { cPeripheral = 0, cCentral };

    enum class talk_t : uint8_t {
        cWriteWithoutResponse = 0,
        cWriteWithResponse,
        cWriteResponseOptional
    };

    struct device_t {
        uint8_t id;
        String mac;
        String name;
    };

    enum class output_state_t : uint8_t {
        cPio2LowPio3Low = 0,
        cPio2LowPio3High,
        cPio2HighPio3Low,
        cPio2HighPio3High
    };

    enum class system_key_t : uint8_t { cCancel = 0, cFactoryReset };

    enum class response_t : uint8_t {
        cReconnecting,
        cConnecting,
        cError,
        cFail,
        cNoAddress,
        cOther
    };

    enum class mode_t : uint8_t {
        cSendAndReceive = 0,
        cSendOnlyRemoteConfigures,
        cSendOnlyRemoteQueries,
    };

    enum class rate_t : uint8_t {
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

    enum class parity_t : uint8_t { cNone = 0, cEven, cOdd };

    enum class stop_t : uint8_t { cOneBit = 0, cTwoBits };

  private:
    Stream &stream;
    int statePin;
    bool stateBlinks;

    unsigned long commandTimeout;
    void startCommandTimer(unsigned long);
    void expireCommandTimer();
    bool commandTimedOut();

    String responseBuf;

    bool discoveringDevices;
    device_t devices[BLE_MAX_DISCOVERED_DEVICES];
    uint8_t deviceCount;
    void parseDiscoveredDevice(const String &);

    // Notifications
    bool connected;
    String remoteAddress;

  public:
    Modem(Stream &stream) : Modem(stream, -1){};

    Modem(Stream &stream, int state_pin)
        : stream(stream), statePin(statePin), discoveringDevices(false),
          deviceCount(0), stateBlinks(true), connected(false), remoteAddress() {

        delay(BLE_STARTUP_DELAY);
        enableStateDetection(state_pin);
    };

    void enableStateDetection(int pin);
    void statePinBlinks(bool);
    bool readState();

    // Convenience methods for switching between BLE peripheral and Central
    bool makeCentral();
    bool makePeripheral(bool autostart);

    bool isConnected();

    // Documented modem commands
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

    bool getLinkLayerConnectionSlaveLatency(uint8_t &);
    bool setLinkLayerConnectionSlaveLatency(uint8_t);

    // bool getUnknownInterval(uint8_t &);
    // bool setUnknownInterval(uint8_t);

    bool getUpdateConnection(bool &);
    bool setUpdateConnection(bool);

    bool clearLastConnectedDeviceAddress();
    bool reconnect(response_t &, bool waitConnected);
    bool connectId(uint8_t, response_t &, bool waitConnected);
    bool connectAddress(const String &, response_t &, bool waitConnected);
    // bool connectRandom(response_t&);

    bool discoverDevices();
    bool discoverDevicesAsync();
    bool discoverDevicesAsyncComplete();
    uint8_t devicesCount();
    bool getDevice(uint8_t, device_t &);

    bool characteristic2Enabled(bool &);
    bool enableCharacteristic2(bool);

    // AT+FLOW? not supported yet as of V1 datasheet
    // AT+FLOW[0-1] not supported yet as of V1 datasheet

    bool highTxGainEnabled(bool &);
    bool enableHighTxGain(bool);

    bool highRxGainEnabled(bool &);
    bool enableHighRxGain(bool);

    String help();

    bool manualStartupEnabled(bool &);
    bool enableManualStartup(bool);

    bool getUartMode(mode_t &);
    bool setUartMode(mode_t);

    bool notificationsEnabled(bool &);
    bool enableNotifications(bool);
    bool notifyAddressOnDisconnectEnabled(bool &);
    bool enableNotifyAddressOnDisconnect(bool);

    bool getModuleName(String &);
    bool setModuleName(const String &);

    bool getParity(parity_t &);
    bool setParity(parity_t);

    bool getPio(uint8_t, bool &);
    bool setPio(uint8_t, bool);

    bool autoSleepEnabled(bool &);
    bool enableAutoSleep(bool);

    bool getModulePower(power_t &);
    bool setModulePower(power_t);

    bool reliableAdvertisingEnabled(bool &);
    bool enableReliableAdvertising(bool);

    bool resetConfiguration();
    bool softReset();

    bool getRole(role_t &);
    bool setRole(role_t);

    bool getLastConnected(String &);

    bool getTalkMethod(talk_t &);
    bool setTalkMethod(talk_t);

    bool getSystemKeySetting(system_key_t &);
    bool setSystemKeySetting(system_key_t);

    bool getStopBit(stop_t &);
    bool setStopBit(stop_t);

    bool sleep();

    bool start();

    bool getScanDuration(uint8_t &);
    bool setScanDuration(uint8_t);

    bool saveLastDeviceEnabled(bool &);
    bool enableSaveLastDevice(bool);

    bool nameDiscoveryEnabled(bool &);
    bool enableNameDiscovery(bool);

    bool getServiceUuid(String &);
    bool setServiceUuid(const String &);

    bool getFirmwareVersion(String &);

    // Helper functions
    bool readResponse(String &, unsigned long timeout);
    bool readResponse(String &);
    bool processResponse(String &);
    void putBackResponse(const String &);
    void drainResponses();

    bool sendCommand(const String &, String &);
    bool sendQueryCommand(const String &, String &);
    bool sendSetCommand(const String &, const String &);

    bool getHexDigit(const String &, uint8_t &);
    bool setHexDigit(const String &, uint8_t);
    bool getBool(const String &, bool &);
    bool setBool(const String &, bool);
};

#endif /* BLEUART_H_INCLUDED */
