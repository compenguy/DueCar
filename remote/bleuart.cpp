
#include "bleuart.h"

using Ble::Modem;

String Modem::issueCommand(String &args) {
    String command("AT");
    if (args.length()) {
        command.concat("+");
        command.concat(args);
    }
    stream.print(command);
    String response(stream.readString());
    if (args.length()) {
        if (response.startsWith("OK+")) {
            return response.substring(strlen("OK+"));
        }
    } else {
        if (response.startsWith("OK")) {
            return response.substring(strlen("OK"));
        }
    }
    return response;
}

String Modem::issueCommand(const char *args) {
    String strargs(args);
    return issueCommand(strargs);
}

// TOOD: It's possible that all queries return Get: as the response prefix
// (contrary to documentation), and if so, this method is redundant, and should
// be folded into issueGet
String Modem::issueQuery(String &cmd, String &respprefix) {
    String query(cmd);
    query.concat("?");
    String respprefix2(respprefix);
    respprefix2.concat(":");
    String response(issueCommand(query));
    if (response.startsWith(respprefix2)) {
        return response.substring(respprefix2.length());
    }
    return response;
}

String Modem::issueQuery(const char *cmd, const char *respprefix) {
    String strcmd(cmd);
    String strprefix(respprefix);
    return issueQuery(strcmd, strprefix);
}

String Modem::issueGet(String &cmd) {
    String prefix("Get");
    return issueQuery(cmd, prefix);
}

String Modem::issueGet(const char *cmd) {
    String strcmd(cmd);
    return issueGet(strcmd);
}

String Modem::issueSet(String &cmd, String &args) {
    String set(cmd);
    set.concat(args);
    String response(issueCommand(set));
    if (response.startsWith("Set:")) {
        return response.substring(strlen("Set:"));
    }
    return response;
}

String Modem::issueSet(const char *cmd, const char *args) {
    String strcmd(cmd);
    String strargs(args);
    return issueSet(strcmd, strargs);
}

bool Modem::issueSetChecked(String &cmd, String &args) {
    return issueSet(cmd, args).equals(args);
}

// AT -> OK/OK+LOST
String Modem::disconnect() { return issueCommand("AT"); }

// AT+ADDR? -> OK+ADDR:MAC Address
String Modem::getAddress() { return issueGet("ADDR"); }

// AT+ADVI? -> OK+Get:[0-F]
Ble::adv_interval_t Modem::getAdvertisingInterval() {
    String interval = issueGet("ADVI");
    long interval2 = interval.toInt();
    return (Ble::adv_interval_t)interval2;
}

// AT+ADVI[0-F] -> OK+Set:[0-F]
bool Modem::setAdvertisingInterval(adv_interval_t interval) {
    String cmd("ADVI");
    String val((uint8_t)interval, HEX);
    return issueSetChecked(cmd, val);
}

// AT+ADTY? -> OK+Get:[0-3]
Ble::advertising_t Modem::getAdvertisingType() {
    String type = issueGet("ADTY");
    long type2 = type.toInt();
    return (Ble::advertising_t)type2;
}

// AT+ADTY[0-3] -> OK+Set:[0-3]
bool Modem::setAdvertisingType(Ble::advertising_t type) {
    String cmd("ADTY");
    String val((uint8_t)type, HEX);
    return issueSetChecked(cmd, val);
}

// AT+AFTC? -> OK+Get:[0-3] void
Ble::ModulePin::output_state_t Modem::getConnectedModulePinOutputState() {
    String state = issueGet("AFTC");
    long state2 = state.toInt();
    return (ModulePin::output_state_t)state2;
}

// AT+AFTC[0-3] -> OK+Set:[0-3]
bool Modem::setConnectedModulePinOutputState(ModulePin::output_state_t state) {
    String cmd("AFTC");
    String val((uint8_t)state, HEX);
    return issueSetChecked(cmd, val);
}

// AT+BEFC?  -> OK+Get:[0-3]
Ble::ModulePin::output_state_t Modem::getInitialModulePinOutputState() {
    String state = issueGet("BEFC");
    long state2 = state.toInt();
    return (ModulePin::output_state_t)state2;
}

// AT+BEFC[0-3] -> OK+Set:[0-3]
bool Modem::setInitialModulePinOutputState(ModulePin::output_state_t state) {
    String cmd("BEFC");
    String val((uint8_t)state, HEX);
    return issueSetChecked(cmd, val);
}

// AT+BAUD? -> OK+Get:[0-8]
Ble::Uart::rate_t Modem::getBaudRate() {
    String rate = issueGet("BAUD");
    long rate2 = rate.toInt();
    return (Uart::rate_t)rate2;
}

// AT+BAUD[0-8] -> OK+Set:[0-8]
bool Modem::setBaudRate(Ble::Uart::rate_t rate) {
    String cmd("BAUD");
    String val((uint8_t)rate, HEX);
    return issueSetChecked(cmd, val);
}

// AT+CHAR? -> OK+Get:'0x'[0-F]{4}
String Modem::getCharacteristic() { return issueGet("CHAR"); }

// AT+CHAR'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}
bool Modem::setCharacteristic(String &charc) {
    String cmd("CHAR");
    return issueSetChecked(cmd, charc);
}

// AT+COMI? -> OK+Get:[0-9]
Ble::ll_connect_interval_t Modem::getMinimumLinkLayerConnectionInterval() {
    String intvl = issueGet("COMI");
    long intvl2 = intvl.toInt();
    return (Ble::ll_connect_interval_t)intvl2;
}

// AT+COMI[0-9] -> OK+Set:[0-9]
bool Modem::setMinimumLinkLayerConnectionInterval(
    Ble::ll_connect_interval_t interval) {
    String cmd("COMI");
    String val((uint8_t)interval, HEX);
    return issueSetChecked(cmd, val);
}

// AT+COMA? -> OK+Get:[0-9]
Ble::ll_connect_interval_t Modem::getMaximumLinkLayerConnectionInterval() {
    String intvl = issueGet("COMA");
    long intvl2 = intvl.toInt();
    return (Ble::ll_connect_interval_t)intvl2;
}

// AT+COMA[0-9] -> OK+Set:[0-9]
bool Modem::setMaximumLinkLayerConnectionInterval(
    Ble::ll_connect_interval_t interval) {
    String cmd("COMA");
    String val((uint8_t)interval, HEX);
    return issueSetChecked(cmd, val);
}

// AT+COLA? -> OK+Get:[0-4]
uint8_t Modem::getLinkLayerConnectionSlaveLatency() {
    String intvl = issueGet("COLA");
    long intvl2 = intvl.toInt();
    return (uint8_t)intvl2;
}

// AT+COLA[0-4] -> OK+Set:[0-4]
bool Modem::setLinkLayerConnectionSlaveLatency(uint8_t lat) {
    String cmd("COLA");
    String val((uint8_t)lat, HEX);
    return issueSetChecked(cmd, val);
}

// AT+COSU? -> OK+Get:[0-6]
uint8_t Modem::getUnknownInterval() {
    String intvl = issueGet("COSU");
    long intvl2 = intvl.toInt();
    return (uint8_t)intvl2;
}

// AT+COSU[0-6] -> OK+Set:[0-6]
bool Modem::setUnknownInterval(uint8_t interval) {
    String cmd("COSU");
    String val((uint8_t)interval, HEX);
    return issueSetChecked(cmd, val);
}

// AT+COUP? -> OK+Get:[0-1]
bool Modem::getUpdateConnection() {
    String update = issueGet("COUP");
    long update2 = update.toInt();
    return (bool)update2;
}

// AT+COUP[0-1] -> OK+Set:[0-1]
bool Modem::setUpdateConnection(bool update) {
    String cmd("COUP");
    String val((uint8_t)update, HEX);
    return issueSetChecked(cmd, val);
}

// AT+CLEAR -> OK+CLEAR
bool Modem::clearLastConnectedDeviceAddress() {
    String cmd("CLEAR");
    String val("");
    return issueSetChecked(cmd, val);
}

// AT+CONNL -> OK+CONN[LEFN]
Ble::Connection::response_t Modem::reconnect() {
    String result = issueSet("CONN", "L");
    switch (result[0]) {
    case 'L':
        return Connection::cReconnecting;
    case 'E':
        return Connection::cError;
    case 'F':
        return Connection::cFail;
    case 'N':
        return Connection::cNoAddress;
    default:
        return Connection::cOther;
    }
}

// AT+CONN[0-5] -> OK+CONN[AEF]
Ble::Connection::response_t Modem::connectId(uint8_t id) {
    String cmd("CONN");
    String val(id, HEX);
    String result = issueSet(cmd, val);
    switch (result[0]) {
    case 'A':
        return Connection::cConnecting;
    case 'E':
        return Connection::cError;
    case 'F':
        return Connection::cFail;
    default:
        return Connection::cOther;
    }
}

// AT+CO[N1][0-2][0-9]{6} -> OK+CO[N1][N1][AEF]
Ble::Connection::response_t Modem::connectAddress(String &addr) {
    String cmd("CON");
    String result = issueSet(cmd, addr);
    if (result[0] != 'N')
        return Connection::cOther;

    switch (result[1]) {
    case 'A':
        return Connection::cConnecting;
    case 'E':
        return Connection::cError;
    case 'F':
        return Connection::cFail;
    default:
        return Connection::cOther;
    }
}

/*
void Modem::discoverDevices(); // AT+DISC? -> OK+DIS[C0-2]([SE]|[0-F]{6})
uint8_t Modem::devicesCount();
device_t Modem::getDevice(uint8_t);

void Modem::discoverBeacons(); // AT+DISI? -> OK+DISC????
uint8_t Modem::beaconsCount();
beacon_t Modem::getBeacon(uint8_t);

bool Modem::characteristic2Enabled(); // AT+FFE2? -> OK+Get:[0-1]
void Modem::enableCharacteristic2(bool); // AT+FFE2[0-1] -> OK+Set:[0-1]

// AT+FLOW? not supported yet as of V1 datasheet
// AT+FLOW[0-1] not supported yet as of V1 datasheet

bool Modem::highTxGainEnabled(); // AT+GAIT? -> OK+Get:[0-1]
void Modem::enableHighTxGain(bool); // AT+GAIT[0-1] -> OK+Set:[0-1]

bool Modem::highRxGainEnabled(); // AT+GAIN? -> OK+Get:[0-1]
void Modem::enableHighRxGain(bool); // AT+GAIN[0-1] -> OK+Set:[0-1]

string Modem::help(); // AT+HELP? -> String

bool Modem::manualStartupEnabled(); // AT+IMME? -> OK+Get:[0-1]
void Modem::enableManualStartup(bool); // AT+IMME[0-1] -> OK+Set:[0-1]

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

Uart::mode_t Modem::getUartMode(); // AT+MODE? -> OK+Get:[0-2]
void Modem::setUartMode(Uart::mode_t); // AT+MODE[0-2] -> OK+Set:[0-2]

bool Modem::notificationsEnabled(); // AT+NOTI? -> OK+Get:[0-1]
void Modem::enableNotifications(bool); // AT+NOTI[0-1] -> OK+Set:[0-1]
bool Modem::getNotifyAddressOnDisconnect(); // AT+NOTP? -> OK+Get:[0-1]
void Modem::setNotifyAddressOnDisconnect(bool); // AT+NOTP[0-1] ->
OK+Set:[0-1]

string Modem::getModuleName(); // AT+NAME? -> OK+NAME\w{1,13}
void Modem::setModuleName(string); // AT+NAME\w{1,13} -> OK+Set\w{1,13}

Uart::parity_t Modem::getParity(); // AT+PARI? -> OK+Get:[0-2]
void Modem::setParity(Uart::parity_t); // AT+PARI[0-2] -> OK+Set:[0-2]

bool Modem::getPio(uint8_t); // AT+PIO[1-B]? -> OK+PIO[1-B]:[0-1]
void Modem::setPio(uint8_t, bool); // AT+PIO[1-B][0-1] -> OK+PIO[1-B]:[0-1]

bool Modem::autoSleepEnabled(); // AT+PWRM? -> OK+Get:[0-1]
void Modem::enableAutoSleep(bool); // AT+PWRM[0-1] -> OK+Set:[0-1]

Ble::power_t Modem::getModulePower(); // AT+POWE? -> OK+Get:[0-7]
void Modem::setModulePower(Ble::power_t); // AT+POWE[0-7] -> OK+Set:[0-7]

bool Modem::reliableAdvertisingEnabled(); // AT+RELI? -> OK+Get:[0-1]
void Modem::enableReliableAdvertising(bool); // AT+RELI[0-1] -> OK+Set:[0-1]

void Modem::resetConfiguration(); // AT+RENEW -> OK+RENEW
void Modem::reset(); // AT+RESET -> OK+RESET

Ble::role_t Modem::getRole(); // AT+ROLE? -> OK+Get:[0-1]
void Modem::setRole(Ble::role_t); // AT+ROLE[0-1] -> OK+Set:[0-1]

string Modem::getLastConnected(); // AT+RADD? -> OK+RADD:[0-F]{6}

Ble::talk_t Modem::getTalkMethod(); // AT+RESP? -> OK+Get:[0-2]
void Modem::setTalkMethod(Ble::talk_t); // AT+RESP[0-2] -> OK+Set:[0-2]

ModulePin::system_key_t Modem::getSystemKeySetting(); // AT+SYSK? ->
OK+Get:[0-1] void Modem::setSystemKeySetting(ModulePin::system_key_t); //
AT+SYSK[0-1] -> OK+Set:[0-1]

Uart::stop_t Modem::getStopBit(); // AT+STOP? -> OK+Get:[0-1]
void Modem::setStopBit(Uart::stop_t); // AT+STOP[0-1] -> OK+Set:[0-1]

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
