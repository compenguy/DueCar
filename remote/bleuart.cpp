
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
    String query(cmd);
    query.concat("?");
    String respprefix2(respprefix);
    respprefix2.concat(":");
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
    String set(cmd);
    set.concat(args);
    if (!issueCommand(set, response)) {
        return false;
    }
    if (response.startsWith("Set:")) {
        response = response.substring(strlen("Set:"));
        return args.equals(response);
    }
    return false;
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
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    addr = resp;
    return true;
}

// AT+ADVI? -> OK+Get:[0-F]
bool Modem::getAdvertisingInterval(adv_interval_t &interval) {
    String cmd("ADVI");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    interval = (adv_interval_t)(resp.toInt());
    return true;
}

// AT+ADVI[0-F] -> OK+Set:[0-F]
bool Modem::setAdvertisingInterval(adv_interval_t interval) {
    String cmd("ADVI");
    String val((uint8_t)interval, HEX);
    String response;
    return issueSet(cmd, val, response);
}

// AT+ADTY? -> OK+Get:[0-3]
bool Modem::getAdvertisingType(advertising_t &type) {
    String cmd("ADTY");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    type = (advertising_t)(resp.toInt());
    return true;
}

// AT+ADTY[0-3] -> OK+Set:[0-3]
bool Modem::setAdvertisingType(advertising_t type) {
    String cmd("ADTY");
    String val((uint8_t)type, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+AFTC? -> OK+Get:[0-3] void
bool Modem::getConnectedModulePinOutputState(output_state_t &state) {
    String cmd("AFTC");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    state = (output_state_t)(resp.toInt());
    return true;
}

// AT+AFTC[0-3] -> OK+Set:[0-3]
bool Modem::setConnectedModulePinOutputState(output_state_t state) {
    String cmd("AFTC");
    String val((uint8_t)state, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+BEFC?  -> OK+Get:[0-3]
bool Modem::getInitialModulePinOutputState(output_state_t &state) {
    String cmd("BEFC");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    state = (output_state_t)(resp.toInt());
    return true;
}

// AT+BEFC[0-3] -> OK+Set:[0-3]
bool Modem::setInitialModulePinOutputState(output_state_t state) {
    String cmd("BEFC");
    String val((uint8_t)state, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+BAUD? -> OK+Get:[0-8]
bool Modem::getBaudRate(rate_t &rate) {
    String cmd("BAUD");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    rate = (rate_t)(resp.toInt());
    return true;
}

// AT+BAUD[0-8] -> OK+Set:[0-8]
bool Modem::setBaudRate(rate_t rate) {
    String cmd("BAUD");
    String val((uint8_t)rate, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+CHAR? -> OK+Get:'0x'[0-F]{4}
bool Modem::getCharacteristic(String &charc) {
    String cmd("CHAR");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    charc = resp;
    return true;
}

// AT+CHAR'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}
bool Modem::setCharacteristic(const String &charc) {
    String cmd("CHAR");
    String resp;
    return issueSet(cmd, charc, resp);
}

// AT+COMI? -> OK+Get:[0-9]
bool Modem::getMinimumLinkLayerConnectionInterval(
    ll_connect_interval_t &interval) {
    String cmd("COMI");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    interval = (ll_connect_interval_t)(resp.toInt());
    return true;
}

// AT+COMI[0-9] -> OK+Set:[0-9]
bool Modem::setMinimumLinkLayerConnectionInterval(
    ll_connect_interval_t interval) {
    String cmd("COMI");
    String val((uint8_t)interval, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+COMA? -> OK+Get:[0-9]
bool Modem::getMaximumLinkLayerConnectionInterval(
    ll_connect_interval_t &interval) {
    String cmd("COMA");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    interval = (ll_connect_interval_t)(resp.toInt());
    return true;
}

// AT+COMA[0-9] -> OK+Set:[0-9]
bool Modem::setMaximumLinkLayerConnectionInterval(
    ll_connect_interval_t interval) {
    String cmd("COMA");
    String val((uint8_t)interval, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+COLA? -> OK+Get:[0-4]
bool Modem::getLinkLayerConnectionSlaveLatency(uint8_t &latency) {
    String cmd("COLA");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    latency = (uint8_t)(resp.toInt());
    return true;
}

// AT+COLA[0-4] -> OK+Set:[0-4]
bool Modem::setLinkLayerConnectionSlaveLatency(uint8_t lat) {
    String cmd("COLA");
    String val((uint8_t)lat, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+COSU? -> OK+Get:[0-6]
bool Modem::getUnknownInterval(uint8_t &interval) {
    String cmd("COSU");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    interval = (uint8_t)(resp.toInt());
    return true;
}

// AT+COSU[0-6] -> OK+Set:[0-6]
bool Modem::setUnknownInterval(uint8_t interval) {
    String cmd("COSU");
    String val((uint8_t)interval, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+COUP? -> OK+Get:[0-1]
bool Modem::getUpdateConnection(bool &update) {
    String cmd("COUP");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    update = (bool)(resp.toInt());
    return true;
}

// AT+COUP[0-1] -> OK+Set:[0-1]
bool Modem::setUpdateConnection(bool update) {
    String cmd("COUP");
    String val((uint8_t)update, HEX);
    String resp;
    return issueSet(cmd, val, resp);
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
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    enabled = (bool)(resp.toInt());
    return true;
}

// AT+FFE2[0-1] -> OK+Set:[0-1]
bool Modem::enableCharacteristic2(bool enable) {
    String cmd("FFE2");
    String val((uint8_t)enable, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+FLOW? not supported yet as of V1 datasheet
// AT+FLOW[0-1] not supported yet as of V1 datasheet

// AT+GAIT? -> OK+Get:[0-1]
bool Modem::highTxGainEnabled(bool &enabled) {
    String cmd("GAIT");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    enabled = (bool)(resp.toInt());
    return true;
}

// AT+GAIT[0-1] -> OK+Set:[0-1]
bool Modem::enableHighTxGain(bool enable) {
    String cmd("GAIT");
    String val((uint8_t)enable, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+GAIN? -> OK+Get:[0-1]
bool Modem::highRxGainEnabled(bool &enabled) {
    String cmd("GAIN");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    enabled = (bool)(resp.toInt());
    return true;
}

// AT+GAIN[0-1] -> OK+Set:[0-1]
bool Modem::enableHighRxGain(bool enable) {
    String cmd("GAIN");
    String val((uint8_t)enable, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

// AT+HELP? -> String
String Modem::help() {
    stream.print("AT+HELP?");
    return stream.readString();
}

// AT+IMME? -> OK+Get:[0-1]
bool Modem::manualStartupEnabled(bool &enabled) {
    String cmd("IMME");
    String resp;
    if (!issueGet(cmd, resp)) {
        return false;
    }
    enabled = (bool)(resp.toInt());
    return true;
}

// AT+IMME[0-1] -> OK+Set:[0-1]
bool Modem::enableManualStartup(bool enable) {
    String cmd("IMME");
    String val((uint8_t)enable, HEX);
    String resp;
    return issueSet(cmd, val, resp);
}

/*
bool Modem::beaconEnabled(); // AT+IBEA? -> OK+Get:[0-1]
void Modem::enableBeacon(bool); // AT+IBEA[0-1] -> OK+Set:[0-1]

string Modem::getBeaconUuid(); // AT+IBE0? -> OK+Get:[0-F]{4}, AT+IBE1,
AT+IBE2, AT+IBE3 void Modem::setBeaconUuid(string); // AT+IBE0[0-F]{4} ->
OK+Set:[0-F]{4}, AT+IBE1, AT+IBE2, AT+IBE3

uint16_t Modem::getBeaconMajorVersion(); // AT+MARJ? -> OK+Get:[0-F]{4}
void Modem::setBeaconMajorVersion(uint16_t); // AT+MARJ[0-F]{4} ->
OK+Set:[0-F]{4}

uint16_t Modem::getBeaconMinorVersion(); // AT+MINO? -> OK+Get:[0-F]{4}
void Modem::setBeaconMinorVersion(uint16_t); // AT+MINO[0-F]{4} ->
OK+Set:[0-F]{4}

uint8_t Modem::getBeaconMeasuredPower(); // AT+MEAS? -> OK+Get:[0-F]{2}
void Modem::setBeaconMeasuredPower(uint8_t); // AT+MEAS[0-F]{2} ->
OK+Set:[0-F]{2}

mode_t Modem::getUartMode(); // AT+MODE? -> OK+Get:[0-2]
void Modem::setUartMode(mode_t); // AT+MODE[0-2] -> OK+Set:[0-2]

bool Modem::notificationsEnabled(); // AT+NOTI? -> OK+Get:[0-1]
void Modem::enableNotifications(bool); // AT+NOTI[0-1] -> OK+Set:[0-1]
bool Modem::getNotifyAddressOnDisconnect(); // AT+NOTP? -> OK+Get:[0-1]
void Modem::setNotifyAddressOnDisconnect(bool); // AT+NOTP[0-1] ->
OK+Set:[0-1]

string Modem::getModuleName(); // AT+NAME? -> OK+NAME\w{1,13}
void Modem::setModuleName(string); // AT+NAME\w{1,13} -> OK+Set\w{1,13}

parity_t Modem::getParity(); // AT+PARI? -> OK+Get:[0-2]
void Modem::setParity(parity_t); // AT+PARI[0-2] -> OK+Set:[0-2]

bool Modem::getPio(uint8_t); // AT+PIO[1-B]? -> OK+PIO[1-B]:[0-1]
void Modem::setPio(uint8_t, bool); // AT+PIO[1-B][0-1] -> OK+PIO[1-B]:[0-1]

bool Modem::autoSleepEnabled(); // AT+PWRM? -> OK+Get:[0-1]
void Modem::enableAutoSleep(bool); // AT+PWRM[0-1] -> OK+Set:[0-1]

power_t Modem::getModulePower(); // AT+POWE? -> OK+Get:[0-7]
void Modem::setModulePower(power_t); // AT+POWE[0-7] -> OK+Set:[0-7]

bool Modem::reliableAdvertisingEnabled(); // AT+RELI? -> OK+Get:[0-1]
void Modem::enableReliableAdvertising(bool); // AT+RELI[0-1] -> OK+Set:[0-1]

void Modem::resetConfiguration(); // AT+RENEW -> OK+RENEW
void Modem::reset(); // AT+RESET -> OK+RESET

role_t Modem::getRole(); // AT+ROLE? -> OK+Get:[0-1]
void Modem::setRole(role_t); // AT+ROLE[0-1] -> OK+Set:[0-1]

string Modem::getLastConnected(); // AT+RADD? -> OK+RADD:[0-F]{6}

talk_t Modem::getTalkMethod(); // AT+RESP? -> OK+Get:[0-2]
void Modem::setTalkMethod(talk_t); // AT+RESP[0-2] -> OK+Set:[0-2]

system_key_t Modem::getSystemKeySetting(); // AT+SYSK? ->
OK+Get:[0-1] void Modem::setSystemKeySetting(system_key_t); //
AT+SYSK[0-1] -> OK+Set:[0-1]

stop_t Modem::getStopBit(); // AT+STOP? -> OK+Get:[0-1]
void Modem::setStopBit(stop_t); // AT+STOP[0-1] -> OK+Set:[0-1]

void Modem::sleep(); // AT+SLEEP -> OK+SLEEP

void Modem::start(); // AT+START -> OK+START

uint8_t Modem::getScanDuration(); // AT+SCAN? -> OK+Get[1-5]
void Modem::setScanDuration(uint8_t); // AT+SCAN[1-5] -> OK+Set[1-5]

bool Modem::saveLastDeviceEnabled(); // AT+SAVE? -> OK+Get:[0-1]
void Modem::enableSaveLastDevice(bool); // AT+SAVE[0-1] -> OK+Set:[0-1]

bool Modem::nameDiscoveryEnabled(); // AT+SHOW? -> OK+Get:[0-1]
void Modem::enableNameDiscovery(bool); // AT+SHOW[0-1] -> OK+Set:[0-1]

uint16_t Modem::getServiceUuid(); // AT+UUID? -> OK+Get:'0x'[0-F]{4}
void Modem::setServiceUuid(uint16_t); // AT+UUID'0x'[0-F]{4} ->
OK+Set:'0x'[0-F]{4}

string Modem::getFirmwareVersion(); // AT+VERR/AT+VERS -> String
*/
