#include "ESPAPP_API_Files.h"

#ifdef DEBUG
ESPAPP_API_Files::ESPAPP_API_Files(ESPAPP_API_Flash *_Flash, ESPAPP_SerialDebugger *_Debugger)
{
  this->Debugger = _Debugger;
  this->Flash = _Flash;
}
#else
ESPAPP_API_Files::ESPAPP_API_Files(ESPAPP_API_Flash *_Flash)
{
  this->Flash = _Flash;
  /** Deleting all files from temporary files folder */
  this->Flash->deleteAllFilesInDirectory(FPSTR(path_temp));
}
#endif

/** File: connection.json */
bool ESPAPP_API_Files::read(ESPAPP_NETWORK_CONNECTION_MODE *data)
{

  bool success = false;

#ifdef DEBUG
  this->Debugger->printHeader(2, 0, 72, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
  this->Debugger->printInformation(F("Reading connection mode configuration"), F("API-FILE"));
#endif

  StaticJsonDocument<256> doc;
  success = this->Flash->getJSON(F("/boot/connection.json"), doc);

  if (success)
  {
    *data = doc["mode"] | ESPAPP_NETWORK_CONNECTION_MODE::ACCESS_POINT;

#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Connection mode: "));
    this->Debugger->printValue(*data);
#endif
  }
  else
  {
    #ifdef DEBUG
    this->Debugger->printBulletPoint(F("Failed to load Connection Mode configuration, creating default"), ESPAPP_DEBUGGER_MESSAGE_COLOR::BLUE);
#endif
    success = this->createDefaultConnectionModeConfiguration();
  }

#ifdef DEBUG
  this->Debugger->printHeader(1, 1, 72, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
#endif

  return success;
}

bool ESPAPP_API_Files::save(ESPAPP_NETWORK_CONNECTION_MODE *data)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Saving connection mode configuration: "), F("API-FILE"));
  this->Debugger->printValue(data);
#endif
  StaticJsonDocument<256> doc;
  doc["mode"] = *data;
  return this->Flash->saveJSON(F("/boot/connection.json"), doc);
}

bool ESPAPP_API_Files::createDefaultConnectionModeConfiguration(void)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Creating default connection mode configuration"), F("API-FILE"));
#endif

  ESPAPP_NETWORK_CONNECTION_MODE *config = new ESPAPP_NETWORK_CONNECTION_MODE;
  *config = ESPAPP_NETWORK_CONNECTION_MODE::ACCESS_POINT; // @TODO change to AP for Prod
  bool success = save(config);
  delete config;
  return success;
}

/** File: mode.json */
bool ESPAPP_API_Files::read(ESPAPP_OPERATING_MODE *data)
{
  bool success = false;

#ifdef DEBUG
  this->Debugger->printHeader(2, 0, 72, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
  this->Debugger->printInformation(F("Reading Operating mode configuration"), F("API-FILE"));
#endif

  StaticJsonDocument<256> doc;
  success = this->Flash->getJSON(F("/boot/mode.json"), doc);

  if (success)
  {
    *data = doc["mode"] | ESPAPP_OPERATING_MODE::FIRST_TIME_BOOT;

#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Operating mode: "));
    this->Debugger->printValue(*data);
#endif
  }
  else
  {
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Failed to load Operating Mode, creating default"), ESPAPP_DEBUGGER_MESSAGE_COLOR::BLUE);
#endif
    success = this->createDefaultOperatingModeConfiguration();
  }

#ifdef DEBUG
  this->Debugger->printHeader(1, 1, 72, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
#endif

  return success;
}

bool ESPAPP_API_Files::save(ESPAPP_OPERATING_MODE *data)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Saving Operating mode configuration: "), F("API-FILE"));
  this->Debugger->printValue(*data);
#endif
  StaticJsonDocument<256> doc;
  doc["mode"] = *data;
  return this->Flash->saveJSON(F("/boot/mode.json"), doc);
}

bool ESPAPP_API_Files::createDefaultOperatingModeConfiguration()
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Creating default Operating mode configuration"), F("API-FILE"));
#endif
  ESPAPP_OPERATING_MODE *config = new ESPAPP_OPERATING_MODE;
  *config = ESPAPP_OPERATING_MODE::FIRST_TIME_BOOT;
  bool success = save(config);
  delete config;
  return success;
}

/** File: network.json */
bool ESPAPP_API_Files::read(ESPAPP_NETWORK *data)
{
  bool success = false;

#ifdef DEBUG
  this->Debugger->printHeader(2, 0, 72, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
  this->Debugger->printInformation(F("Reading network configuration"), F("API-FILE"));
#endif

  StaticJsonDocument<1024> doc;
  success = this->Flash->getJSON(F("/etc/network.json"), doc);

  if (success)
  {
    // Primary network
    strlcpy(data->primary->ssid, doc["primary"]["ssid"] | ESPAPP_NETWORK_DEFAULT_NONE_SSID, sizeof(data->primary->ssid));
    strlcpy(data->primary->password, doc["primary"]["password"] | "", sizeof(data->primary->password));
    data->primary->isDHCP = doc["primary"]["isDHCP"] | ESPAPP_NETWORK_DEFAULT_DHCP;
    strlcpy(data->primary->ip, doc["primary"]["ip"] | "", sizeof(data->primary->ip));
    strlcpy(data->primary->gateway, doc["primary"]["gateway"] | ESPAPP_NETWORK_DEFAULT_GATEWAY, sizeof(data->primary->gateway));
    strlcpy(data->primary->subnet, doc["primary"]["subnet"] | ESPAPP_NETWORK_DEFAULT_SUBNET, sizeof(data->primary->subnet));
    strlcpy(data->primary->dns1, doc["primary"]["dns1"] | ESPAPP_NETWORK_DEFAULT_DNS1, sizeof(data->primary->dns1));
    strlcpy(data->primary->dns2, doc["primary"]["dns2"] | ESPAPP_NETWORK_DEFAULT_DNS2, sizeof(data->primary->dns2));

    // Secondary network
    strlcpy(data->secondary->ssid, doc["secondary"]["ssid"] | ESPAPP_NETWORK_DEFAULT_NONE_SSID, sizeof(data->secondary->ssid));
    strlcpy(data->secondary->password, doc["secondary"]["password"] | "", sizeof(data->secondary->password));
    data->secondary->isDHCP = doc["secondary"]["isDHCP"] | ESPAPP_NETWORK_DEFAULT_DHCP;
    strlcpy(data->secondary->ip, doc["secondary"]["ip"] | "", sizeof(data->secondary->ip));
    strlcpy(data->secondary->gateway, doc["secondary"]["gateway"] | ESPAPP_NETWORK_DEFAULT_GATEWAY, sizeof(data->secondary->gateway));
    strlcpy(data->secondary->subnet, doc["secondary"]["subnet"] | ESPAPP_NETWORK_DEFAULT_SUBNET, sizeof(data->secondary->subnet));
    strlcpy(data->secondary->dns1, doc["secondary"]["dns1"] | ESPAPP_NETWORK_DEFAULT_DNS1, sizeof(data->secondary->dns1));
    strlcpy(data->secondary->dns2, doc["secondary"]["dns2"] | ESPAPP_NETWORK_DEFAULT_DNS2, sizeof(data->secondary->dns2));

    // General settings
    data->connectionTimeout = doc["connectionTimeout"] | ESPAPP_NETWORK_DEFAULT_CONNECTION_TIMEOUT;
    data->sleepTimeout = doc["sleepTimeout"] | ESPAPP_NETWORK_DEFAULT_SLEEP_TIMEOUT;
    data->failuresToSwitch = doc["failuresToSwitch"] | ESPAPP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER;
    data->mDNS = doc["mDNS"] | ESPAPP_NETWORK_DEFAULT_MDNS;

#if !defined(ESP32)
    data->radioMode = doc["radioMode"] | ESPAPP_NETWORK_DEFAULT_RADIO_MODE;
    data->outputPower = doc["outputPower"] | ESPAPP_NETWORK_DEFAULT_OUTPUT_POWER_MAX;
#endif

#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Network configuration loaded"));
    this->Debugger->printBulletPoint(F("Primary SSID: "));
    this->Debugger->printValue(data->primary->ssid);
    this->Debugger->printBulletPoint(F("Primary password: hidden"));
    this->Debugger->printBulletPoint(F("Primary isDHCP: "));
    this->Debugger->printValue(data->primary->isDHCP);
    this->Debugger->printBulletPoint(F("Primary IP: "));
    this->Debugger->printValue(data->primary->ip);
    this->Debugger->printBulletPoint(F("Primary gateway: "));
    this->Debugger->printValue(data->primary->gateway);
    this->Debugger->printBulletPoint(F("Primary subnet: "));
    this->Debugger->printValue(data->primary->subnet);
    this->Debugger->printBulletPoint(F("Primary DNS1: "));
    this->Debugger->printValue(data->primary->dns1);
    this->Debugger->printBulletPoint(F("Primary DNS2: "));
    this->Debugger->printValue(data->primary->dns2);
    this->Debugger->printBulletPoint(F("Secondary SSID: "));
    this->Debugger->printValue(data->secondary->ssid);
    this->Debugger->printBulletPoint(F("Secondary password: hidden"));
    this->Debugger->printBulletPoint(F("Secondary isDHCP: "));
    this->Debugger->printValue(data->secondary->isDHCP);
    this->Debugger->printBulletPoint(F("Secondary IP: "));
    this->Debugger->printValue(data->secondary->ip);
    this->Debugger->printBulletPoint(F("Secondary gateway: "));
    this->Debugger->printValue(data->secondary->gateway);
    this->Debugger->printBulletPoint(F("Secondary subnet: "));
    this->Debugger->printValue(data->secondary->subnet);
    this->Debugger->printBulletPoint(F("Secondary DNS1: "));
    this->Debugger->printValue(data->secondary->dns1);
    this->Debugger->printBulletPoint(F("Secondary DNS2: "));
    this->Debugger->printValue(data->secondary->dns2);
    this->Debugger->printBulletPoint(F("Connection timeout: "));
    this->Debugger->printValue(data->connectionTimeout);
    this->Debugger->printBulletPoint(F("Sleep timeout: "));
    this->Debugger->printValue(data->sleepTimeout);
    this->Debugger->printBulletPoint(F("Failures to switch: "));
    this->Debugger->printValue(data->failuresToSwitch);
    this->Debugger->printBulletPoint(F("mDNS: "));
    this->Debugger->printValue(data->mDNS);

#endif
  }
  else
  {
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Failed to load Network configuration, creating default"), ESPAPP_DEBUGGER_MESSAGE_COLOR::BLUE);
#endif
    success = this->createDefaultNetworkConfiguration();
  }

#ifdef DEBUG
  this->Debugger->printHeader(1, 1, 72, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
#endif

  return success;
}

bool ESPAPP_API_Files::save(ESPAPP_NETWORK *data)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Saving network configuration"), F("API-FILE"));
#endif

  StaticJsonDocument<1024> doc;

  // Primary network
  JsonObject primary = doc.createNestedObject("primary");
  primary["ssid"] = data->primary->ssid;
  primary["password"] = data->primary->password;
  primary["isDHCP"] = data->primary->isDHCP;
  primary["ip"] = data->primary->ip;
  primary["gateway"] = data->primary->gateway;
  primary["subnet"] = data->primary->subnet;
  primary["dns1"] = data->primary->dns1;
  primary["dns2"] = data->primary->dns2;

  // Secondary network
  JsonObject secondary = doc.createNestedObject("secondary");
  secondary["ssid"] = data->secondary->ssid;
  secondary["password"] = data->secondary->password;
  secondary["isDHCP"] = data->secondary->isDHCP;
  secondary["ip"] = data->secondary->ip;
  secondary["gateway"] = data->secondary->gateway;
  secondary["subnet"] = data->secondary->subnet;
  secondary["dns1"] = data->secondary->dns1;
  secondary["dns2"] = data->secondary->dns2;

  // General settings
  doc["connectionTimeout"] = data->connectionTimeout;
  doc["sleepTimeout"] = data->sleepTimeout;
  doc["failuresToSwitch"] = data->failuresToSwitch;
  doc["mDNS"] = data->mDNS;

#if !defined(ESP32)
  doc["radioMode"] = data->radioMode;
  doc["outputPower"] = data->outputPower;
#endif

  return this->Flash->saveJSON(F("/etc/network.json"), doc);
}

bool ESPAPP_API_Files::createDefaultNetworkConfiguration(void)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Creating default network configuration"), F("API-FILE"));
#endif

  ESPAPP_NETWORK *config = new ESPAPP_NETWORK();

  // Primary network defaults
  strlcpy(config->primary->ssid, ESPAPP_NETWORK_DEFAULT_NONE_SSID, sizeof(config->primary->ssid));
  config->primary->password[0] = ESPAPP_EMPTY_STRING_TERMINATED;

  config->primary->isDHCP = ESPAPP_NETWORK_DEFAULT_DHCP;
  config->primary->ip[0] = ESPAPP_EMPTY_STRING_TERMINATED;
  strlcpy(config->primary->gateway, ESPAPP_NETWORK_DEFAULT_GATEWAY, sizeof(config->primary->gateway));
  strlcpy(config->primary->subnet, ESPAPP_NETWORK_DEFAULT_SUBNET, sizeof(config->primary->subnet));
  strlcpy(config->primary->dns1, ESPAPP_NETWORK_DEFAULT_DNS1, sizeof(config->primary->dns1));
  strlcpy(config->primary->dns2, ESPAPP_NETWORK_DEFAULT_DNS2, sizeof(config->primary->dns2));

  // Secondary network defaults
  strlcpy(config->secondary->ssid, ESPAPP_NETWORK_DEFAULT_NONE_SSID, sizeof(config->secondary->ssid));
  config->secondary->password[0] = ESPAPP_EMPTY_STRING_TERMINATED;
  config->secondary->isDHCP = ESPAPP_NETWORK_DEFAULT_DHCP;
  config->secondary->ip[0] = ESPAPP_EMPTY_STRING_TERMINATED;
  strlcpy(config->secondary->gateway, ESPAPP_NETWORK_DEFAULT_GATEWAY, sizeof(config->secondary->gateway));
  strlcpy(config->secondary->subnet, ESPAPP_NETWORK_DEFAULT_SUBNET, sizeof(config->secondary->subnet));
  strlcpy(config->secondary->dns1, ESPAPP_NETWORK_DEFAULT_DNS1, sizeof(config->secondary->dns1));
  strlcpy(config->secondary->dns2, ESPAPP_NETWORK_DEFAULT_DNS2, sizeof(config->secondary->dns2));

  // General network settings
  config->connectionTimeout = ESPAPP_NETWORK_DEFAULT_CONNECTION_TIMEOUT;
  config->sleepTimeout = ESPAPP_NETWORK_DEFAULT_SLEEP_TIMEOUT;
  config->failuresToSwitch = ESPAPP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER;
  config->mDNS = ESPAPP_NETWORK_DEFAULT_MDNS;

#if !defined(ESP32)
  config->radioMode = ESPAPP_NETWORK_DEFAULT_RADIO_MODE;
  config->outputPower = ESPAPP_NETWORK_DEFAULT_OUTPUT_POWER_MAX;
#endif

  bool result = save(config);
  delete config;
  return result;
}

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
/** File: acs758.json */
bool ESPAPP_API_Files::read(ACS758_CONFIG *data)
{
  bool success = false;

#ifdef DEBUG
  this->Debugger->printHeader(2, 0, 72, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
  this->Debugger->printInformation(F("Reading ACS758 sensor configuration"), F("API-FILE"));
#endif

  StaticJsonDocument<512> doc;
  success = this->Flash->getJSON(F("/etc/acs758.json"), doc);

  if (success)
  {
    data->sensorModel = (ACS758_MODEL)(doc["sensorModel"] | (int)ACS758_100B);
    data->analogPin = doc["analogPin"];
    data->vRef = doc["vRef"];
    data->adcResolution = doc["adcResolution"];
    data->calibration = doc["calibration"];
    data->readInterval = doc["readInterval"];
    data->samplesCount = doc["samplesCount"];

#ifdef DEBUG
    this->Debugger->printBulletPoint(F("ACS758 configuration loaded successfully"));
    this->Debugger->printBulletPoint(F("Sensor model: "));
    this->Debugger->printValue((int)data->sensorModel);
    this->Debugger->printBulletPoint(F("Read interval: "));
    this->Debugger->printValue(data->readInterval);
    this->Debugger->printBulletPoint(F("Analog pin: "));
    this->Debugger->printValue(data->analogPin);
    this->Debugger->printBulletPoint(F("ADC resolution: "));
    this->Debugger->printValue(data->adcResolution);
    this->Debugger->printBulletPoint(F("Calibration: "));
    this->Debugger->printValue(data->calibration);
    this->Debugger->printBulletPoint(F("Samples count: "));
    this->Debugger->printValue(data->samplesCount);
    this->Debugger->printBulletPoint(F("Reference voltage: "));
    this->Debugger->printValue(data->vRef);

#endif
  }
  else
  {
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Failed to load ACS758 configuration, creating default"), ESPAPP_DEBUGGER_MESSAGE_COLOR::BLUE);
#endif
    success = this->createDefaultACS758Configuration();
    if (success)
    {
      success = this->read(data); // Read the newly created configuration
    }
  }

#ifdef DEBUG
  this->Debugger->printHeader(1, 1, 72, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
#endif

  return success;
}

bool ESPAPP_API_Files::save(ACS758_CONFIG *data)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Saving ACS758 sensor configuration"), F("API-FILE"));
#endif

  StaticJsonDocument<512> doc;

  doc["sensorModel"] = (int)data->sensorModel;
  doc["analogPin"] = data->analogPin;
  doc["vRef"] = data->vRef;
  doc["adcResolution"] = data->adcResolution;
  doc["calibration"] = data->calibration;
  doc["readInterval"] = data->readInterval;
  doc["samplesCount"] = data->samplesCount;

  bool success = this->Flash->saveJSON(F("/etc/acs758.json"), doc);

#ifdef DEBUG
  if (success)
  {
    this->Debugger->printBulletPoint(F("ACS758 configuration saved successfully"));
  }
  else
  {
    this->Debugger->printError(F("Failed to save ACS758 configuration"), F("API-FILE"));
  }
#endif

  return success;
}

bool ESPAPP_API_Files::createDefaultACS758Configuration(void)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Creating default ACS758 configuration"), F("API-FILE"));
#endif

  ACS758_CONFIG config;

  // Set default values
  config.sensorModel = ACS758_100B;
  config.analogPin = 34;
  config.vRef = 3.3;
  config.adcResolution = 10;
  config.calibration = 1;
  config.readInterval = 5; // s
  config.samplesCount = 10;

  return this->save(&config);
}
#endif // ESPAPP_HARDWARE_ACS758_INCLUDED
