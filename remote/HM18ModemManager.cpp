
#include "HM18ModemManager.h"

void Modem::enableStateDetection(int pinNumber) {
    statePin = pinNumber;
    pinMode(statePin, INPUT);
}

void Modem::statePinBlinks(bool blinks) { stateBlinks = blinks; }

bool Modem::readState() {
    if (statePin == -1) {
        return false;
    }
    // Serial.print("[Modem::readState] Reading from State pin ");
    // Serial.println(statePin);
    // blink = disconnected
    // solid = connected

    if (stateBlinks) {
        unsigned long p1 = pulseIn(statePin, HIGH, 2000000);
        unsigned long p2 = pulseIn(statePin, LOW, 2000000);
        if (p1 > 0 && p2 > 0) {
            Serial.println("State pin is blinking");
            return false;
        }
    }
    int state = digitalRead(statePin);
    if (state == HIGH) {
        // Serial.println("[Modem::readState] Connection state LED is solid
        // ON");
    } else {
        // Serial.println("[Modem::readState] Connection state LED is solid
        // OFF");
    }
    return state == HIGH;
}

bool Modem::makeCentral() {
    // Serial.println("[Modem::makeCentral] Ensuring we're in immediate mode");
    // AT+IMME1
    if (!enableManualStartup(true)) {
        // TODO: in case of failure, rebuild the prior state?
        Serial.println(
            "[Modem::makeCentral] Failed to switch into immediate mode");
        return false;
    }
    // Serial.println("[Modem::makeCentral] In immediate mode");

    // Serial.println("[Modem::makeCentral] Ensuring we're in central role");
    // AT+ROLE1
    if (!setRole(role_t::cCentral)) {
        Serial.println(
            "[Modem::makeCentral] Failed to switch into central role");
        return false;
    }
    // Serial.println("[Modem::makeCentral] In central role");
    return true;
}

bool Modem::makePeripheral(bool autostart) {
    // AT+IMME[0-1]
    bool want_manualstart = !autostart;
    if (!enableManualStartup(want_manualstart)) {
        // TODO: in case of failure, rebuild the prior state?
        return false;
    }
    // AT+ROLE0
    if (!setRole(role_t::cPeripheral)) {
        return false;
    }
    return true;
}

bool Modem::isConnected() {
    drainResponses();
    if (statePin != -1) {
        // Serial.println("[Modem::isConnected] Reading connection state pin");
        return readState();
    } else {
        // Serial.println("[Modem::isConnected] Cannot validate current
        // connection state, but a connection has been made before this
        // session");
        return connected;
    }
}

// AT -> OK/OK+LOST
bool Modem::disconnect() {
    String cmd;
    String resp;
    return sendCommand(cmd, resp);
}

// AT+ADDR? -> OK+ADDR:MAC Address
bool Modem::getAddress(String &addr) {
    String cmd("ADDR");
    return sendQueryCommand(cmd, addr);
}

// AT+ADVI? -> OK+Get:[0-F]
bool Modem::getAdvertisingInterval(adv_interval_t &interval) {
    String cmd("ADVI");
    return getHexDigit(cmd, (uint8_t &)interval);
}

// AT+ADVI[0-F] -> OK+Set:[0-F]
bool Modem::setAdvertisingInterval(adv_interval_t interval) {
    String cmd("ADVI");
    return setHexDigit(cmd, (uint8_t)interval);
}

// AT+ADTY? -> OK+Get:[0-3]
bool Modem::getAdvertisingType(advertising_t &type) {
    String cmd("ADTY");
    return getHexDigit(cmd, (uint8_t &)type);
}

// AT+ADTY[0-3] -> OK+Set:[0-3]
bool Modem::setAdvertisingType(advertising_t type) {
    String cmd("ADTY");
    return setHexDigit(cmd, (uint8_t)type);
}

// AT+AFTC? -> OK+Get:[0-3] void
bool Modem::getConnectedModulePinOutputState(output_state_t &state) {
    String cmd("AFTC");
    return getHexDigit(cmd, (uint8_t &)state);
}

// AT+AFTC[0-3] -> OK+Set:[0-3]
bool Modem::setConnectedModulePinOutputState(output_state_t state) {
    String cmd("AFTC");
    return setHexDigit(cmd, (uint8_t)state);
}

// AT+BEFC?  -> OK+Get:[0-3]
bool Modem::getInitialModulePinOutputState(output_state_t &state) {
    String cmd("BEFC");
    return getHexDigit(cmd, (uint8_t &)state);
}

// AT+BEFC[0-3] -> OK+Set:[0-3]
bool Modem::setInitialModulePinOutputState(output_state_t state) {
    String cmd("BEFC");
    return setHexDigit(cmd, (uint8_t)state);
}

// AT+BAUD? -> OK+Get:[0-8]
bool Modem::getBaudRate(rate_t &rate) {
    String cmd("BAUD");
    return getHexDigit(cmd, (uint8_t &)rate);
}

// AT+BAUD[0-8] -> OK+Set:[0-8]
bool Modem::setBaudRate(rate_t rate) {
    String cmd("BAUD");
    return setHexDigit(cmd, (uint8_t)rate);
}

// AT+CHAR? -> OK+Get:'0x'[0-F]{4}
bool Modem::getCharacteristic(String &charc) {
    String cmd("CHAR");
    return sendQueryCommand(cmd, charc);
}

// AT+CHAR'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}
bool Modem::setCharacteristic(const String &charc) {
    String cmd("CHAR");
    return sendSetCommand(cmd, charc);
}

// AT+COMI? -> OK+Get:[0-9]
bool Modem::getMinimumLinkLayerConnectionInterval(
    ll_connect_interval_t &interval) {
    String cmd("COMI");
    return getHexDigit(cmd, (uint8_t &)interval);
}

// AT+COMI[0-9] -> OK+Set:[0-9]
bool Modem::setMinimumLinkLayerConnectionInterval(
    ll_connect_interval_t interval) {
    String cmd("COMI");
    return setHexDigit(cmd, (uint8_t)interval);
}

// AT+COMA? -> OK+Get:[0-9]
bool Modem::getMaximumLinkLayerConnectionInterval(
    ll_connect_interval_t &interval) {
    String cmd("COMA");
    return getHexDigit(cmd, (uint8_t &)interval);
}

// AT+COMA[0-9] -> OK+Set:[0-9]
bool Modem::setMaximumLinkLayerConnectionInterval(
    ll_connect_interval_t interval) {
    String cmd("COMA");
    return setHexDigit(cmd, (uint8_t)interval);
}

// AT+COLA? -> OK+Get:[0-4]
bool Modem::getLinkLayerConnectionSlaveLatency(uint8_t &latency) {
    String cmd("COLA");
    return getHexDigit(cmd, latency);
}

// AT+COLA[0-4] -> OK+Set:[0-4]
bool Modem::setLinkLayerConnectionSlaveLatency(uint8_t latency) {
    String cmd("COLA");
    return setHexDigit(cmd, latency);
}

// AT+COSU? -> OK+Get:[0-6]
/* Effect of command unclear, description is duplicate of another command
bool Modem::getUnknownInterval(uint8_t &interval) {
    String cmd("COSU");
    return getHexDigit(cmd, interval);
}
*/

// AT+COSU[0-6] -> OK+Set:[0-6]
/* Effect of command unclear, description is duplicate of another command
bool Modem::setUnknownInterval(uint8_t interval) {
    String cmd("COSU");
    return setHexDigit(cmd, interval);
}
*/

// AT+COUP? -> OK+Get:[0-1]
bool Modem::getUpdateConnection(bool &update) {
    String cmd("COUP");
    return getBool(cmd, update);
}

// AT+COUP[0-1] -> OK+Set:[0-1]
bool Modem::setUpdateConnection(bool update) {
    String cmd("COUP");
    return setBool(cmd, update);
}

// AT+CLEAR -> OK+CLEAR
bool Modem::clearLastConnectedDeviceAddress() {
    String cmd("CLEAR");
    String val;
    String resp;
    return sendSetCommand(cmd, val);
}

// AT+CONNL -> OK+CONN[LEFN]
bool Modem::reconnect(response_t &response, bool waitForConnection) {
    String cmd("AT+CONNL");
    Serial.println("Reconnecting...");
    drainResponses();
    startCommandTimer(BLE_TIMEOUT);
    stream.print(cmd);
    expireCommandTimer();

    String tmp;
    if (!readResponse(tmp)) {
        return false;
    }

    // If OK+CONNL, second response OK+CONNF may take ~10 seconds to be reported
    switch (tmp[strlen("OK+CONN")]) {
    case 'L':
        Serial.println("Request response: Connecting");
        response = response_t::cReconnecting;
        if (waitForConnection && readResponse(tmp, 10100) &&
            tmp.equals("OK+CONNF")) {
            response = response_t::cFail;
        }
        break;
    case 'E':
        Serial.println("Request response: Error");
        response = response_t::cError;
        break;
    case 'F':
        Serial.println("Request response: Fail");
        response = response_t::cFail;
        break;
    case 'N':
        Serial.println("Request response: No Address");
        response = response_t::cNoAddress;
        break;
    default:
        Serial.println("Request response: Unrecognized response");
        response = response_t::cOther;
        break;
    }
    return true;
}

// AT+CONN[0-5] -> OK+CONN[AEF]
bool Modem::connectId(uint8_t id, response_t &response,
                      bool waitForConnection) {
    String cmd("AT+CONN");
    String val(id, HEX);
    cmd.concat(val);
    drainResponses();
    // Serial.print("[Modem::connectId] Connecting to device with id ");
    // Serial.println(val);
    startCommandTimer(BLE_TIMEOUT);
    stream.print(cmd);
    expireCommandTimer();

    String tmp;
    if (!readResponse(tmp)) {
        return false;
    }

    // TODO: this might be wrong - I may need to watch for an OK+CONNL after
    // this to see if it was actually successful
    String code = tmp.substring(strlen("OK+CONN"));
    if (val.equals(code)) {
        // Serial.println("[Modem::connectId] Success");
        response = response_t::cConnecting;
        return true;
    }

    // OK+CONNF may take ~10 seconds to be reported
    // Serial.println("[Modem::connectId] Waiting for response code...");
    switch (code[0]) {
    case 'A':
        // Serial.println("[Modem::connectId] Connection in progress...");
        response = response_t::cConnecting;
        if (waitForConnection && readResponse(tmp, 10100) &&
            tmp.equals("OK+CONNF")) {
            response = response_t::cFail;
        }
        break;
    case 'E':
        response = response_t::cError;
        break;
    case 'F':
        response = response_t::cFail;
        break;
    default:
        response = response_t::cOther;
        break;
    }
    // Serial.println("[Modem::connectId] Command complete");
    return true;
}

// AT+CO[N1][0-2][0-9]{6} -> OK+CO[N1][N1][AEF]
bool Modem::connectAddress(const String &addr, response_t &response,
                           bool waitForConnection) {
    String cmd("AT+CON" + addr);
    drainResponses();
    // Serial.print("[Modem::connectAddress] Requesting ");
    // Serial.println(cmd);
    startCommandTimer(BLE_TIMEOUT);
    stream.print(cmd);
    expireCommandTimer();

    String tmp;
    if (!readResponse(tmp)) {
        return false;
    }

    // OK+CONNF may take ~10 seconds to be reported
    String code = tmp.substring(strlen("OK+CONN"));
    switch (code[0]) {
    case 'A':
        response = response_t::cConnecting;
        // Serial.println("[Modem::connectAddress] Waiting for connection
        // validation...");
        if (waitForConnection && readResponse(tmp, 10100) &&
            tmp.equals("OK+CONNF")) {
            response = response_t::cFail;
        }
        break;
    case 'E':
        response = response_t::cError;
        break;
    case 'F':
        response = response_t::cFail;
        break;
    default:
        response = response_t::cOther;
        break;
    }
    // Serial.println("[Modem::connectAddress] Command complete");
    return true;
}
// TODO: connectRandom() AT+CO1??? -> OK+CO11[AEF]

// AT+DISC? -> OK+DISCS/OK+DIS[0-F]:[0-F]{6}:-[0-9]{3}:(\w+)?/OK+DISCE
bool Modem::discoverDevices() {
    if (discoveringDevices) {
        return false;
    }

    role_t cur_role;
    if (getRole(cur_role) && cur_role != role_t::cCentral) {
        Serial.println("[Modem::discoverDevices] Attempting to discover when "
                       "not in Central role.");
    }

    deviceCount = 0;
    drainResponses();

    String tmp;
    // Serial.println("[Modem::discoverDevices] Sending AT+DISC?");
    startCommandTimer(BLE_TIMEOUT);
    stream.print("AT+DISC?");
    expireCommandTimer();
    if (!readResponse(tmp)) {
        Serial.println("[Modem::discoverDevices] Discovery request failed");
        return false;
    }
    // Serial.print("[Modem::discoverDevices] Discovery request response: ");
    // Serial.println(tmp);
    // Serial.println("[Modem::discoverDevices] Waiting for start-of-discovery
    // notification...");
    while (!tmp.equals("OK+DISCS")) {
        if (!readResponse(tmp)) {
            Serial.println("[Modem::discoverDevices] Timed out waiting for "
                           "start-of-discovery notification");
            return false;
        }
        // Serial.print("[Modem::discoverDevices] new notification: ");
        // Serial.println("tmp");
    }
    // Serial.println("[Modem::discoverDevices] Discovery started");
    // We got the "DISCS" response... we can't get off this train until we get
    // OK+DISCE, or readResponse fails
    startCommandTimer(5050);

    while (1) {
        if (!readResponse(tmp)) {
            // Serial.println("[Modem::discoverDevices] No new responses
            // found");
            yield();
            continue;
        }
        if (tmp.equals("OK+DISCE") || commandTimedOut()) {
            break;
        }

        // Serial.print("[Modem::discoverDevices] Discovery result: ");
        // Serial.println(tmp);
        if (tmp.startsWith("OK+DIS")) {
            String deviceData = tmp.substring(strlen("OK+DIS"));
            // Serial.print("[Modem::discoverDevices] Sending device data for
            // parsing: "); Serial.println(deviceData);
            parseDiscoveredDevice(deviceData);
        } else if (tmp.length() == 0) {
            continue;
        } else {
            Serial.print(
                "[Modem::discoverDevices] Unhandled discovery entry: ");
            Serial.println(tmp);
        }
    }
    // Serial.print("[Modem::discoverDevices] Discovery completed. Found ");
    // Serial.print(deviceCount);
    // Serial.println(" devices.");
    return true;
}

bool Modem::discoverDevicesAsync() {
    if (discoveringDevices) {
        return false;
    }

    deviceCount = 0;
    drainResponses();

    String tmp;
    startCommandTimer(BLE_TIMEOUT);
    stream.print("AT+DISC?");
    expireCommandTimer();
    if (!readResponse(tmp)) {
        return false;
    }
    while (!tmp.equals("OK+DISCS")) {
        if (!readResponse(tmp)) {
            return false;
        }
    }
    discoveringDevices = true;
    return true;
}

bool Modem::discoverDevicesAsyncComplete() {
    drainResponses();
    return discoveringDevices;
}

uint8_t Modem::devicesCount() { return deviceCount; }

bool Modem::getDevice(uint8_t id, device_t &device) {
    if (id >= deviceCount) {
        return false;
    } else {
        device = devices[id];
        return true;
    }
}

// AT+FFE2? -> OK+Get:[0-1]
bool Modem::characteristic2Enabled(bool &enabled) {
    String cmd("FFE2");
    return getBool(cmd, enabled);
}

// AT+FFE2[0-1] -> OK+Set:[0-1]
bool Modem::enableCharacteristic2(bool enable) {
    String cmd("FFE2");
    return setBool(cmd, enable);
}

// AT+FLOW? not supported yet as of V1 datasheet
// AT+FLOW[0-1] not supported yet as of V1 datasheet

// This command doesn't seem to be recognized by my HM-19 modem
// AT+GAIT? -> OK+Get:[0-1]
bool Modem::highTxGainEnabled(bool &enabled) {
    String cmd("GAIT");
    return getBool(cmd, enabled);
}

// This command doesn't seem to be recognized by my HM-19 modem
// AT+GAIT[0-1] -> OK+Set:[0-1]
bool Modem::enableHighTxGain(bool enable) {
    String cmd("GAIT");
    return setBool(cmd, enable);
}

// AT+GAIN? -> OK+Get:[0-1]
bool Modem::highRxGainEnabled(bool &enabled) {
    String cmd("GAIN");
    return getBool(cmd, enabled);
}

// AT+GAIN[0-1] -> OK+Set:[0-1]
bool Modem::enableHighRxGain(bool enable) {
    String cmd("GAIN");
    return setBool(cmd, enable);
}

// AT+HELP? -> String
String Modem::help() {
    drainResponses();
    startCommandTimer(BLE_TIMEOUT);
    stream.print("AT+HELP?");
    expireCommandTimer();
    String tmp = stream.readString();
    int start = tmp.indexOf("OK");
    if (start >= 0) {
        String resp = tmp.substring(start);
        putBackResponse(resp);
        tmp = tmp.substring(0, start);
    }
    return tmp;
}

// AT+IMME? -> OK+Get:[0-1]
bool Modem::manualStartupEnabled(bool &enabled) {
    String cmd("IMME");
    return getBool(cmd, enabled);
}

// AT+IMME[0-1] -> OK+Set:[0-1]
bool Modem::enableManualStartup(bool enable) {
    String cmd("IMME");
    return setBool(cmd, enable);
}

// AT+MODE? -> OK+Get:[0-2]
bool Modem::getUartMode(mode_t &mode) {
    String cmd("MODE");
    return getHexDigit(cmd, (uint8_t &)mode);
}

// AT+MODE[0-2] -> OK+Set:[0-2]
bool Modem::setUartMode(mode_t mode) {
    String cmd("MODE");
    return setHexDigit(cmd, (uint8_t)mode);
}

// AT+NOTI? -> OK+Get:[0-1]
bool Modem::notificationsEnabled(bool &enabled) {
    String cmd("NOTI");
    return getBool(cmd, enabled);
}

// AT+NOTI[0-1] -> OK+Set:[0-1]
bool Modem::enableNotifications(bool enable) {
    String cmd("NOTI");
    return setBool(cmd, enable);
}

// AT+NOTP? -> OK+Get:[0-1]
bool Modem::notifyAddressOnDisconnectEnabled(bool &enabled) {
    String cmd("NOTP");
    return getBool(cmd, enabled);
}

// AT+NOTP[0-1] -> OK+Set:[0-1]
bool Modem::enableNotifyAddressOnDisconnect(bool enable) {
    String cmd("NOTP");
    return setBool(cmd, enable);
}

// AT+NAME? -> OK+NAME\w{1,13}
bool Modem::getModuleName(String &name) {
    String cmd("NAME");
    return sendQueryCommand(cmd, name);
}

// AT+NAME\w{1,13} -> OK+Set\w{1,13}
bool Modem::setModuleName(const String &name) {
    String cmd("NAME");
    return sendSetCommand(cmd, name);
}

// AT+PARI? -> OK+Get:[0-2]
bool Modem::getParity(parity_t &parity) {
    String cmd("PARI");
    return getHexDigit(cmd, (uint8_t &)parity);
}

// AT+PARI[0-2] -> OK+Set:[0-2]
bool Modem::setParity(parity_t parity) {
    String cmd("PARI");
    return setHexDigit(cmd, (uint8_t)parity);
}

// AT+PIO[1-B]? -> OK+PIO[1-B]:[0-1]
bool Modem::getPio(uint8_t id, bool &state) {
    String cmd("PIO");
    String strid(id, HEX);
    cmd.concat(strid);
    return getBool(cmd, state);
}

// AT+PIO[1-B][0-1] -> OK+PIO[1-B]:[0-1]
bool Modem::setPio(uint8_t id, bool state) {
    String cmd("PIO");
    String strid(id, HEX);
    cmd.concat(strid);
    return setBool(cmd, state);
}

// AT+PWRM? -> OK+Get:[0-1]
// Sense of 0/1 switched - 0 autosleeps, 1 doesn't
bool Modem::autoSleepEnabled(bool &enabled) {
    String cmd("PWRM");
    if (!getBool(cmd, enabled)) {
        return false;
    }
    enabled = !enabled;
    return true;
}

// AT+PWRM[0-1] -> OK+Set:[0-1]
// Sense of 0/1 switched - 0 autosleeps, 1 doesn't
bool Modem::enableAutoSleep(bool enable) {
    String cmd("PWRM");
    return setBool(cmd, !enable);
}

// AT+POWE? -> OK+Get:[0-7]
bool Modem::getModulePower(power_t &power) {
    String cmd("POWE");
    return getHexDigit(cmd, (uint8_t &)power);
}

// AT+POWE[0-7] -> OK+Set:[0-7]
bool Modem::setModulePower(power_t power) {
    String cmd("POWE");
    return setHexDigit(cmd, (uint8_t)power);
}

// AT+RELI? -> OK+Get:[0-1]
bool Modem::reliableAdvertisingEnabled(bool &enabled) {
    String cmd("RELI");
    return getBool(cmd, enabled);
}

// AT+RELI[0-1] -> OK+Set:[0-1]
bool Modem::enableReliableAdvertising(bool enable) {
    String cmd("RELI");
    return setBool(cmd, enable);
}

// AT+RENEW -> OK+RENEW
bool Modem::resetConfiguration() {
    String cmd("RENEW");
    String resp;
    return sendCommand(cmd, resp);
}

// AT+RESET -> OK+RESET
bool Modem::softReset() {
    String cmd("RESET");
    String resp;
    return sendCommand(cmd, resp);
}

// AT+ROLE? -> OK+Get:[0-1]
bool Modem::getRole(role_t &role) {
    String cmd("ROLE");
    if (!getHexDigit(cmd, (uint8_t &)role)) {
        return false;
    }
    return true;
}

// AT+ROLE[0-1] -> OK+Set:[0-1]
bool Modem::setRole(role_t role) {
    String cmd("ROLE");
    return setHexDigit(cmd, (uint8_t)role);
}

// AT+RADD? -> OK+RADD:[0-F]{6}
bool Modem::getLastConnected(String &address) {
    String cmd("RADD");
    if (!sendQueryCommand(cmd, address)) {
        return false;
    }
    if (address.equals("000000000000")) {
        address = "";
    }
    return true;
}

// AT+RESP? -> OK+Get:[0-2]
bool Modem::getTalkMethod(talk_t &method) {
    String cmd("RESP");
    return getHexDigit(cmd, (uint8_t &)method);
}

// AT+RESP[0-2] -> OK+Set:[0-2]
bool Modem::setTalkMethod(talk_t method) {
    String cmd("RESP");
    return setHexDigit(cmd, (uint8_t)method);
}

// AT+SYSK? -> OK+Get:[0-1]
bool Modem::getSystemKeySetting(system_key_t &key_setting) {
    String cmd("SYSK");
    return getHexDigit(cmd, (uint8_t &)key_setting);
}

// AT+SYSK[0-1] -> OK+Set:[0-1]
bool Modem::setSystemKeySetting(system_key_t key_setting) {
    String cmd("SYSK");
    return setHexDigit(cmd, (uint8_t)key_setting);
}

// AT+STOP? -> OK+Get:[0-1]
bool Modem::getStopBit(stop_t &stop_bit) {
    String cmd("STOP");
    return getHexDigit(cmd, (uint8_t &)stop_bit);
}

// AT+STOP[0-1] -> OK+Set:[0-1]
bool Modem::setStopBit(stop_t stop_bit) {
    String cmd("STOP");
    return setHexDigit(cmd, (uint8_t)stop_bit);
}

// AT+SLEEP -> OK+SLEEP
bool Modem::sleep() {
    String cmd("SLEEP");
    String resp;
    return sendCommand(cmd, resp);
}

// AT+START -> OK+START
bool Modem::start() {
    String cmd("START");
    String resp;
    return sendCommand(cmd, resp);
}

// AT+SCAN? -> OK+Get[1-5]
bool Modem::getScanDuration(uint8_t &seconds) {
    String cmd("SCAN");
    return getHexDigit(cmd, seconds);
}

// AT+SCAN[1-5] -> OK+Set[1-5]
bool Modem::setScanDuration(uint8_t seconds) {
    String cmd("SCAN");
    return setHexDigit(cmd, seconds);
}

// AT+SAVE? -> OK+Get:[0-1]
bool Modem::saveLastDeviceEnabled(bool &enabled) {
    String cmd("SAVE");
    return getBool(cmd, enabled);
}

// AT+SAVE[0-1] -> OK+Set:[0-1]
bool Modem::enableSaveLastDevice(bool enable) {
    String cmd("SAVE");
    return setBool(cmd, enable);
}

// AT+SHOW? -> OK+Get:[0-1]
bool Modem::nameDiscoveryEnabled(bool &enabled) {
    String cmd("SHOW");
    return getBool(cmd, enabled);
}

// AT+SHOW[0-1] -> OK+Set:[0-1]
// NOTE: Observed not to get OK responses? x1
bool Modem::enableNameDiscovery(bool enable) {
    String cmd("SHOW");
    return setBool(cmd, enable);
}

// AT+UUID? -> OK+Get:'0x'[0-F]{4}
bool Modem::getServiceUuid(String &uuid) {
    String cmd("UUID");
    return sendQueryCommand(cmd, uuid);
}

// AT+UUID'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}
bool Modem::setServiceUuid(const String &uuid) {
    String cmd("UUID");
    return sendSetCommand(cmd, uuid);
}

// AT+VERR?/AT+VERS? -> OK+Get:String
bool Modem::getFirmwareVersion(String &fwver) {
    String cmd("VERR");
    return sendQueryCommand(cmd, fwver);
}

// Helper methods
void Modem::startCommandTimer(unsigned long timeout) {
    commandTimeout = timeout + millis();
}

void Modem::expireCommandTimer() {
    while (!commandTimedOut()) {
        yield();
    }
}

bool Modem::commandTimedOut() { return millis() >= commandTimeout; }

bool Modem::readResponse(String &resp, unsigned long timeout) {
    // Save the current timeout so we can restore it after the read operation
    unsigned long expiration = millis() + timeout;
    bool ret = false;

    while (millis() < expiration) {
        if (processResponse(resp)) {
            return true;
        } else {
            yield();
        }
    }

    return ret;
}

bool Modem::readResponse(String &resp) {
    return readResponse(resp, BLE_TIMEOUT);
}

bool Modem::processResponse(String &resp) {
    // Take whatever's ready within the timeout already set, and add it to the
    // buf
    unsigned long last_size;
    do {
        last_size = responseBuf.length();
        responseBuf += stream.readString();
    } while (last_size != responseBuf.length());
    // Serial.print("[Modem::processResponse] Current buffer contents: ");
    // Serial.println(responseBuf);

    // Parse out the next response from the buffer, handling notifications,
    // until we get to the first non-notification response
    String tmp;

    int startResp = responseBuf.indexOf("OK");
    int endResp = responseBuf.indexOf("OK", startResp + 1);

    if (startResp >= 0 && endResp >= 0) {
        tmp = responseBuf.substring(startResp, endResp);
        responseBuf = responseBuf.substring(endResp);
    } else if (startResp >= 0) {
        tmp = responseBuf.substring(startResp);
        responseBuf = "";
    } else {
        // Serial.print("[Modem::processResponse] No responses found: '");
        // Serial.print(responseBuf);
        // Serial.println("'");
        return false;
    }
    tmp.trim();

    bool isNotification = false;
    // Handle notifications
    // If response is a notification, set the "ready" flag false to process
    // another response
    if (discoveringDevices) {
        if (tmp.equals("OK+DISCE")) {
            discoveringDevices = false;
            isNotification = true;
        } else if (tmp.startsWith("OK+DIS")) {
            String deviceData = tmp.substring(strlen("OK+DIS"));
            parseDiscoveredDevice(deviceData);
            isNotification = true;
        } else if (tmp.startsWith("OK+NAME:")) {
            // Requires AT+SHOW1, maybe also AT+NOTI1
            devices[deviceCount - 1].name = tmp.substring(strlen("OK+NAME:"));
            isNotification = true;
        }
    }
    if (tmp[3] == 'L' && tmp.startsWith("OK+LOST:")) {
        connected = false;
        isNotification = true;
    } else if (tmp[3] == 'C' && tmp.equals("OK+CONN")) {
        connected = true;
        isNotification = true;
    }

    if (isNotification) {
        // Serial.print("[Modem::processResponse] Handled notification: ");
        // Serial.println(tmp);
        return false;
    } else {
        resp = tmp;
        // Serial.print("[Modem::processResponse] Returning response ");
        // Serial.println(resp);
        return true;
    }
}

void Modem::putBackResponse(const String &resp) {
    responseBuf = resp + responseBuf;
}

void Modem::drainResponses() {
    // We could just erase the buffer, but we want to make
    // sure notifications still get handled
    String tmp;
    while (readResponse(tmp, 1)) {
        Serial.print("[Modem::drainResponse] Drained response ");
        Serial.println(tmp);
        yield();
    }
    // All notifications handled.  Burn that sucker to the ground!
    responseBuf = "";
    return;
}

void Modem::parseDiscoveredDevice(const String &deviceData) {
    long start, end;
    start = 0;
    end = deviceData.indexOf(':', start);
    uint8_t idx = deviceData.substring(start, end).toInt();
    devices[idx].id = idx;

    start = end + 1;
    end = deviceData.indexOf(':', start);
    devices[idx].mac = deviceData.substring(start, end);

    start = end + 1;
    end = deviceData.indexOf(':', start);
    // devices[idx].power = deviceData.substring(start, end);

    start = end + 1;
    end = deviceData.indexOf(':', start);
    devices[idx].name = deviceData.substring(start, end);

    /*
    Serial.print("[Modem::parseDiscoveredDevices] Found device ");
    Serial.print(idx);
    Serial.print(" MAC ");
    Serial.print(devices[idx].mac);
    Serial.print(" [");
    Serial.print(devices[idx].name);
    Serial.println("]");
    */

    deviceCount = idx + 1;
}

bool Modem::sendCommand(const String &args, String &response) {
    drainResponses();
    String command("AT");
    if (args.length()) {
        command.concat("+");
    }
    // Serial.print("[Modem::sendCommand] Sending command ");
    // Serial.println(command+args);
    startCommandTimer(BLE_TIMEOUT);
    stream.print(command + args);
    expireCommandTimer();
    String tmp;
    if (!readResponse(tmp)) {
        Serial.print("[Modem::sendCommand] No response to command ");
        Serial.println(command + args);
        return false;
    }
    response = tmp.substring(command.length());
    // Serial.print("[Modem::sendCommand] Successful command. Response ");
    // Serial.println(response);
    return true;
}

bool Modem::sendQueryCommand(const String &cmd, String &response) {
    String query(cmd + "?");
    String tmp;
    // Serial.print("[Modem::sendQueryCommand] ");
    // Serial.println(query);
    if (!sendCommand(query, tmp)) {
        Serial.println("[Modem::sendQueryCommand] Failed.");
        return false;
    }
    // Serial.print("[Modem::sendQueryCommand] Query response: ");
    // Serial.println(tmp);
    while (!tmp.startsWith("Get:")) {
        // Serial.print("[Modem::sendQueryCommand] Invalid reply to Get request:
        // "); Serial.println(tmp);
        if (!readResponse(tmp)) {
            return false;
        }
    }
    response = tmp.substring(strlen("Get:"));
    // Serial.print("[Modem::sendQueryCommand] Successful reply ");
    // Serial.println(response);
    return true;
}

bool Modem::sendSetCommand(const String &cmd, const String &args) {
    String set(cmd + args);
    String tmp;
    if (!sendCommand(set, tmp)) {
        // Serial.print("[Modem::sendSetCommand] Failed sending command ");
        // Serial.println(set);
        return false;
    }
    String validreply("Set:" + args);
    while (!tmp.equals(validreply)) {
        // Serial.print("[Modem::sendSetCommand] Invalid reply to Set request:
        // "); Serial.println(tmp); Serial.print("[Modem::sendSetCommand]
        // Expected reply: "); Serial.println(validreply);
        if (!readResponse(tmp)) {
            return false;
        }
    }
    return true;
}

bool Modem::getHexDigit(const String &cmd, uint8_t &val) {
    String tmp;
    // Serial.print("[Modem::getHexDigit] ");
    // Serial.println(cmd);
    if (!sendQueryCommand(cmd, tmp)) {
        // Serial.println("[Modem::getHexDigit] Failed");
        return false;
    }
    while (tmp.length() != 1) {
        // Serial.print("[Modem::getHexDigit] Discarded response for being the
        // wrong size: "); Serial.println(tmp);
        if (!readResponse(tmp)) {
            return false;
        }
    }
    val = strtoul(tmp.c_str(), NULL, 16);
    return true;
}

bool Modem::setHexDigit(const String &cmd, uint8_t val) {
    String intval(val, HEX);
    return sendSetCommand(cmd, intval);
}

bool Modem::getBool(const String &cmd, bool &val) {
    uint8_t tmp;
    if (!getHexDigit(cmd, tmp)) {
        return false;
    }
    val = (bool)tmp;
    return true;
}

bool Modem::setBool(const String &cmd, bool val) {
    return setHexDigit(cmd, (uint8_t)val);
}
