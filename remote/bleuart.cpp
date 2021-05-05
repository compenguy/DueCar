
#include "bleuart.h"

bool Modem::makeCentral() {
    bool dirty = false;
    // AT+IMME1
    bool cur_manualstart;
    if (manualStartupEnabled(cur_manualstart) && !cur_manualstart) {
        if (!enableManualStartup(true)) {
            // TODO: in case of failure, rebuild the prior state?
            return false;
        }
        dirty = true;
    }
    // AT+ROLE1
    role_t cur_role;
    if (getRole(cur_role) && cur_role != role_t::cCentral) {
        if (!setRole(role_t::cCentral)) {
            return false;
        }
        dirty = true;
    }
    if (dirty) {
        reset();
    }
}

bool Modem::makePeripheral(bool autostart) {
    bool dirty = false;
    // AT+IMME[0-1]
    bool cur_manualstart;
    bool want_manualstart = !autostart;
    if (manualStartupEnabled(cur_manualstart) &&
        cur_manualstart != want_manualstart) {
        if (!enableManualStartup(want_manualstart)) {
            // TODO: in case of failure, rebuild the prior state?
            return false;
        }
        dirty = true;
    }
    // AT+ROLE0
    role_t cur_role;
    if (getRole(cur_role) && cur_role != role_t::cPeripheral) {
        if (!setRole(role_t::cPeripheral)) {
            return false;
        }
        dirty = true;
    }
    if (dirty) {
        reset();
    }
}

Modem::response_t Modem::easyReconnect() {
    response_t resp = response_t::cOther;
    if (!makeCentral()) {
        return resp;
    }
    reconnect(resp, true);
    return resp;
}

// AT -> OK/OK+LOST
bool Modem::disconnect() {
    String cmd("AT");
    String resp;
    return issueCommand(cmd, resp);
}

// AT+ADDR? -> OK+ADDR:MAC Address
bool Modem::getAddress(String &addr) {
    String cmd("ADDR");
    return getString(cmd, addr);
}

// AT+ADVI? -> OK+Get:[0-F]
bool Modem::getAdvertisingInterval(adv_interval_t &interval) {
    String cmd("ADVI");
    return getChar(cmd, (uint8_t &)interval);
}

// AT+ADVI[0-F] -> OK+Set:[0-F]
bool Modem::setAdvertisingInterval(adv_interval_t interval) {
    String cmd("ADVI");
    return setChar(cmd, (uint8_t)interval);
}

// AT+ADTY? -> OK+Get:[0-3]
bool Modem::getAdvertisingType(advertising_t &type) {
    String cmd("ADTY");
    return getChar(cmd, (uint8_t &)type);
}

// AT+ADTY[0-3] -> OK+Set:[0-3]
bool Modem::setAdvertisingType(advertising_t type) {
    String cmd("ADTY");
    return setChar(cmd, (uint8_t)type);
}

// AT+AFTC? -> OK+Get:[0-3] void
bool Modem::getConnectedModulePinOutputState(output_state_t &state) {
    String cmd("AFTC");
    return getChar(cmd, (uint8_t &)state);
}

// AT+AFTC[0-3] -> OK+Set:[0-3]
bool Modem::setConnectedModulePinOutputState(output_state_t state) {
    String cmd("AFTC");
    return setChar(cmd, (uint8_t)state);
}

// AT+BEFC?  -> OK+Get:[0-3]
bool Modem::getInitialModulePinOutputState(output_state_t &state) {
    String cmd("BEFC");
    return getChar(cmd, (uint8_t &)state);
}

// AT+BEFC[0-3] -> OK+Set:[0-3]
bool Modem::setInitialModulePinOutputState(output_state_t state) {
    String cmd("BEFC");
    return setChar(cmd, (uint8_t)state);
}

// AT+BAUD? -> OK+Get:[0-8]
bool Modem::getBaudRate(rate_t &rate) {
    String cmd("BAUD");
    return getChar(cmd, (uint8_t &)rate);
}

// AT+BAUD[0-8] -> OK+Set:[0-8]
bool Modem::setBaudRate(rate_t rate) {
    String cmd("BAUD");
    return setChar(cmd, (uint8_t)rate);
}

// AT+CHAR? -> OK+Get:'0x'[0-F]{4}
bool Modem::getCharacteristic(String &charc) {
    String cmd("CHAR");
    return getString(cmd, charc);
}

// AT+CHAR'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}
bool Modem::setCharacteristic(const String &charc) {
    String cmd("CHAR");
    return setString(cmd, charc);
}

// AT+COMI? -> OK+Get:[0-9]
bool Modem::getMinimumLinkLayerConnectionInterval(
    ll_connect_interval_t &interval) {
    String cmd("COMI");
    return getChar(cmd, (uint8_t &)interval);
}

// AT+COMI[0-9] -> OK+Set:[0-9]
bool Modem::setMinimumLinkLayerConnectionInterval(
    ll_connect_interval_t interval) {
    String cmd("COMI");
    return setChar(cmd, (uint8_t)interval);
}

// AT+COMA? -> OK+Get:[0-9]
bool Modem::getMaximumLinkLayerConnectionInterval(
    ll_connect_interval_t &interval) {
    String cmd("COMA");
    return getChar(cmd, (uint8_t &)interval);
}

// AT+COMA[0-9] -> OK+Set:[0-9]
bool Modem::setMaximumLinkLayerConnectionInterval(
    ll_connect_interval_t interval) {
    String cmd("COMA");
    return setChar(cmd, (uint8_t)interval);
}

// AT+COLA? -> OK+Get:[0-4]
bool Modem::getLinkLayerConnectionSlaveLatency(uint8_t &latency) {
    String cmd("COLA");
    return getChar(cmd, latency);
}

// AT+COLA[0-4] -> OK+Set:[0-4]
bool Modem::setLinkLayerConnectionSlaveLatency(uint8_t latency) {
    String cmd("COLA");
    return setChar(cmd, latency);
}

// AT+COSU? -> OK+Get:[0-6]
/* Effect of command unclear, description is duplicate of another command
bool Modem::getUnknownInterval(uint8_t &interval) {
    String cmd("COSU");
    return getChar(cmd, interval);
}
*/

// AT+COSU[0-6] -> OK+Set:[0-6]
/* Effect of command unclear, description is duplicate of another command
bool Modem::setUnknownInterval(uint8_t interval) {
    String cmd("COSU");
    return setChar(cmd, interval);
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
    return issueSet(cmd, val, resp);
}

// AT+CONNL -> OK+CONN[LEFN]
bool Modem::reconnect(response_t &response, bool waitForConnection) {
    String cmd("AT+CONNL");
    stream.print(cmd);
    String resp;
    // TODO: timeout, in case OK+CONN isn't coming, like if we're already
    // connected
    while (resp.indexOf("OK+CONN") < 0) {
        if (stream.available()) {
            resp += stream.read();
        }
    }
    while (!stream.available()) {
    }
    char code = stream.peek();
    if (code != 'L' && code != 'E' && code != 'F' && code != 'N') {
        return false;
    }
    stream.read();

    switch (code) {
    case 'L':
        response = response_t::cReconnecting;
        if (waitForConnection) {
            waitConnected();
        }
        break;
    case 'E':
        response = response_t::cError;
        break;
    case 'F':
        response = response_t::cFail;
        break;
    case 'N':
        response = response_t::cNoAddress;
        break;
    default:
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
    stream.print(cmd);
    String resp;
    // TODO: timeout, in case OK+CONN isn't coming, like if we're already
    // connected
    while (resp.indexOf("OK+CONN") < 0) {
        if (stream.available()) {
            resp += stream.read();
        }
    }
    while (!stream.available()) {
    }
    char code = stream.peek();
    if (code != 'A' && code != 'E' && code != 'F') {
        return false;
    }
    stream.read();

    switch (code) {
    case 'A':
        response = response_t::cConnecting;
        if (waitForConnection) {
            waitConnected();
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
    return true;
}

// AT+CO[N1][0-2][0-9]{6} -> OK+CO[N1][N1][AEF]
bool Modem::connectAddress(const String &addr, response_t &response,
                           bool waitForConnection) {
    String cmd("AT+CON");
    cmd.concat(addr);
    stream.print(cmd);
    String resp;
    // TODO: timeout, in case OK+CONN isn't coming, like if we're already
    // connected
    while (resp.indexOf("OK+CONN") < 0) {
        if (stream.available()) {
            resp += stream.read();
        }
    }
    while (!stream.available()) {
    }
    char code = stream.peek();
    if (code != 'A' && code != 'E' && code != 'F') {
        return false;
    }
    stream.read();

    switch (code) {
    case 'A':
        response = response_t::cConnecting;
        if (waitForConnection) {
            waitConnected();
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
    return true;
}
// TODO: connectRandom() AT+CO1??? -> OK+CO11[AEF]

bool Modem::waitConnected() {
    // TODO: add a timeout
    String buffer;
    while (buffer.indexOf("OK+CONN", 1) < 0) {
        buffer.concat(stream.readString());
    }
    Serial.println(buffer);
    return true;
}

// AT+DISC? -> OK+DISCS/OK+DIS[0-2]:[0-F]{6}:-[0-9]{3}:(\w+)?/OK+DISCE
bool Modem::discoverDevices() {
    // TODO: DIS0/DIS1/DIS2 not well documented - what causes them,
    // what do they mean?
    deviceCount = 0;

    stream.print("AT+DISC?");
    String resp = stream.readString();

    // TODO: Only error out here if we exceed a timeout without receiving the
    // OK+DISCS response
    if (resp.indexOf("OK+DISCS") < 0) {
        return false;
    }

    // We got the "DISCS" response... we can't get off this train until we
    // receive DISCE (TODO: or perhaps if we've exceeded the scan timeout)
    String discbuf = resp.substring(resp.indexOf("OK+DISCS"));
    while (!discbuf.endsWith("OK+DISCE")) {
        discbuf.concat(stream.readString());
    }

    Serial.println(discbuf);

    for (int i = 0;
         i < BLE_MAX_DISCOVERED_DEVICES && !discbuf.startsWith("OK+DISCE");) {
        unsigned int end = discbuf.indexOf("OK+", 1);
        String entry = discbuf.substring(0, end);
        discbuf = discbuf.substring(end);
        if (entry.equals("OK+DISCS")) {
            continue;
        } else if (entry.startsWith("OK+DIS0:")) {
            long start;
            long end;
            devices[i].id = i;

            start = strlen("DIS0");
            end = entry.indexOf(':', start);
            devices[i].mac = entry.substring(start, end);

            start = end + 1;
            end = entry.indexOf(':', start);
            // devices[i].power = entry.substring(start, end);

            start = end + 1;
            end = entry.indexOf(':', start);
            devices[i].name = entry.substring(start, end);

            deviceCount = ++i;
        } else {
            Serial.print("Unhandled discovery entry: ");
            Serial.println(entry);
        }
    }

    return true;
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

// AT+DISI? -> OK+DISC????
bool Modem::discoverBeacons() {
    beaconCount = 0;

    stream.print("AT+DISI?");
    String resp = stream.readString();

    // TODO: Only error out here if we exceed a timeout without receiving the
    // OK+DISCS response
    if (resp.indexOf("OK+DISCS") < 0) {
        return false;
    }

    // We got the "DISCS" response... we can't get off this train until we
    // receive DISCE (TODO: or perhaps if we've exceeded the scan timeout)
    String discbuf = resp.substring(resp.indexOf("OK+DISCS"));
    while (!discbuf.endsWith("OK+DISCE")) {
        discbuf.concat(stream.readString());
    }

    Serial.println(discbuf);

    for (int i = 0;
         i < BLE_MAX_DISCOVERED_DEVICES && !discbuf.startsWith("OK+DISCE");) {
        unsigned int end = discbuf.indexOf("OK+", 1);
        String entry = discbuf.substring(0, end);
        discbuf = discbuf.substring(end);
        if (entry.equals("OK+DISCS")) {
            continue;
        } else if (entry.startsWith("OK+DIS0:")) {
            long start = strlen("DIS");
            long end = entry.indexOf(':', start);
            beacons[i].id = i;
            beacons[i].factory_id = entry.substring(start, end);
            start = end + 1;
            end = entry.indexOf(':', start);
            beacons[i].uuid = entry.substring(start, end);
            start = end + 1;
            end = entry.indexOf(':', start);
            beacons[i].majorVersion = entry.substring(start, end);
            start = end + 1;
            end = entry.indexOf(':', start);
            beacons[i].minorVersion = entry.substring(start, end);
            start = end + 1;
            end = entry.indexOf(':', start);
            beacons[i].minorVersion = entry.substring(start, end);
            start = end + 1;
            end = entry.indexOf(':', start);
            beacons[i].power = entry.substring(start, end);
            start = end + 1;
            end = entry.indexOf(':', start);
            beacons[i].mac = entry.substring(start, end);
            start = end + 1;
            end = entry.indexOf(':', start);
            beacons[i].rssi = entry.substring(start, end);

            beaconCount = ++i;
        } else {
            Serial.print("Unhandled discovery entry: ");
            Serial.println(entry);
        }
    }

    return true;
}

uint8_t Modem::beaconsCount() { return beaconCount; }

bool Modem::getBeacon(uint8_t id, beacon_t &beacon) {
    if (id >= beaconCount) {
        return false;
    } else {
        beacon = beacons[id];
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

// AT+GAIT? -> OK+Get:[0-1]
bool Modem::highTxGainEnabled(bool &enabled) {
    String cmd("GAIT");
    return getBool(cmd, enabled);
}

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
    stream.print("AT+HELP?");
    return stream.readString();
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
// AT+IBEA? -> OK+Get:[0-1]
bool Modem::beaconEnabled(bool &enabled) {
    String cmd("IBEA");
    return getBool(cmd, enabled);
}

// AT+IBEA[0-1] -> OK+Set:[0-1]
bool Modem::enableBeacon(bool enable) {
    String cmd("IBEA");
    return setBool(cmd, enable);
}

bool Modem::getBeaconUuid(String &uuid) {
    String builder;
    String tmp;
    for (int i = 0; i < 4; i++) {
        if (!getBeaconUuidPart(i, tmp)) {
            return false;
        }
        builder.concat(tmp);
    }
    uuid = builder;
    return true;
}

// AT+IBE0? -> OK+Get:[0-F]{4}, AT+IBE1, AT+IBE2, AT+IBE3
bool Modem::getBeaconUuidPart(uint8_t quad, String &uuid) {
    String cmd("IBE");
    String part(quad, HEX);
    cmd.concat(part);
    return getString(cmd, uuid);
}

bool Modem::setBeaconUuid(const String &uuid) {
    for (int i = 0; i < 4; i++) {
        String part = uuid.substring(i * 8, (i + 1) * 8);
        if (!setBeaconUuidPart(i, part)) {
            return false;
        }
    }
    return true;
}

// AT+IBE0[0-F]{4} -> OK+Set:[0-F]{4}, AT+IBE1, AT+IBE2, AT+IBE3
bool Modem::setBeaconUuidPart(uint8_t quad, const String &uuid) {
    String cmd("IBE");
    String part(quad, HEX);
    cmd.concat(part);
    return setString(cmd, uuid);
}

// AT+MARJ? -> OK+Get:[0-F]{4}
bool Modem::getBeaconMajorVersion(String &version) {
    String cmd("MARJ");
    return getString(cmd, version);
}

// AT+MARJ[0-F]{4} -> OK+Set:[0-F]{4}
bool Modem::setBeaconMajorVersion(const String &version) {
    String cmd("MARJ");
    return setString(cmd, version);
}

// AT+MINO? -> OK+Get:[0-F]{4}
bool Modem::getBeaconMinorVersion(String &version) {
    String cmd("MINO");
    return getString(cmd, version);
}

// AT+MINO[0-F]{4} -> OK+Set:[0-F]{4}
bool Modem::setBeaconMinorVersion(const String &version) {
    String cmd("MINO");
    return setString(cmd, version);
}

// AT+MEAS? -> OK+Get:[0-F]{2}
bool Modem::getBeaconMeasuredPower(String &power) {
    String cmd("MEAS");
    return getString(cmd, power);
}

// AT+MEAS[0-F]{2} -> OK+Set:[0-F]{2}
bool Modem::setBeaconMeasuredPower(const String &power) {
    String cmd("MEAS");
    return setString(cmd, power);
}

// AT+MODE? -> OK+Get:[0-2]
bool Modem::getUartMode(mode_t &mode) {
    String cmd("MODE");
    return getChar(cmd, (uint8_t &)mode);
}

// AT+MODE[0-2] -> OK+Set:[0-2]
bool Modem::setUartMode(mode_t mode) {
    String cmd("MODE");
    return setChar(cmd, (uint8_t)mode);
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
    return getString(cmd, name);
}

// AT+NAME\w{1,13} -> OK+Set\w{1,13}
bool Modem::setModuleName(const String &name) {
    String cmd("NAME");
    return setString(cmd, name);
}

// AT+PARI? -> OK+Get:[0-2]
bool Modem::getParity(parity_t &parity) {
    String cmd("PARI");
    return getChar(cmd, (uint8_t &)parity);
}

// AT+PARI[0-2] -> OK+Set:[0-2]
bool Modem::setParity(parity_t parity) {
    String cmd("PARI");
    return setChar(cmd, (uint8_t)parity);
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
    return getChar(cmd, (uint8_t &)power);
}

// AT+POWE[0-7] -> OK+Set:[0-7]
bool Modem::setModulePower(power_t power) {
    String cmd("POWE");
    return setChar(cmd, (uint8_t)power);
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
    return issueCommand(cmd, resp);
}

// AT+RESET -> OK+RESET
bool Modem::reset() {
    String cmd("RESET");
    String resp;
    return issueCommand(cmd, resp);
}

// AT+ROLE? -> OK+Get:[0-1]
bool Modem::getRole(role_t &role) {
    String cmd("ROLE");
    return getChar(cmd, (uint8_t &)role);
}

// AT+ROLE[0-1] -> OK+Set:[0-1]
bool Modem::setRole(role_t role) {
    String cmd("ROLE");
    return setChar(cmd, (uint8_t)role);
}

// AT+RADD? -> OK+RADD:[0-F]{6}
bool Modem::getLastConnected(String &address) {
    String cmd("RADD");
    return getString(cmd, address);
}

// AT+RESP? -> OK+Get:[0-2]
bool Modem::getTalkMethod(talk_t &method) {
    String cmd("RESP");
    return getChar(cmd, (uint8_t &)method);
}

// AT+RESP[0-2] -> OK+Set:[0-2]
bool Modem::setTalkMethod(talk_t method) {
    String cmd("RESP");
    return setChar(cmd, (uint8_t)method);
}

// AT+SYSK? -> OK+Get:[0-1]
bool Modem::getSystemKeySetting(system_key_t &key_setting) {
    String cmd("SYSK");
    return getChar(cmd, (uint8_t &)key_setting);
}

// AT+SYSK[0-1] -> OK+Set:[0-1]
bool Modem::setSystemKeySetting(system_key_t key_setting) {
    String cmd("SYSK");
    return setChar(cmd, (uint8_t)key_setting);
}

// AT+STOP? -> OK+Get:[0-1]
bool Modem::getStopBit(stop_t &stop_bit) {
    String cmd("STOP");
    return getChar(cmd, (uint8_t &)stop_bit);
}

// AT+STOP[0-1] -> OK+Set:[0-1]
bool Modem::setStopBit(stop_t stop_bit) {
    String cmd("STOP");
    return setChar(cmd, (uint8_t)stop_bit);
}

// AT+SLEEP -> OK+SLEEP
bool Modem::sleep() {
    String cmd("SLEEP");
    String resp;
    return issueCommand(cmd, resp);
}

// AT+START -> OK+START
bool Modem::start() {
    String cmd("START");
    String resp;
    return issueCommand(cmd, resp);
}

// AT+SCAN? -> OK+Get[1-5]
bool Modem::getScanDuration(uint8_t &seconds) {
    String cmd("SCAN");
    return getChar(cmd, seconds);
}

// AT+SCAN[1-5] -> OK+Set[1-5]
bool Modem::setScanDuration(uint8_t seconds) {
    String cmd("SCAN");
    return setChar(cmd, seconds);
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
bool Modem::enableNameDiscovery(bool enable) {
    String cmd("SHOW");
    return setBool(cmd, enable);
}

// AT+UUID? -> OK+Get:'0x'[0-F]{4}
bool Modem::getServiceUuid(String &uuid) {
    String cmd("UUID");
    return getString(cmd, uuid);
}

// AT+UUID'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}
bool Modem::setServiceUuid(const String &uuid) {
    String cmd("UUID");
    return setString(cmd, uuid);
}

// AT+VERR?/AT+VERS? -> OK+Get:String
bool Modem::getFirmwareVersion(String &fwver) {
    String cmd("VERR");
    return getString(cmd, fwver);
}

// Helper methods
bool Modem::readResponse(String &response) {
    String tmp(stream.readString());
    if (tmp.startsWith("OK+")) {
        response = tmp.substring(strlen("OK+"));
        return true;
    } else if (response.startsWith("OK")) {
        response = tmp.substring(strlen("OK"));
        return true;
    }
    response = tmp;
    return false;
}

bool Modem::issueCommand(const String &args, String &response) {
    String command("AT");
    if (args.length()) {
        command.concat("+");
        command.concat(args);
    }
    stream.print(command);
    return readResponse(response);
}

bool Modem::issueQuery(const String &cmd, const String &respprefix,
                       String &response) {
    String query(cmd + "?");
    String respprefix2(respprefix + ":");
    if (!issueCommand(query, response)) {
        return false;
    }
    if (response.startsWith(respprefix2)) {
        response = response.substring(respprefix2.length());
        return true;
    }
    Serial.println(response);
    return false;
}

bool Modem::issueGet(const String &cmd, String &response) {
    String prefix("Get");
    return issueQuery(cmd, prefix, response);
}

bool Modem::issueSet(const String &cmd, const String &args, String &response) {
    String set(cmd + args);
    if (!issueCommand(set, response)) {
        return false;
    }
    if (response.startsWith("Set:")) {
        response = response.substring(strlen("Set:"));
        return args.equals(response);
    }
    return false;
}

bool Modem::getBool(String &cmd, bool &val) {
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    val = (bool)(resp.toInt());
    return true;
}

bool Modem::setBool(const String &cmd, bool val) {
    String intval((uint8_t)val, HEX);
    String resp;
    bool foo = issueSet(cmd, intval, resp);
    return foo;
}

bool Modem::getChar(String &cmd, uint8_t &val) {
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    val = (uint8_t)(resp.toInt());
    return true;
}

bool Modem::setChar(const String &cmd, uint8_t val) {
    String intval((uint8_t)val, HEX);
    String resp;
    return issueSet(cmd, intval, resp);
}

bool Modem::getString(const String &cmd, String &val) {
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    val = resp;
    return true;
}

bool Modem::setString(const String &cmd, const String &val) {
    String resp;
    return issueSet(cmd, val, resp);
}
