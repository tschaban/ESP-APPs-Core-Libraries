#include "ESPAPP_API_Files.h"

#ifdef DEBUG
ESPAPP_API_Files::ESPAPP_API_Files(ESPAPP_API_Flash *_Flash, ESPAPP_SerialMessages *_Msg)
{
  this->Msg = _Msg;
  this->Flash = _Flash;
}
#else
ESPAPP_API_Files::ESPAPP_API_Files(ESPAPP_API_Flash *_Flash)
{
  this->Flash = _Flash;
}
#endif

/** File: connection.json */
bool ESPAPP_API_Files::read(ESPAPP_NETWORK_CONNECTION_MODE *data)
{

  bool success = false;

#ifdef DEBUG
  this->Msg->printInformation(F("Reading connection mode configuration"), F("API-FILE"));
#endif

  StaticJsonDocument<256> doc;
  success = this->Flash->getJSON(F("/boot/connection.json"), doc);

  if (success)
  {
    *data = doc["mode"] | ESPAPP_NETWORK_CONNECTION_MODE::ACCESS_POINT; 

#ifdef DEBUG
    this->Msg->printBulletPoint(F("Connection mode: "));
    this->Msg->printValue(*data);
#endif
  }
  else
  {
    success = this->createDefaultConnectionModeConfiguration();
  }

  return success;
}

bool ESPAPP_API_Files::save(ESPAPP_NETWORK_CONNECTION_MODE *data)
{
#ifdef DEBUG
  this->Msg->printInformation(F("Saving connection mode configuration: "), F("API-FILE"));
  this->Msg->printValue(data);
#endif
  StaticJsonDocument<256> doc;
  doc["mode"] = *data;
  return this->Flash->saveJSON(F("/boot/connection.json"), doc);
}

bool ESPAPP_API_Files::createDefaultConnectionModeConfiguration(void)
{
#ifdef DEBUG
  this->Msg->printInformation(F("Creating default connection mode configuration"), F("API-FILE"));
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
    this->Msg->printInformation(F("Reading Operating mode configuration"), F("API-FILE"));
  #endif
  
    StaticJsonDocument<256> doc;
    success = this->Flash->getJSON(F("/boot/mode.json"), doc);
  
    if (success)
    {
      *data = doc["mode"] | ESPAPP_OPERATING_MODE::FIRST_TIME_BOOT;
  
  #ifdef DEBUG
      this->Msg->printBulletPoint(F("Operating mode: "));
      this->Msg->printValue(*data);
  #endif
    }
    else
    {
      success = this->createDefaultOperatingModeConfiguration();
    }
  
    return success;
}

bool ESPAPP_API_Files::save(ESPAPP_OPERATING_MODE *data)
{
  #ifdef DEBUG
  this->Msg->printInformation(F("Saving Operating mode configuration: "), F("API-FILE"));
  this->Msg->printValue(*data);
#endif
  StaticJsonDocument<256> doc;
  doc["mode"] = *data;
  return this->Flash->saveJSON(F("/boot/mode.json"), doc);
}

bool ESPAPP_API_Files::createDefaultOperatingModeConfiguration()
{
#ifdef DEBUG
  this->Msg->printInformation(F("Creating default Operating mode configuration"), F("API-FILE"));
#endif
  ESPAPP_OPERATING_MODE *config = new ESPAPP_OPERATING_MODE;
  *config = ESPAPP_OPERATING_MODE::FIRST_TIME_BOOT;
  bool success = save(config);
  delete config;
  return success;

}

/** File: network.json */
bool ESPAPP_API_Files::read(NETWORK *data)
{
  bool success = false;

#ifdef DEBUG
  this->Msg->printInformation(F("Reading network configuration"), F("API-FILE"));
#endif

  StaticJsonDocument<1024> doc;
  success = this->Flash->getJSON(F("/etc/network.json"), doc);

  if (success)
  {
    // Primary network
    strlcpy(data->primary.ssid, doc["primary"]["ssid"] | ESPAPP_NETWORK_DEFAULT_NONE_SSID, sizeof(data->primary.ssid));
    strlcpy(data->primary.password, doc["primary"]["password"] | "", sizeof(data->primary.password));
    data->primary.isDHCP = doc["primary"]["isDHCP"] | ESPAPP_NETWORK_DEFAULT_DHCP;
    strlcpy(data->primary.ip, doc["primary"]["ip"] | "", sizeof(data->primary.ip));
    strlcpy(data->primary.gateway, doc["primary"]["gateway"] | ESPAPP_NETWORK_DEFAULT_GATEWAY, sizeof(data->primary.gateway));
    strlcpy(data->primary.subnet, doc["primary"]["subnet"] | ESPAPP_NETWORK_DEFAULT_SUBNET, sizeof(data->primary.subnet));
    strlcpy(data->primary.dns1, doc["primary"]["dns1"] | ESPAPP_NETWORK_DEFAULT_DNS1, sizeof(data->primary.dns1));
    strlcpy(data->primary.dns2, doc["primary"]["dns2"] | ESPAPP_NETWORK_DEFAULT_DNS2, sizeof(data->primary.dns2));

    // Secondary network
    strlcpy(data->secondary.ssid, doc["secondary"]["ssid"] | ESPAPP_NETWORK_DEFAULT_NONE_SSID, sizeof(data->secondary.ssid));
    strlcpy(data->secondary.password, doc["secondary"]["password"] | "", sizeof(data->secondary.password));
    data->secondary.isDHCP = doc["secondary"]["isDHCP"] | ESPAPP_NETWORK_DEFAULT_DHCP;
    strlcpy(data->secondary.ip, doc["secondary"]["ip"] | "", sizeof(data->secondary.ip));
    strlcpy(data->secondary.gateway, doc["secondary"]["gateway"] | ESPAPP_NETWORK_DEFAULT_GATEWAY, sizeof(data->secondary.gateway));
    strlcpy(data->secondary.subnet, doc["secondary"]["subnet"] | ESPAPP_NETWORK_DEFAULT_SUBNET, sizeof(data->secondary.subnet));
    strlcpy(data->secondary.dns1, doc["secondary"]["dns1"] | ESPAPP_NETWORK_DEFAULT_DNS1, sizeof(data->secondary.dns1));
    strlcpy(data->secondary.dns2, doc["secondary"]["dns2"] | ESPAPP_NETWORK_DEFAULT_DNS2, sizeof(data->secondary.dns2));

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
    this->Msg->printBulletPoint(F("Network configuration loaded"));
#endif
  }
  else
  {
#ifdef DEBUG
    this->Msg->printWarning(F("Failed to load network configuration, creating default"), F("API-FILE"));
#endif
    success = this->createDefaultNetworkConfiguration();
  }

  return success;
}

bool ESPAPP_API_Files::save(NETWORK *data)
{
#ifdef DEBUG
  this->Msg->printInformation(F("Saving network configuration"), F("API-FILE"));
#endif
  
  StaticJsonDocument<1024> doc;

  // Primary network
  JsonObject primary = doc.createNestedObject("primary");
  primary["ssid"] = data->primary.ssid;
  primary["password"] = data->primary.password;
  primary["isDHCP"] = data->primary.isDHCP;
  primary["ip"] = data->primary.ip;
  primary["gateway"] = data->primary.gateway;
  primary["subnet"] = data->primary.subnet;
  primary["dns1"] = data->primary.dns1;
  primary["dns2"] = data->primary.dns2;

  // Secondary network
  JsonObject secondary = doc.createNestedObject("secondary");
  secondary["ssid"] = data->secondary.ssid;
  secondary["password"] = data->secondary.password;
  secondary["isDHCP"] = data->secondary.isDHCP;
  secondary["ip"] = data->secondary.ip;
  secondary["gateway"] = data->secondary.gateway;
  secondary["subnet"] = data->secondary.subnet;
  secondary["dns1"] = data->secondary.dns1;
  secondary["dns2"] = data->secondary.dns2;

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
  this->Msg->printInformation(F("Creating default network configuration"), F("API-FILE"));
#endif

  NETWORK *config = new NETWORK();
  
  // Primary network defaults
  strlcpy(config->primary.ssid, ESPAPP_NETWORK_DEFAULT_NONE_SSID, sizeof(config->primary.ssid));
  config->primary.password[0] = ESPAPP_EMPTY_STRING_TERMINATED;
    
  config->primary.isDHCP = ESPAPP_NETWORK_DEFAULT_DHCP;
  config->primary.ip[0] = ESPAPP_EMPTY_STRING_TERMINATED;
  strlcpy(config->primary.gateway, ESPAPP_NETWORK_DEFAULT_GATEWAY, sizeof(config->primary.gateway));
  strlcpy(config->primary.subnet, ESPAPP_NETWORK_DEFAULT_SUBNET, sizeof(config->primary.subnet));
  strlcpy(config->primary.dns1, ESPAPP_NETWORK_DEFAULT_DNS1, sizeof(config->primary.dns1));
  strlcpy(config->primary.dns2, ESPAPP_NETWORK_DEFAULT_DNS2, sizeof(config->primary.dns2));

  // Secondary network defaults
  strlcpy(config->secondary.ssid, ESPAPP_NETWORK_DEFAULT_NONE_SSID, sizeof(config->secondary.ssid));
  config->secondary.password[0] = ESPAPP_EMPTY_STRING_TERMINATED;
  config->secondary.isDHCP = ESPAPP_NETWORK_DEFAULT_DHCP;
  config->secondary.ip[0] = ESPAPP_EMPTY_STRING_TERMINATED;
  strlcpy(config->secondary.gateway, ESPAPP_NETWORK_DEFAULT_GATEWAY, sizeof(config->secondary.gateway));
  strlcpy(config->secondary.subnet, ESPAPP_NETWORK_DEFAULT_SUBNET, sizeof(config->secondary.subnet));
  strlcpy(config->secondary.dns1, ESPAPP_NETWORK_DEFAULT_DNS1, sizeof(config->secondary.dns1));
  strlcpy(config->secondary.dns2, ESPAPP_NETWORK_DEFAULT_DNS2, sizeof(config->secondary.dns2));

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
