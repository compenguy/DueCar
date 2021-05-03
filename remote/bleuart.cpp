
#include "bleuart.h"

using Ble::Modem;

String Modem::issueCommand(String &args) {
    String command("AT");
    if (args.length()) {
        command.concat("+");
        command.concat(args);
    }
    Serial.print("Sending: ");
    Serial.println(command);
    stream.print(command);
    String response(stream.readString());
    Serial.print("Received: ");
    Serial.println(response);
    return response;
}

String Modem::issueCommand(const char *args) {
    String strargs(args);
    return issueCommand(strargs);
}

/*
void BLEUart::disconnect(); // AT -> OK/OK+LOST
void BLEUart::getAddress(); // AT+ADDR? -> OK+ADDR:MAC Address

uint8_t BLEUart::getAdvertisingInterval(); // AT+ADVI? -> OK+Get:[0-F]
void BLEUart::setAdvertisingInterval(uint8_t); // AT+ADVI[0-F] -> OK+Set:[0-F]

Ble::advertising_t BLEUart::getAdvertisingType(); // AT+ADTY? -> OK+Get:[0-3]
void BLEUart::setAdvertisingType(Ble::advertising_t); // AT+ADTY[0-3] -> OK+Set:[0-3]

ModulePin::output_state_t BLEUart::getConnectedModulePinOutputState(); // AT+AFTC? -> OK+Get:[0-3]
void BLEUart::setConnectedModulePinOutputState(ModulePin::output_state_t); // AT+AFTC[0-3] -> OK+Set:[0-3]

ModulePin::output_state_t BLEUart::getInitialModulePinOutputState(); // AT+BEFC? -> OK+Get:[0-3]
void BLEUart::setInitialModulePinOutputState(ModulePin::output_state_t); // AT+BEFC[0-3] -> OK+Set:[0-3]

Uart::rate_t BLEUart::getBaudRate(); // AT+BAUD? -> OK+Get:[0-8]
void BLEUart::setBaudRate(Uart::rate_t); // AT+BAUD[0-8] -> OK+Set:[0-8]

uint16_t BLEUart::getCharacteristic(); // AT+CHAR? -> OK+Get:'0x'[0-F]{4}
void BLEUart::setCharacteristic(uint16_t); // AT+CHAR'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}

uint8_t BLEUart::getMinimumLinkLayerConnectionInterval(); // AT+COMI? -> OK+Get:[0-9]
void BLEUart::setMinimumLinkLayerConnectionInterval(uint8_t); // AT+COMI[0-9] -> OK+Set:[0-9]

uint8_t BLEUart::getMaximumLinkLayerConnectionInterval(); // AT+COMA? -> OK+Get:[0-9]
void BLEUart::setMaximumLinkLayerConnectionInterval(uint8_t); // AT+COMA[0-9] -> OK+Set:[0-9]

uint8_t BLEUart::getLinkLayerConnectionSlaveLatency(); // AT+COLA? -> OK+Get:[0-4]
void BLEUart::setLinkLayerConnectionSlaveLatency(uint8_t); // AT+COLA[0-4] -> OK+Set:[0-4]

uint8_t BLEUart::getUnknownInterval(); // AT+COSU? -> OK+Get:[0-6]
void BLEUart::setUnknownInterval(uint8_t); // AT+COSU[0-6] -> OK+Set:[0-6]

bool BLEUart::getUpdateConnection(); // AT+COUP? -> OK+Get:[0-1]
void BLEUart::setUpdateConnection(bool); // AT+COUP[0-1] -> OK+Set:[0-1]

void BLEUart::clearLastConnectedDeviceAddress(); // AT+CLEAR -> OK+CLEAR
Connection::response_t BLEUart::reconnect(); // AT+CONNL -> OK+CONN[LEFN]
Connection::response_t BLEUart::connectId(uint8_t); // AT+CONN[0-5] -> OK+CONN[AEF]
Connection::response_t BLEUart::connectAddress(uint8_t[6]); // AT+CO[N1][0-2][0-9]{6} -> OK+CO[N1][N1][AEF]

void BLEUart::discoverDevices(); // AT+DISC? -> OK+DIS[C0-2]([SE]|[0-F]{6})
uint8_t BLEUart::devicesCount();
device_t BLEUart::getDevice(uint8_t);

void BLEUart::discoverBeacons(); // AT+DISI? -> OK+DISC????
uint8_t BLEUart::beaconsCount();
beacon_t BLEUart::getBeacon(uint8_t);

bool BLEUart::characteristic2Enabled(); // AT+FFE2? -> OK+Get:[0-1]
void BLEUart::enableCharacteristic2(bool); // AT+FFE2[0-1] -> OK+Set:[0-1]

// AT+FLOW? not supported yet as of V1 datasheet
// AT+FLOW[0-1] not supported yet as of V1 datasheet

bool BLEUart::highTxGainEnabled(); // AT+GAIT? -> OK+Get:[0-1]
void BLEUart::enableHighTxGain(bool); // AT+GAIT[0-1] -> OK+Set:[0-1]

bool BLEUart::highRxGainEnabled(); // AT+GAIN? -> OK+Get:[0-1]
void BLEUart::enableHighRxGain(bool); // AT+GAIN[0-1] -> OK+Set:[0-1]

string BLEUart::help(); // AT+HELP? -> String

bool BLEUart::manualStartupEnabled(); // AT+IMME? -> OK+Get:[0-1]
void BLEUart::enableManualStartup(bool); // AT+IMME[0-1] -> OK+Set:[0-1]

bool BLEUart::beaconEnabled(); // AT+IBEA? -> OK+Get:[0-1]
void BLEUart::enableBeacon(bool); // AT+IBEA[0-1] -> OK+Set:[0-1]

string BLEUart::getBeaconUuid(); // AT+IBE0? -> OK+Get:[0-F]{4}, AT+IBE1, AT+IBE2, AT+IBE3
void BLEUart::setBeaconUuid(string); // AT+IBE0[0-F]{4} -> OK+Set:[0-F]{4}, AT+IBE1, AT+IBE2, AT+IBE3

uint16_t BLEUart::getBeaconMajorVersion(); // AT+MARJ? -> OK+Get:[0-F]{4}
void BLEUart::setBeaconMajorVersion(uint16_t); // AT+MARJ[0-F]{4} -> OK+Set:[0-F]{4}

uint16_t BLEUart::getBeaconMinorVersion(); // AT+MINO? -> OK+Get:[0-F]{4}
void BLEUart::setBeaconMinorVersion(uint16_t); // AT+MINO[0-F]{4} -> OK+Set:[0-F]{4}

uint8_t BLEUart::getBeaconMeasuredPower(); // AT+MEAS? -> OK+Get:[0-F]{2}
void BLEUart::setBeaconMeasuredPower(uint8_t); // AT+MEAS[0-F]{2} -> OK+Set:[0-F]{2}

Uart::mode_t BLEUart::getUartMode(); // AT+MODE? -> OK+Get:[0-2]
void BLEUart::setUartMode(Uart::mode_t); // AT+MODE[0-2] -> OK+Set:[0-2]

bool BLEUart::notificationsEnabled(); // AT+NOTI? -> OK+Get:[0-1]
void BLEUart::enableNotifications(bool); // AT+NOTI[0-1] -> OK+Set:[0-1]
bool BLEUart::getNotifyAddressOnDisconnect(); // AT+NOTP? -> OK+Get:[0-1]
void BLEUart::setNotifyAddressOnDisconnect(bool); // AT+NOTP[0-1] -> OK+Set:[0-1]

string BLEUart::getModuleName(); // AT+NAME? -> OK+NAME\w{1,13}
void BLEUart::setModuleName(string); // AT+NAME\w{1,13} -> OK+Set\w{1,13}

Uart::parity_t BLEUart::getParity(); // AT+PARI? -> OK+Get:[0-2]
void BLEUart::setParity(Uart::parity_t); // AT+PARI[0-2] -> OK+Set:[0-2]

bool BLEUart::getPio(uint8_t); // AT+PIO[1-B]? -> OK+PIO[1-B]:[0-1]
void BLEUart::setPio(uint8_t, bool); // AT+PIO[1-B][0-1] -> OK+PIO[1-B]:[0-1]

bool BLEUart::autoSleepEnabled(); // AT+PWRM? -> OK+Get:[0-1]
void BLEUart::enableAutoSleep(bool); // AT+PWRM[0-1] -> OK+Set:[0-1]

Ble::power_t BLEUart::getModulePower(); // AT+POWE? -> OK+Get:[0-7]
void BLEUart::setModulePower(Ble::power_t); // AT+POWE[0-7] -> OK+Set:[0-7]

bool BLEUart::reliableAdvertisingEnabled(); // AT+RELI? -> OK+Get:[0-1]
void BLEUart::enableReliableAdvertising(bool); // AT+RELI[0-1] -> OK+Set:[0-1]

void BLEUart::resetConfiguration(); // AT+RENEW -> OK+RENEW
void BLEUart::reset(); // AT+RESET -> OK+RESET

Ble::role_t BLEUart::getRole(); // AT+ROLE? -> OK+Get:[0-1]
void BLEUart::setRole(Ble::role_t); // AT+ROLE[0-1] -> OK+Set:[0-1]

string BLEUart::getLastConnected(); // AT+RADD? -> OK+RADD:[0-F]{6}

Ble::talk_t BLEUart::getTalkMethod(); // AT+RESP? -> OK+Get:[0-2]
void BLEUart::setTalkMethod(Ble::talk_t); // AT+RESP[0-2] -> OK+Set:[0-2]

ModulePin::system_key_t BLEUart::getSystemKeySetting(); // AT+SYSK? -> OK+Get:[0-1]
void BLEUart::setSystemKeySetting(ModulePin::system_key_t); // AT+SYSK[0-1] -> OK+Set:[0-1]

Uart::stop_t BLEUart::getStopBit(); // AT+STOP? -> OK+Get:[0-1]
void BLEUart::setStopBit(Uart::stop_t); // AT+STOP[0-1] -> OK+Set:[0-1]

void BLEUart::sleep(); // AT+SLEEP -> OK+SLEEP

void BLEUart::start(); // AT+START -> OK+START

uint8_t BLEUart::getScanDuration(); // AT+SCAN? -> OK+Get[1-5]
void BLEUart::setScanDuration(uint8_t); // AT+SCAN[1-5] -> OK+Set[1-5]

bool BLEUart::saveLastDeviceEnabled(); // AT+SAVE? -> OK+Get:[0-1]
void BLEUart::enableSaveLastDevice(bool); // AT+SAVE[0-1] -> OK+Set:[0-1]

bool BLEUart::nameDiscoveryEnabled(); // AT+SHOW? -> OK+Get:[0-1]
void BLEUart::enableNameDiscovery(bool); // AT+SHOW[0-1] -> OK+Set:[0-1]

uint16_t BLEUart::getServiceUuid(); // AT+UUID? -> OK+Get:'0x'[0-F]{4}
void BLEUart::setServiceUuid(uint16_t); // AT+UUID'0x'[0-F]{4} -> OK+Set:'0x'[0-F]{4}

string BLEUart::getFirmwareVersion(); // AT+VERR/AT+VERS -> String
*/
