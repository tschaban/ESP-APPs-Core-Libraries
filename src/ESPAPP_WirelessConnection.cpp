/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#include "ESPAPP_WirelessConnection.h"

boolean ESPAPP_WirelessConnection::isConnected = false;
boolean ESPAPP_WirelessConnection::eventConnectionEstablished = false;
boolean ESPAPP_WirelessConnection::eventConnectionLost = true;

ESPAPP_WirelessConnection::ESPAPP_WirelessConnection(ESPAPP_Core *_System)
{
  System = _System;
}

bool ESPAPP_WirelessConnection::init(void)
{

#ifdef DEBUG
  this->System->Msg->printInformation(F("Starting network"), F("WIFI"));
#endif

  /** Read network configuration  */
  this->ready = this->readConfiguration();

  if (!this->ready)
  {
#ifdef DEBUG
    this->System->Msg->printWarning(F("Failed to read network configuration, creating default"), F("WIFI"));
#endif
    this->ready = createDefaultConfiguration();
  }

  if (this->ready)
  {

/** Setting handlers for WiFi events */
#ifdef ESP32
    this->WirelessNetwork.onEvent(ESPAPP_WirelessConnection::onWiFiEvent);
#else
    wifiConnectHandler =
        this->WirelessNetwork.onStationModeGotIP(ESPAPP_WirelessConnection::onWifiConnect);
    wifiDisconnectHandler =
        this->WirelessNetwork.onStationModeDisconnected(ESPAPP_WirelessConnection::onWifiDisconnect);
#ifdef DEBUG
    wifiAPStationConnectedHandler = this->WirelessNetwork.onSoftAPModeStationConnected(
        ESPAPP_WirelessConnection::onWiFiAPStationConnected);
#endif
#endif

    /** Checkign is Beackup WiFi configuration exists */

#ifdef DEBUG
    this->System->Msg->printInformation(F("Secondary WiFi configuration exist: "), F("WIFI"));
#endif

    if (strlen(this->configuration->secondary.ssid) > 0 &&
        strcmp(this->configuration->secondary.ssid,
               ESPAPP_NETWORK_DEFAULT_NONE_SSID) != 0 &&
        strlen(this->configuration->secondary.password) > 0)
    {
      this->isBackupConfigurationSet = true;
    }

#ifdef DEBUG
    this->System->Msg->printValue(this->isBackupConfigurationSet);
#endif

#ifndef ESP32
    if (this->configuration->radioMode != AFE_NONE)
    {
      // wifi_set_phy_mode(this->configuration->radioMode);
      this->WirelessNetwork.setPhyMode(this->configuration->radioMode == 1
                                           ? WIFI_PHY_MODE_11B
                                       : this->configuration->radioMode == 2
                                           ? WIFI_PHY_MODE_11G
                                           : WIFI_PHY_MODE_11N);
#ifdef DEBUG
      this->System->Msg->printInformation(F("Setting Radio mode (1:B 2:G 3:N) to: "),
                                          F("WIFI"));
      this->System->Msg->printValue(this->configuration->radioMode);
#endif
    }

    if (this->configuration->outputPower != AFE_NONE &&
        this->configuration->outputPower >=
            AFE_CONFIG_NETWORK_DEFAULT_OUTPUT_POWER_MIN &&
        this->configuration->outputPower <=
            AFE_CONFIG_NETWORK_DEFAULT_OUTPUT_POWER_MAX)
    {
      this->WirelessNetwork.setOutputPower(this->configuration->outputPower);
#ifdef DEBUG
      this->System->Msg->printInformation(F("Setting TX Output power to: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->outputPower, F("dBm"));
#endif
    }

#endif

    if (this->System->getConnectionMode() == ESPAPP_NETWORK_CONNECTION_MODE_ACCESS_POINT)
    {
#ifdef DEBUG
      this->System->Msg->printInformation(F("Starting Hotspot..."), F("WIFI"));
#endif

      this->WirelessNetwork.mode(WIFI_AP_STA);

      this->ready = this->WirelessNetwork.softAPConfig(IPAddress(192, 168, 5, 1),
                                                       IPAddress(192, 168, 5, 200),
                                                       IPAddress(255, 255, 255, 0));

      if (this->ready)
      {
        this->WirelessNetwork.softAP(this->System->configuration->deviceName);
#ifdef DEBUG
        this->System->Msg->printValue(F("Ready"));
      }
      else
      {
        this->System->Msg->printValue(F("Failed"));
#endif
      }
    }
    else
    {
      /* Add additional configuration parameters */
      this->switchConfiguration();
    }
#ifdef DEBUG
    this->System->Msg->printInformation(F("Network initialized..."), F("BOOT"));
#endif
  }
#ifdef DEBUG
  else
  {
    this->System->Msg->printError(F("Network cannot be initialized"), F("WIFI"));
  }
#endif

  return this->ready;
}

void ESPAPP_WirelessConnection::switchConfiguration()
{
  this->isPrimaryConfiguration = this->isPrimaryConfiguration ? false : true;
  this->noOfFailures = 0;
  this->WirelessNetwork.persistent(false);
  this->WirelessNetwork.disconnect(true);

#ifndef ESP32
  this->WirelessNetwork.setSleepMode(WIFI_NONE_SLEEP);
#else
  WiFi.setSleep(false);
#endif

  /**
   * @brief Setting Fixed IP for Primary Configuration if set
   *
   */
  if (this->isPrimaryConfiguration && !this->configuration->primary.isDHCP)
  {

    IPAddress dns1;
    if (!dns1.fromString(this->configuration->primary.dns1))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with DNS1: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->primary.dns1);
#endif
    }
    IPAddress dns2;
    if (!dns2.fromString(this->configuration->primary.dns2))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with DNS2: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->primary.dns2);
#endif
    }

    IPAddress ip;
    if (!ip.fromString(this->configuration->primary.ip))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with WIFI IP: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->primary.ip);
#endif
    }
    IPAddress gateway;
    if (!gateway.fromString(this->configuration->primary.gateway))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with WIFI gateway: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->primary.gateway);
#endif
    }
    IPAddress subnet;
    if (!subnet.fromString(this->configuration->primary.subnet))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with WIFI subnet: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->primary.subnet);
#endif
    }

    this->WirelessNetwork.config(ip, gateway, subnet, dns1, dns2);
#ifdef DEBUG
    this->System->Msg->printInformation(F("Fixed IP set"), F("WIFI"));
#endif
  }
  /**
   * @brief set IPs to 0 for configuration over DHCP
   *
   */

  else if ((this->isPrimaryConfiguration && this->configuration->primary.isDHCP) ||
           (!this->isPrimaryConfiguration && this->configuration->secondary.isDHCP))
  {
    this->WirelessNetwork.config((uint32_t)0x00000000, (uint32_t)0x00000000,
                                 (uint32_t)0x00000000);
  }

  /**
   * @brief Setting fixed IP for backup WiFi configurations
   *
   */
  else if (!this->isPrimaryConfiguration && !this->configuration->secondary.isDHCP)
  {
#ifdef DEBUG
    this->System->Msg->printInformation(F("Setting fixed IP ("), F("WIFI"));
    this->System->Msg->printValue(this->configuration->secondary.ip);
    this->System->Msg->printValue(F(") address for backup WiFi configuration"));
#endif

    IPAddress dns1;
    if (!dns1.fromString(this->configuration->secondary.dns1))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with WIFI DNS1: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->secondary.dns1);
#endif
    }
    IPAddress dns2;
    if (!dns2.fromString(this->configuration->secondary.dns2))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with WIFI DNS2: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->secondary.dns2);
#endif
    }

    IPAddress ip;
    if (!ip.fromString(this->configuration->secondary.ip))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with WIFI IP: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->secondary.ip);
#endif
    }
    IPAddress gateway;
    if (!gateway.fromString(this->configuration->secondary.gateway))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with WIFI gateway: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->secondary.gateway);
#endif
    }
    IPAddress subnet;
    if (!subnet.fromString(this->configuration->secondary.subnet))
    {
#ifdef DEBUG
      this->System->Msg->printError(F("Problem with WIFI subnet: "), F("WIFI"));
      this->System->Msg->printValue(this->configuration->secondary.subnet);
#endif
    }

    this->WirelessNetwork.config(ip, gateway, subnet, dns1, dns2);
#ifdef DEBUG
    this->System->Msg->printInformation(F("Fixed IP set"), F("WIFI"));
#endif
  } /* Endif: Setting Fixed IP for Primary Configuration if set */

  this->WirelessNetwork.mode(WIFI_STA);
  this->System->Events->triggerEvent(EVENT_WIFI_CONFIG_CHANGED);

#ifdef DEBUG
  this->System->Msg->printInformation(
      (this->isPrimaryConfiguration ? F("Primary") : F("Backup")), F("WIFI"));
  this->System->Msg->printValue(F(" configuration set"));
#endif
}

void ESPAPP_WirelessConnection::listener()
{
  // If network configuration is not set then exit
  if (!ready)
  {
    return;
  }

  // If work offline mode is set then exit
  if (this->System->getConnectionMode() == ESPAPP_NETWORK_CONNECTION_MODE_NO_CONNECTION)
  {
    return;
  }

  /** Trigger connected event */
  this->connectionEvent();

  /** Connection is esthablished to the WiFi Router */
  if (this->connection() == ESPAPP_NETWORK_CONNECTION_MODE_CLIENT)
  {
    /** Update mDNS */
    if (this->configuration->mDNS)
    {
#ifndef ESP32
      MDNS.update();
#else
// @TODO 3.8.0 what is ESP32 equivalent ESP32
#endif
    }

    if (this->connections > 0)
    {
      this->connections = 0;
      this->noOfFailures = 0;
      this->delayStartTime = 0;

#ifdef AFE_CONFIG_HARDWARE_LED
      ledStartTime = 0;
      Led->off();
#endif
    }
    return;
  }

  if (this->connection() == ESPAPP_NETWORK_CONNECTION_MODE_NO_CONNECTION)
  {
    /** Sleep Mode */
    if (this->sleepMode)
    {
      if (millis() - this->sleepStartTime >= this->configuration->sleepTimeout * 1000)
      {
        this->sleepMode = false;
#ifdef DEBUG
        this->System->Msg->printBulletPoint(F("Sleep timeout completed, resuming connection attempts"));
#endif
      }
      return;
    }

    /** (Re)Starting connection */
    if (this->delayStartTime == 0)
    {
      this->delayStartTime = millis();
      this->scecondsTimer = this->delayStartTime;
      if (this->connections == 0)
      {
        if (strlen(this->configuration->primary.ssid) == 0 || strlen(this->configuration->primary.password) == 0)
        {
#ifdef DEBUG
          this->System->Msg->printError(F("WiFi not configured, switching to Hotspot mode"), F("WIFI"));
#endif
          this->System->setConnectionMode(ESPAPP_NETWORK_CONNECTION_MODE_ACCESS_POINT);
          this->System->Events->triggerEvent(EVENT_REBOOT);
          return;
        }

        if (this->isPrimaryConfiguration)
        {
          this->WirelessNetwork.begin(this->configuration->primary.ssid, this->configuration->primary.password);
#ifdef DEBUG
          this->System->Msg->printBulletPoint(F("Attempting connection to primary network"));
          this->System->Msg->printBulletPoint(F("SSID: "));
          this->System->Msg->printValue(this->configuration->primary.ssid);
#endif
        }
        else
        {
          this->WirelessNetwork.begin(this->configuration->secondary.ssid, this->configuration->secondary.password);
#ifdef DEBUG
          this->System->Msg->printBulletPoint(F("Attempting connection to secondary network"));
          this->System->Msg->printBulletPoint(F("SSID: "));
          this->System->Msg->printValue(this->configuration->secondary.ssid);
#endif
        }
      }
    }

#ifdef AFE_CONFIG_HARDWARE_LED
    if (ledStartTime == 0)
    {
      ledStartTime = millis();
    }

    if (millis() > ledStartTime + 500)
    {
      Led->toggle();
      ledStartTime = 0;
    }
#endif

    /** Timeout  */
    if (millis() > this->delayStartTime + (this->configuration->connectionTimeout * 1000))
    {

      this->delayStartTime = 0;
      this->sleepMode = true;
      this->WirelessNetwork.disconnect();
      this->sleepStartTime = millis();

#ifdef AFE_CONFIG_HARDWARE_LED
      ledStartTime = 0;
      Led->off();
#endif

      this->connections = 0;

#ifdef DEBUG
      this->System->Msg->printBulletPoint(F("Connection failed. Entering sleep mode for "));
      this->System->Msg->printValue(this->configuration->sleepTimeout, F(" seconds"));
#endif

      if (this->isBackupConfigurationSet)
      {
        this->noOfFailures++;

        if (this->noOfFailures >= this->configuration->failuresToSwitch)
        {
#ifdef DEBUG
          this->System->Msg->printBulletPoint(F("Switching network configuration to "));
          this->System->Msg->printValue(this->isPrimaryConfiguration ? F("secondary") : F("primary"));
#endif
          this->switchConfiguration();
        }
      }
    }

    /** Displaying connection status and counting connection attempts */
    if (millis() > this->scecondsTimer + 1000)
    {
      this->scecondsTimer = millis();
      this->connections++;
#ifdef DEBUG
      this->System->Msg->printBulletPoint(F("Establishing connection: "));
      this->System->Msg->printValue(this->connections);
      this->System->Msg->printValue(F("s, Status="));
      this->System->Msg->printValue((int)this->WirelessNetwork.status());
      if (this->isBackupConfigurationSet)
      {
        this->System->Msg->printValue(F("Failures: "));
        this->System->Msg->printValue(this->noOfFailures + 1);
        this->System->Msg->printValue(F("/ "));
        this->System->Msg->printValue(this->configuration->failuresToSwitch);
      }
#endif
    }
  }
}

void ESPAPP_WirelessConnection::connectionEvent(void)
{
  if (ESPAPP_WirelessConnection::eventConnectionEstablished == true)
  {

#ifdef DEBUG
    this->System->Msg->printHeader(2, 1, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_MSG_HEADER_TYPE_DASH);
    this->System->Msg->printBulletPoint(F("Connection established successfully"));
    this->System->Msg->printBulletPoint(F("Setting hostname to: "));
    this->System->Msg->printValue(this->System->configuration->deviceName);
#endif

    if (this->WirelessNetwork.hostname(this->System->configuration->deviceName))
    {
#ifdef DEBUG
      this->System->Msg->printValue(F(" ... Success"));
#endif
    }
#ifdef DEBUG
    else
    {
      this->System->Msg->printValue(F(" ... Error"));
    }
    this->System->Msg->printBulletPoint(F("MAC: "));
    Serial << this->WirelessNetwork.macAddress();
    this->System->Msg->printBulletPoint(F("IP: "));
    Serial << this->WirelessNetwork.localIP();
#ifdef ESP32
    this->System->Msg->printBulletPoint(F("IPv6: "));
    Serial << this->WirelessNetwork.localIPv6();
#endif
    this->System->Msg->printBulletPoint(F("Subnet: "));
    Serial << this->WirelessNetwork.subnetMask();
#ifdef ESP32
    Serial << F("/") << this->WirelessNetwork.subnetCIDR();
#endif
    this->System->Msg->printBulletPoint(F("Gateway: "));
    Serial << this->WirelessNetwork.gatewayIP();
#ifdef ESP32
    this->System->Msg->printBulletPoint(F("Broadcast: "));
    Serial << this->WirelessNetwork.broadcastIP();
#endif
    this->System->Msg->printBulletPoint(F("DNS1: "));
    Serial << this->WirelessNetwork.dnsIP(0);
    this->System->Msg->printBulletPoint(F("DNS2: "));
    Serial << this->WirelessNetwork.dnsIP(1);
#ifdef ESP32
    this->System->Msg->printBulletPoint(F("Network ID: "));
    Serial << this->WirelessNetwork.networkID();
#endif
    this->System->Msg->printBulletPoint(F("RSSI: "));
    Serial << this->WirelessNetwork.RSSI();
#ifdef ESP32
    this->System->Msg->printBulletPoint(F("Hostname: "));
    Serial << this->WirelessNetwork.getHostname();
#else
    this->System->Msg->printBulletPoint(F("Hostname: "));
    Serial << this->WirelessNetwork.hostname();
#endif
    this->System->Msg->printHeader(1, 1, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_MSG_HEADER_TYPE_DASH);
#endif

    if (this->configuration->mDNS)
    {
      // @TODO imlement this
      // char _mDNSDeviceID[AFE_CONFIG_DEVICE_ID_SIZE + 4]; // extended deviceId "afe-1234-5678"
      char _mDNSDeviceID[20]; // extended deviceId "afe-1234-5678"
      //  Data->getDeviceID(_mDNSDeviceID, true);

      //  if (MDNS.begin(_mDNSDeviceID)) {
      if (MDNS.begin("_mDNSDeviceID"))
      {
#ifdef DEBUG
        Serial << endl
               << F("INFO: mDNS: Responder started: ") << _mDNSDeviceID;
#endif

        MDNS.addService("http", "tcp", 80);
#ifdef DEBUG
        Serial << endl
               << F("INFO: mDNS: HTTP service added");
      }
      else
      {
        Serial << endl
               << F("ERROR: mDNS: Responder not set");
#endif
      }
    }

    ESPAPP_WirelessConnection::eventConnectionEstablished = false;
    this->System->Events->triggerEvent(EVENT_NETWORK_CONNECTED);
  }

  if (!ESPAPP_WirelessConnection::eventConnectionLost)
  {
    this->System->Events->triggerEvent(EVENT_NETWORK_DISCONNECTED);
    ESPAPP_WirelessConnection::eventConnectionLost = false;
  }
}

#ifdef ESP32
void ESPAPP_WirelessConnection::onWiFiEvent(WiFiEvent_t event)
{
  switch (event)
  {
  case SYSTEM_EVENT_STA_GOT_IP:
    ESPAPP_WirelessConnection::eventConnectionEstablished = true;
    ESPAPP_WirelessConnection::isConnected = true;
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    ESPAPP_WirelessConnection::eventConnectionLost = true;
    ESPAPP_WirelessConnection::isConnected = false;
    break;
  }
}

#else // ESP8266
void ESPAPP_WirelessConnection::onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  ESPAPP_WirelessConnection::eventConnectionEstablished = true;
  ESPAPP_WirelessConnection::isConnected = true;
}

void ESPAPP_WirelessConnection::onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
#ifdef DEBUG
  Serial << endl
         << F("WARN: WIFI: Disconnected");
#endif
  ESPAPP_WirelessConnection::eventConnectionLost = true;
  ESPAPP_WirelessConnection::isConnected = false;
}

#ifdef DEBUG

void ESPAPP_WirelessConnection::onWiFiAPStationConnected(
    const WiFiEventSoftAPModeStationConnected &event)
{

  Serial << endl
         << F("INFO: HotSpot: New device connected. (Connected: ")
         << WiFi.softAPgetStationNum() << F(" devices)");
}
#endif
#endif

bool ESPAPP_WirelessConnection::readConfiguration(void)
{

  bool success = false;

#ifdef DEBUG
  this->System->Msg->printInformation(F("Reading network configuration"), F("WIFI"));
#endif

  /* Read configuration */
  StaticJsonDocument<1000> doc;
  success = this->System->Flash->getJSON(F("/cfg/network.json"), doc);

  if (success)
  {
    sprintf(this->configuration->primary.ssid, doc["primary"]["ssid"] | "");
    sprintf(this->configuration->primary.password, doc["primary"]["password"] | "");
    this->configuration->primary.isDHCP = doc["primary"]["dhcp"] | true;
    sprintf(this->configuration->primary.ip, doc["primary"]["ip"] | "");
    sprintf(this->configuration->primary.gateway, doc["primary"]["gateway"] | "");
    sprintf(this->configuration->primary.subnet, doc["primary"]["subnet"] | "");
    sprintf(this->configuration->primary.dns1, doc["primary"]["dns1"] | "");
    sprintf(this->configuration->primary.dns2, doc["primary"]["dns2"] | "");

    sprintf(this->configuration->secondary.ssid, doc["secondary"]["ssid"] | "");
    sprintf(this->configuration->secondary.password, doc["secondary"]["password"] | "");
    this->configuration->secondary.isDHCP = doc["secondary"]["dhcp"] | true;
    sprintf(this->configuration->secondary.ip, doc["secondary"]["ip"] | "");
    sprintf(this->configuration->secondary.gateway, doc["secondary"]["gateway"] | "");
    sprintf(this->configuration->secondary.subnet, doc["secondary"]["subnet"] | "");
    sprintf(this->configuration->secondary.dns1, doc["secondary"]["dns1"] | "");
    sprintf(this->configuration->secondary.dns2, doc["secondary"]["dns2"] | "");

    this->configuration->mDNS = doc["mdns"] | true;
    this->configuration->connectionTimeout = doc["connectionTimeout"] | ESPAPP_NETWORK_DEFAULT_CONNECTION_TIMEOUT;
    this->configuration->sleepTimeout = doc["sleepTimeout"] | ESPAPP_NETWORK_DEFAULT_SLEEP_TIMEOUT;
    this->configuration->failuresToSwitch = doc["failuresToSwitch"] | ESPAPP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER;

#ifndef ESP32
    this->configuration->radioMode = doc["m"];
    this->configuration->outputPower = doc["op"];
#endif

#ifdef DEBUG
    this->System->Msg->printBulletPoint(F("Primary SSID: "));
    this->System->Msg->printValue(this->configuration->primary.ssid);
    this->System->Msg->printBulletPoint(F("Primary IP: "));
    this->System->Msg->printValue(this->configuration->primary.ip);
    this->System->Msg->printBulletPoint(F("Primary Gateway: "));
    this->System->Msg->printValue(this->configuration->primary.gateway);
    this->System->Msg->printBulletPoint(F("Primary Subnet: "));
    this->System->Msg->printValue(this->configuration->primary.subnet);
    this->System->Msg->printBulletPoint(F("Primary DNS1: "));
    this->System->Msg->printValue(this->configuration->primary.dns1);
    this->System->Msg->printBulletPoint(F("Primary DNS2: "));
    this->System->Msg->printValue(this->configuration->primary.dns2);
    this->System->Msg->printBulletPoint(F("Primary DHCP: "));
    this->System->Msg->printValue(this->configuration->primary.isDHCP);

    this->System->Msg->printBulletPoint(F("Secondary SSID: "));
    this->System->Msg->printValue(this->configuration->secondary.ssid);
    this->System->Msg->printBulletPoint(F("Secondary IP: "));
    this->System->Msg->printValue(this->configuration->secondary.ip);
    this->System->Msg->printBulletPoint(F("Secondary Gateway: "));
    this->System->Msg->printValue(this->configuration->secondary.gateway);
    this->System->Msg->printBulletPoint(F("Secondary Subnet: "));
    this->System->Msg->printValue(this->configuration->secondary.subnet);
    this->System->Msg->printBulletPoint(F("Secondary DNS1: "));
    this->System->Msg->printValue(this->configuration->secondary.dns1);
    this->System->Msg->printBulletPoint(F("Secondary DNS2: "));
    this->System->Msg->printValue(this->configuration->secondary.dns2);
    this->System->Msg->printBulletPoint(F("Secondary DHCP: "));
    this->System->Msg->printValue(this->configuration->secondary.isDHCP);

    this->System->Msg->printBulletPoint(F("mDNS: "));
    this->System->Msg->printValue(this->configuration->mDNS);
    this->System->Msg->printBulletPoint(F("Connection Timeout: "));
    this->System->Msg->printValue(this->configuration->connectionTimeout);
    this->System->Msg->printBulletPoint(F("Sleep Timeout: "));
    this->System->Msg->printValue(this->configuration->sleepTimeout);
    this->System->Msg->printBulletPoint(F("Failures to switch network: "));
    this->System->Msg->printValue(this->configuration->failuresToSwitch);

#ifndef ESP32
    this->System->Msg->printBulletPoint(F("Radio mode: "));
    this->System->Msg->printValue(this->configuration->radioMode);
    this->System->Msg->printBulletPoint(F("Output power: "));
    this->System->Msg->printValue(this->configuration->outputPower);
#endif
  }
  else
  {
    this->System->Msg->printError(F("Problem with loading network configuration"), F("WIFI"));
#endif // DEBUG
  }

  return success;
}

bool ESPAPP_WirelessConnection::createDefaultConfiguration(void)
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Creating default network configuration"), F("WIFI"));
#endif

  // Set default values for primary network
  sprintf(this->configuration->primary.ssid, "");
  sprintf(this->configuration->primary.password, "");
  this->configuration->primary.isDHCP = ESPAPP_NETWORK_DEFAULT_DHCP;
  sprintf(this->configuration->primary.ip, "");
  sprintf(this->configuration->primary.gateway, "%s", F(ESPAPP_NETWORK_DEFAULT_GATEWAY));
  sprintf(this->configuration->primary.subnet, "%s", F(ESPAPP_NETWORK_DEFAULT_SUBNET));
  sprintf(this->configuration->primary.dns1, "%s", F(ESPAPP_NETWORK_DEFAULT_DNS1));
  sprintf(this->configuration->primary.dns2, "%s", F(ESPAPP_NETWORK_DEFAULT_DNS2));

  // Set default values for secondary network
  sprintf(this->configuration->secondary.ssid, "");
  sprintf(this->configuration->secondary.password, "");
  this->configuration->secondary.isDHCP = ESPAPP_NETWORK_DEFAULT_DHCP;
  sprintf(this->configuration->secondary.ip, "");
  sprintf(this->configuration->secondary.gateway, "%s", F(ESPAPP_NETWORK_DEFAULT_GATEWAY));
  sprintf(this->configuration->secondary.subnet, "%s", F(ESPAPP_NETWORK_DEFAULT_SUBNET));
  sprintf(this->configuration->secondary.dns1, "%s", F(ESPAPP_NETWORK_DEFAULT_DNS1));
  sprintf(this->configuration->secondary.dns2, "%s", F(ESPAPP_NETWORK_DEFAULT_DNS2));

  // Set default values for other network settings
  this->configuration->mDNS = ESPAPP_NETWORK_DEFAULT_MDNS;
  this->configuration->connectionTimeout = ESPAPP_NETWORK_DEFAULT_CONNECTION_TIMEOUT;
  this->configuration->sleepTimeout = ESPAPP_NETWORK_DEFAULT_SLEEP_TIMEOUT;
  this->configuration->failuresToSwitch = ESPAPP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER;

#ifndef ESP32
  this->configuration->radioMode = 3;                                         // WIFI_PHY_MODE_11N by default
  this->configuration->outputPower = ESPAPP_NETWORK_DEFAULT_OUTPUT_POWER_MAX; // Default output power in dBm
#endif

  return saveConfiguration();
}

bool ESPAPP_WirelessConnection::saveConfiguration(void)
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Saving network configuration"), F("WIFI"));
#endif

  StaticJsonDocument<1000> doc;

  // Primary network configuration
  JsonObject primary = doc.createNestedObject("primary");
  primary["ssid"] = this->configuration->primary.ssid;
  primary["password"] = this->configuration->primary.password;
  primary["dhcp"] = this->configuration->primary.isDHCP;
  primary["ip"] = this->configuration->primary.ip;
  primary["gateway"] = this->configuration->primary.gateway;
  primary["subnet"] = this->configuration->primary.subnet;
  primary["dns1"] = this->configuration->primary.dns1;
  primary["dns2"] = this->configuration->primary.dns2;

  // Secondary network configuration
  JsonObject secondary = doc.createNestedObject("secondary");
  secondary["ssid"] = this->configuration->secondary.ssid;
  secondary["password"] = this->configuration->secondary.password;
  secondary["dhcp"] = this->configuration->secondary.isDHCP;
  secondary["ip"] = this->configuration->secondary.ip;
  secondary["gateway"] = this->configuration->secondary.gateway;
  secondary["subnet"] = this->configuration->secondary.subnet;
  secondary["dns1"] = this->configuration->secondary.dns1;
  secondary["dns2"] = this->configuration->secondary.dns2;

  // Other network settings
  doc["mdns"] = this->configuration->mDNS;
  doc["connectionTimeout"] = this->configuration->connectionTimeout;
  doc["sleepTimeout"] = this->configuration->sleepTimeout;
  doc["failuresToSwitch"] = this->configuration->failuresToSwitch;

#ifndef ESP32
  doc["m"] = this->configuration->radioMode;
  doc["op"] = this->configuration->outputPower;
#endif

  return this->System->Flash->saveJSON(F("/cfg/network.json"), doc);
}

uint8_t ESPAPP_WirelessConnection::connection()
{

  if (ESPAPP_WirelessConnection::isConnected)
  {
    return ESPAPP_NETWORK_CONNECTION_MODE_CLIENT;
  } // Check if device is in access point mode
  else if (this->WirelessNetwork.getMode() == WIFI_AP || this->WirelessNetwork.getMode() == WIFI_AP_STA)
  {
    return ESPAPP_NETWORK_CONNECTION_MODE_ACCESS_POINT;
  }
  // Not connected to any network
  else
  {
    return ESPAPP_NETWORK_CONNECTION_MODE_NO_CONNECTION;
  }
}