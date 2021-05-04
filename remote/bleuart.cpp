
#include "bleuart.h"

bool Modem::readResponse(String &response) {
    String tmp(stream.readString());
    if (tmp.startsWith("OK+")) {
        Serial.print("Successful: ");
        Serial.println(tmp);
        response = tmp.substring(strlen("OK+"));
        return true;
    } else if (response.startsWith("OK")) {
        Serial.print("Successful: ");
        Serial.println(tmp);
        response = tmp.substring(strlen("OK"));
        return true;
    }
    Serial.print("Unsuccessful: ");
    Serial.println(tmp);
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

// TODO: It's possible that all queries return Get: as the response prefix
// (contrary to documentation), and if so, this method is redundant, and should
// be folded into issueGet
bool Modem::issueQuery(const String &cmd, const String &respprefix,
                       String &response) {
    String query(cmd + "?");
    String respprefix2(respprefix + ":");
    Serial.print("Issuing query ");
    Serial.println(query);
    if (!issueCommand(query, response)) {
        return false;
    }
    if (response.startsWith(respprefix2)) {
        Serial.println("Got expected response.");
        response = response.substring(respprefix2.length());
        return true;
    }
    Serial.print("Unexpected response ");
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
    return issueSet(cmd, intval, resp);
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
bool Modem::getUnknownInterval(uint8_t &interval) {
    String cmd("COSU");
    return getChar(cmd, interval);
}

// AT+COSU[0-6] -> OK+Set:[0-6]
bool Modem::setUnknownInterval(uint8_t interval) {
    String cmd("COSU");
    return setChar(cmd, interval);
}

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
bool Modem::reconnect(response_t &response) {
    String cmd("CONN");
    String val("L");
    String resp;
    if (!issueSet(cmd, val, resp)) {
        return false;
    }
    switch (resp[0]) {
    case 'L':
        response = response_t::cReconnecting;
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
bool Modem::connectId(uint8_t id, response_t &response) {
    String cmd("CONN");
    String val(id, HEX);
    String resp;
    if (!issueSet(cmd, val, resp)) {
        return false;
    }
    switch (resp[0]) {
    case 'A':
        response = response_t::cConnecting;
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
bool Modem::connectAddress(const String &addr, response_t &response) {
    String cmd("CON");
    String resp;
    if (!issueSet(cmd, addr, resp)) {
        return false;
    }

    if (resp.charAt(0) != 'N') {
        response = response_t::cOther;
        return true;
    }

    char p = resp.charAt(1);
    switch (p) {
    case 'A':
        response = response_t::cConnecting;
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

// AT+DISC? -> OK+DIS[C0-2]([SE]|[0-F]{6})
bool Modem::discoverDevices() {
    // TODO: DIS0/DIS1/DIS2 not well documented - what causes them,
    // what do they mean?
    String cmd("DISC");
    String respprefix("DIS");
    String resp;

    deviceCount = 0;

    if (!issueQuery(cmd, respprefix, resp)) {
        return false;
    }
    bool first = true;
    for (int i = 0; i < BLE_MAX_DISCOVERED_DEVICES;) {
        if (!readResponse(resp)) {
            return false;
        }
        if (resp.equals("DISCE")) {
            break;
        } else if (resp.startsWith("DIS")) {
            // We have to be careful about how we increment i, because
            // 1) we don't want to increment it the first time through, so that
            //    the first device goes in the zeroth slot
            // 2) we increment when we discover a new device, right before
            //    assigning that device so that NAME responses get
            //    correctly paired with the device specified before
            // 3) we don't quit immediately when the table is full - we
            //    might or might not get a NAME response after the last
            //    device in the table, so we only quit when we get
            //    a DISCE response, or when we've read the first DISC
            //    response that won't fit
            if (!first) {
                ++i;
            }
            if (i >= BLE_MAX_DISCOVERED_DEVICES) {
                break;
            }
            devices[i].id = i;
            devices[i].mac = resp.substring(strlen("DIS") + 2);
            devices[i].name = "";
            deviceCount = i;
            first = false;
        } else if (resp.startsWith("NAME:")) {
            devices[i].name = resp.substring(strlen("NAME:"));
        }
    }
    // If our table filled up, there are more responses
    // Drain them off.
    if (!resp.equals("DISCE")) {
        do {
        } while (readResponse(resp) && !resp.equals("") &&
                 !resp.equals("DISCE"));
    }
    return deviceCount > 0;
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
    String cmd("DISI");
    String respprefix("DIS");
    String resp;

    beaconCount = 0;

    if (!issueQuery(cmd, respprefix, resp)) {
        return false;
    }
    for (int i = 0; i < BLE_MAX_DISCOVERED_DEVICES; ++i) {
        if (!readResponse(resp)) {
            return false;
        }
        if (resp.equals("DISCE")) {
            break;
        } else if (resp.startsWith("DIS")) {
            long start = strlen("DIS");
            long end = resp.indexOf(':', start);
            beacons[i].id = i;
            beacons[i].factory_id = resp.substring(start, end);
            start = end + 1;
            end = resp.indexOf(':', start);
            beacons[i].uuid = resp.substring(start, end);
            start = end + 1;
            end = resp.indexOf(':', start);
            beacons[i].majorVersion = resp.substring(start, end);
            start = end + 1;
            end = resp.indexOf(':', start);
            beacons[i].minorVersion = resp.substring(start, end);
            start = end + 1;
            end = resp.indexOf(':', start);
            beacons[i].minorVersion = resp.substring(start, end);
            start = end + 1;
            end = resp.indexOf(':', start);
            beacons[i].power = resp.substring(start, end);
            start = end + 1;
            end = resp.indexOf(':', start);
            beacons[i].mac = resp.substring(start, end);
            start = end + 1;
            end = resp.indexOf(':', start);
            beacons[i].rssi = resp.substring(start, end);

            beaconCount = i + 1;
        }
    }
    // If our table filled up, there are more responses
    // Drain them off.
    if (!resp.equals("DISCE")) {
        do {
            Serial.println("Draining unread responses from discovery.");
        } while (readResponse(resp) && !resp.equals("") &&
                 !resp.equals("DISCE"));
    }
    return beaconCount > 0;
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

// AT+IBE0? -> OK+Get:[0-F]{4}, AT+IBE1, AT+IBE2, AT+IBE3
bool Modem::getBeaconUuid(String &uuid) {
    String cmd("IBE0");
    return getString(cmd, uuid);
}

// AT+IBE0[0-F]{4} -> OK+Set:[0-F]{4}, AT+IBE1, AT+IBE2, AT+IBE3
bool Modem::setBeaconUuid(const String &uuid) {
    String cmd("IBE0");
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
bool Modem::autoSleepEnabled(bool &enabled) {
    String cmd("PWRM");
    return getBool(cmd, enabled);
}

// AT+PWRM[0-1] -> OK+Set:[0-1]
bool Modem::enableAutoSleep(bool enable) {
    String cmd("PWRM");
    return setBool(cmd, enable);
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
