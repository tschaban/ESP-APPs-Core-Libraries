/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#include "ESPAPP_WirelessConnection.h"

boolean ESPAPP_WirelessConnection::eventConnectionEstablished = false;
boolean ESPAPP_WirelessConnection::isConnected = false;
boolean ESPAPP_WirelessConnection::eventConnectionLost = true;

ESPAPP_WirelessConnection::ESPAPP_WirelessConnection(ESPAPP_Core *_System)
{
  System = _System;
}

bool ESPAPP_WirelessConnection::init(void)
{

#ifdef DEBUG
  System->Msg->printInformation(F("Starting network"), F("WIFI"));
#endif

  ready = readConfiguration();

  if (ready)
  {

// Setting handlers for WiFi events
#ifdef ESP32
    WirelessNetwork.onEvent(ESPAPP_WirelessConnection::onWiFiEvent);
#else
    wifiConnectHandler =
        WirelessNetwork.onStationModeGotIP(ESPAPP_WirelessConnection::onWifiConnect);
    wifiDisconnectHandler =
        WirelessNetwork.onStationModeDisconnected(ESPAPP_WirelessConnection::onWifiDisconnect);
#ifdef DEBUG
    wifiAPStationConnectedHandler = WirelessNetwork.onSoftAPModeStationConnected(
        ESPAPP_WirelessConnection::onWiFiAPStationConnected);
#endif
#endif

#ifdef DEBUG
    System->Msg->printInformation(F("Secondary WiFi configuration exist: "), F("WIFI"));
#endif

    if (strlen(configuration->secondary.ssid) > 0 &&
        strcmp(configuration->secondary.ssid,
               ESPAPP_NETWORK_DEFAULT_NONE_SSID) != 0 &&
        strlen(configuration->secondary.password) > 0)
    {
      isBackupConfigurationSet = true;
    }

#ifdef DEBUG
    System->Msg->printValue(isBackupConfigurationSet);
#endif

#if !defined(ESP32)
    if (configuration->radioMode != AFE_NONE)
    {
      // wifi_set_phy_mode(configuration->radioMode);
      WirelessNetwork.setPhyMode(configuration->radioMode == 1
                                     ? WIFI_PHY_MODE_11B
                                 : configuration->radioMode == 2
                                     ? WIFI_PHY_MODE_11G
                                     : WIFI_PHY_MODE_11N);
#ifdef DEBUG
      System->Msg->printInformation(F("Setting Radio mode (1:B 2:G 3:N) to: "),
                                    F("WIFI"));
      System->Msg->printValue(configuration->radioMode);
#endif
    }

    if (configuration->outputPower != AFE_NONE &&
        configuration->outputPower >=
            AFE_CONFIG_NETWORK_DEFAULT_OUTPUT_POWER_MIN &&
        configuration->outputPower <=
            AFE_CONFIG_NETWORK_DEFAULT_OUTPUT_POWER_MAX)
    {
      WirelessNetwork.setOutputPower(configuration->outputPower);
#ifdef DEBUG
      System->Msg->printInformation(F("Setting TX Output power to: "), F("WIFI"));
      System->Msg->printValue(configuration->outputPower, F("dBm"));
#endif
    }

#endif
    if (System->connectionMode() == ESPAPP_NETWORK_CONNECTION_MODE_HOTSPOT)
    {
#ifdef DEBUG
      System->Msg->printInformation(F("Starting Hotspot..."), F("WIFI"));
#endif

      WirelessNetwork.mode(WIFI_AP_STA);

      ready = WirelessNetwork.softAPConfig(IPAddress(192, 168, 5, 1),
                                           IPAddress(192, 168, 5, 200),
                                           IPAddress(255, 255, 255, 0));

      if (ready)
      {
        WirelessNetwork.softAP(System->configuration->deviceName);
#ifdef DEBUG
        System->Msg->printValue(F("Ready"));
      }
      else
      {
        System->Msg->printValue(F("Failed"));
#endif
      }
    }
    else
    {
      /* Add additional configuration parameters */
      switchConfiguration();
    }
#ifdef DEBUG
    System->Msg->printInformation(F("Network initialized..."), F("BOOT"));
#endif
  }
#ifdef DEBUG
  else
  {
    System->Msg->printError(F("Network cannot be initialized"), F("WIFI"));
  }
#endif

  return ready;
}

void ESPAPP_WirelessConnection::switchConfiguration()
{
  isPrimaryConfiguration = isPrimaryConfiguration ? false : true;
  noOfFailures = 0;
  WirelessNetwork.persistent(false);
  WirelessNetwork.disconnect(true);

#ifndef ESP32
  WirelessNetwork.setSleepMode(WIFI_NONE_SLEEP);
#else
  WiFi.setSleep(false);
#endif

  /**
   * @brief Setting Fixed IP for Primary Configuration if set
   *
   */
  if (isPrimaryConfiguration && !configuration->primary.isDHCP)
  {

    IPAddress dns1;
    if (!dns1.fromString(configuration->primary.dns1))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with DNS1: "), F("WIFI"));
      System->Msg->printValue(configuration->primary.dns1);
#endif
    }
    IPAddress dns2;
    if (!dns2.fromString(configuration->primary.dns2))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with DNS2: "), F("WIFI"));
      System->Msg->printValue(configuration->primary.dns2);
#endif
    }

    IPAddress ip;
    if (!ip.fromString(configuration->primary.ip))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with WIFI IP: "), F("WIFI"));
      System->Msg->printValue(configuration->primary.ip);
#endif
    }
    IPAddress gateway;
    if (!gateway.fromString(configuration->primary.gateway))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with WIFI gateway: "), F("WIFI"));
      System->Msg->printValue(configuration->primary.gateway);
#endif
    }
    IPAddress subnet;
    if (!subnet.fromString(configuration->primary.subnet))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with WIFI subnet: "), F("WIFI"));
      System->Msg->printValue(configuration->primary.subnet);
#endif
    }

    WirelessNetwork.config(ip, gateway, subnet, dns1, dns2);
#ifdef DEBUG
    System->Msg->printInformation(F("Fixed IP set"), F("WIFI"));
#endif
  }
  /**
   * @brief set IPs to 0 for configuration over DHCP
   *
   */

  else if ((isPrimaryConfiguration && configuration->primary.isDHCP) ||
           (!isPrimaryConfiguration && configuration->secondary.isDHCP))
  {
    WirelessNetwork.config((uint32_t)0x00000000, (uint32_t)0x00000000,
                           (uint32_t)0x00000000);
  }

  /**
   * @brief Setting fixed IP for backup WiFi configurations
   *
   */
  else if (!isPrimaryConfiguration && !configuration->secondary.isDHCP)
  {
#ifdef DEBUG
    System->Msg->printInformation(F("Setting fixed IP ("), F("WIFI"));
    System->Msg->printValue(configuration->secondary.ip);
    System->Msg->printValue(F(") address for backup WiFi configuration"));
#endif

    IPAddress dns1;
    if (!dns1.fromString(configuration->secondary.dns1))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with WIFI DNS1: "), F("WIFI"));
      System->Msg->printValue(configuration->secondary.dns1);
#endif
    }
    IPAddress dns2;
    if (!dns2.fromString(configuration->secondary.dns2))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with WIFI DNS2: "), F("WIFI"));
      System->Msg->printValue(configuration->secondary.dns2);
#endif
    }

    IPAddress ip;
    if (!ip.fromString(configuration->secondary.ip))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with WIFI IP: "), F("WIFI"));
      System->Msg->printValue(configuration->secondary.ip);
#endif
    }
    IPAddress gateway;
    if (!gateway.fromString(configuration->secondary.gateway))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with WIFI gateway: "), F("WIFI"));
      System->Msg->printValue(configuration->secondary.gateway);
#endif
    }
    IPAddress subnet;
    if (!subnet.fromString(configuration->secondary.subnet))
    {
#ifdef DEBUG
      System->Msg->printError(F("Problem with WIFI subnet: "), F("WIFI"));
      System->Msg->printValue(configuration->secondary.subnet);
#endif
    }

    WirelessNetwork.config(ip, gateway, subnet, dns1, dns2);
#ifdef DEBUG
    System->Msg->printInformation(F("Fixed IP set"), F("WIFI"));
#endif
  } /* Endif: Setting Fixed IP for Primary Configuration if set */

  WirelessNetwork.mode(WIFI_STA);

#ifdef DEBUG
  System->Msg->printInformation(
      (isPrimaryConfiguration ? F("Primary") : F("Backup")), F("WIFI"));
  System->Msg->printValue(F(" configuration set"));
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
  if (System->connectionMode() == ESPAPP_NETWORK_CONNECTION_MODE_NO_CONNECTION)
  {
    return;
  }

  // Connection established
  if (connected())
  {
    if (connections > 0)
    {
      connections = 0;
      noOfFailures = 0;
      delayStartTime = 0;

#ifdef AFE_CONFIG_HARDWARE_LED
      ledStartTime = 0;
      Led->off();
#endif

#ifdef DEBUG
      System->Msg->printBulletPoint(F("Connection established successfully"));
      System->Msg->printBulletPoint(F("Setting hostname to: "));
      System->Msg->printValue(System->configuration->deviceName);
#endif

      if (WirelessNetwork.hostname(System->configuration->deviceName))
      {
#ifdef DEBUG
        System->Msg->printValue(F(" ... Success"));
#endif
      }
#ifdef DEBUG
      else
      {
        System->Msg->printValue(F(" ... Error"));
      }

      System->Msg->printHeader(2, 1, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_MSG_HEADER_TYPE_DASH);
      System->Msg->printValue(F("Connection established"));
      System->Msg->printBulletPoint(F("MAC: "));
      Serial << WirelessNetwork.macAddress();
      System->Msg->printBulletPoint(F("IP: "));
      Serial << WirelessNetwork.localIP();
#ifdef ESP32
      System->Msg->printBulletPoint(F("IPv6: "));
      Serial << WirelessNetwork.localIPv6();
#endif
      System->Msg->printBulletPoint(F("Subnet: "));
      Serial << WirelessNetwork.subnetMask();
#ifdef ESP32
      Serial << F("/") << WirelessNetwork.subnetCIDR();
#endif
      System->Msg->printBulletPoint(F("Gateway: "));
      Serial << WirelessNetwork.gatewayIP();
#ifdef ESP32
      System->Msg->printBulletPoint(F("Broadcast: "));
      Serial << WirelessNetwork.broadcastIP();
#endif
      System->Msg->printBulletPoint(F("DNS1: "));
      Serial << WirelessNetwork.dnsIP(0);
      System->Msg->printBulletPoint(F("DNS2: "));
      Serial << WirelessNetwork.dnsIP(1);
#ifdef ESP32
      System->Msg->printBulletPoint(F("Network ID: "));
      Serial << WirelessNetwork.networkID();
#endif
      System->Msg->printBulletPoint(F("RSSI: "));
      Serial << WirelessNetwork.RSSI();
#ifdef ESP32
      System->Msg->printBulletPoint(F("Hostname: "));
      Serial << WirelessNetwork.getHostname();
#else
      System->Msg->printBulletPoint(F("Hostname: "));
      Serial << WirelessNetwork.hostname();
#endif
      System->Msg->printHeader(1, 1, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_MSG_HEADER_TYPE_DASH);
#endif
    }
    return;
  }

  // Sleep mode
  if (sleepMode)
  {
    if (millis() - sleepStartTime >= configuration->sleepTimeout * 1000)
    {
      sleepMode = false;
#ifdef DEBUG
      System->Msg->printBulletPoint(F("Sleep timeout completed, resuming connection attempts"));
#endif
    }
    return;
  }

  // Starting connection
  if (delayStartTime == 0)
  {
    delayStartTime = millis();
    scecondsTimer = delayStartTime;
    if (connections == 0)
    {
      if (strlen(configuration->primary.ssid) == 0 || strlen(configuration->primary.password) == 0)
      {
#ifdef DEBUG
        System->Msg->printError(F("WiFi not configured, switching to Hotspot mode"), F("WIFI"));
#endif
        System->reboot(ESPAPP_NETWORK_CONNECTION_MODE_HOTSPOT);
        return;
      }

      if (isPrimaryConfiguration)
      {
        WirelessNetwork.begin(configuration->primary.ssid, configuration->primary.password);
#ifdef DEBUG
        System->Msg->printBulletPoint(F("Attempting connection to primary network"));
        System->Msg->printBulletPoint(F("SSID: "));
        System->Msg->printValue(configuration->primary.ssid);
#endif
      }
      else
      {
        WirelessNetwork.begin(configuration->secondary.ssid, configuration->secondary.password);
#ifdef DEBUG
        System->Msg->printBulletPoint(F("Attempting connection to secondary network"));
        System->Msg->printBulletPoint(F("SSID: "));
        System->Msg->printValue(configuration->secondary.ssid);
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

  // Setting timeout for connection
  if (millis() > delayStartTime + (configuration->connectionTimeout * 1000))
  {

    delayStartTime = 0;
    sleepMode = true;
    WirelessNetwork.disconnect();
    sleepStartTime = millis();

#ifdef AFE_CONFIG_HARDWARE_LED
    ledStartTime = 0;
    Led->off();
#endif

    connections = 0;

#ifdef DEBUG
    System->Msg->printBulletPoint(F("Connection failed. Entering sleep mode for "));
    System->Msg->printValue(configuration->sleepTimeout, F(" seconds"));
#endif

    if (isBackupConfigurationSet)
    {
      noOfFailures++;

      if (noOfFailures >= configuration->failuresToSwitch)
      {
#ifdef DEBUG
        System->Msg->printBulletPoint(F("Switching network configuration to "));
        System->Msg->printValue(isPrimaryConfiguration ? F("secondary") : F("primary"));
#endif
        switchConfiguration();
      }
    }
  }

  // Displaying connection status and counting connection attempts
  if (millis() > scecondsTimer + 1000)
  {
    scecondsTimer = millis();
    connections++;
#ifdef DEBUG
    System->Msg->printBulletPoint(F("Establishing connection: "));
    System->Msg->printValue(connections);
    System->Msg->printValue(F("s, Status="));
    System->Msg->printValue((int)WirelessNetwork.status());
    if (isBackupConfigurationSet)
    {
      System->Msg->printValue(F("Failures: "));
      System->Msg->printValue(noOfFailures + 1);
      System->Msg->printValue(F("/ "));
      System->Msg->printValue(configuration->failuresToSwitch);
    }
#endif
  }
}

boolean ESPAPP_WirelessConnection::connected()
{
  if (configuration->mDNS)
  {

#ifndef ESP32
    MDNS.update();
#else
// @TODO 3.8.0 what is ESP32 equivalent ESP32
#endif
  }

  return ESPAPP_WirelessConnection::isConnected;
}

boolean ESPAPP_WirelessConnection::eventConnected()
{
  boolean returnValue = ESPAPP_WirelessConnection::eventConnectionEstablished;

  if (ESPAPP_WirelessConnection::eventConnectionEstablished == true)
  {
    if (configuration->mDNS)
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
    //  Data->addLog(F("wifi:connected"));
    ESPAPP_WirelessConnection::eventConnectionEstablished = false;
  }

  return returnValue;
}

boolean ESPAPP_WirelessConnection::eventDisconnected()
{
  boolean returnValue = ESPAPP_WirelessConnection::eventConnectionLost;

  if (returnValue)
  {
    // Data->addLog(F("wifi:disconnected"));
  }

  ESPAPP_WirelessConnection::eventConnectionLost = false;

  return returnValue;
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
  /* Read configuration */
  StaticJsonDocument<1000> doc;
  // JsonDocument doc;
  success = System->Flash->getJSON(F("/cfg/network.json"), doc);

#ifdef DEBUG
  System->Msg->printBulletPoint(F("Getting data from JSON"));
#endif

  if (success)
  {
    sprintf(configuration->primary.ssid, doc["primary"]["ssid"] | "");
    sprintf(configuration->primary.password, doc["primary"]["password"] | "");
    configuration->primary.isDHCP = doc["primary"]["dhcp"] | true;
    sprintf(configuration->primary.ip, doc["primary"]["ip"] | "");
    sprintf(configuration->primary.gateway, doc["primary"]["gateway"] | "");
    sprintf(configuration->primary.subnet, doc["primary"]["subnet"] | "");
    sprintf(configuration->primary.dns1, doc["primary"]["dns1"] | "");
    sprintf(configuration->primary.dns2, doc["primary"]["dns2"] | "");

    sprintf(configuration->secondary.ssid, doc["secondary"]["ssid"] | "");
    sprintf(configuration->secondary.password, doc["secondary"]["password"] | "");
    configuration->secondary.isDHCP = doc["secondary"]["dhcp"] | true;
    sprintf(configuration->secondary.ip, doc["secondary"]["ip"] | "");
    sprintf(configuration->secondary.gateway, doc["secondary"]["gateway"] | "");
    sprintf(configuration->secondary.subnet, doc["secondary"]["subnet"] | "");
    sprintf(configuration->secondary.dns1, doc["secondary"]["dns1"] | "");
    sprintf(configuration->secondary.dns2, doc["secondary"]["dns2"] | "");

    configuration->mDNS = doc["mdns"] | true;
    configuration->connectionTimeout = doc["connectionTimeout"] | ESPAPP_NETWORK_DEFAULT_CONNECTION_TIMEOUT;
    configuration->sleepTimeout = doc["sleepTimeout"] | ESPAPP_NETWORK_DEFAULT_SLEEP_TIMEOUT;
    configuration->failuresToSwitch = doc["failuresToSwitch"] | ESPAPP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER;

#ifndef ESP32
    configuration->radioMode = doc["m"];
    configuration->outputPower = doc["op"];
#endif

#ifdef DEBUG
    System->Msg->printBulletPoint(F("Primary SSID: "));
    System->Msg->printValue(configuration->primary.ssid);
    System->Msg->printBulletPoint(F("Primary IP: "));
    System->Msg->printValue(configuration->primary.ip);
    System->Msg->printBulletPoint(F("Primary Gateway: "));
    System->Msg->printValue(configuration->primary.gateway);
    System->Msg->printBulletPoint(F("Primary Subnet: "));
    System->Msg->printValue(configuration->primary.subnet);
    System->Msg->printBulletPoint(F("Primary DNS1: "));
    System->Msg->printValue(configuration->primary.dns1);
    System->Msg->printBulletPoint(F("Primary DNS2: "));
    System->Msg->printValue(configuration->primary.dns2);
    System->Msg->printBulletPoint(F("Primary DHCP: "));
    System->Msg->printValue(configuration->primary.isDHCP);

    System->Msg->printBulletPoint(F("Secondary SSID: "));
    System->Msg->printValue(configuration->secondary.ssid);
    System->Msg->printBulletPoint(F("Secondary IP: "));
    System->Msg->printValue(configuration->secondary.ip);
    System->Msg->printBulletPoint(F("Secondary Gateway: "));
    System->Msg->printValue(configuration->secondary.gateway);
    System->Msg->printBulletPoint(F("Secondary Subnet: "));
    System->Msg->printValue(configuration->secondary.subnet);
    System->Msg->printBulletPoint(F("Secondary DNS1: "));
    System->Msg->printValue(configuration->secondary.dns1);
    System->Msg->printBulletPoint(F("Secondary DNS2: "));
    System->Msg->printValue(configuration->secondary.dns2);
    System->Msg->printBulletPoint(F("Secondary DHCP: "));
    System->Msg->printValue(configuration->secondary.isDHCP);

    System->Msg->printBulletPoint(F("mDNS: "));
    System->Msg->printValue(configuration->mDNS);
    System->Msg->printBulletPoint(F("Connection Timeout: "));
    System->Msg->printValue(configuration->connectionTimeout);
    System->Msg->printBulletPoint(F("Sleep Timeout: "));
    System->Msg->printValue(configuration->sleepTimeout);
    System->Msg->printBulletPoint(F("Failures to switch network: "));
    System->Msg->printValue(configuration->failuresToSwitch);

#ifndef ESP32
    System->Msg->printBulletPoint(F("Radio mode: "));
    System->Msg->printValue(configuration->radioMode);
    System->Msg->printBulletPoint(F("Output power: "));
    System->Msg->printValue(configuration->outputPower);
#endif
  }
  else
  {
    System->Msg->printError(F("Problem with loading network configuration"), F("WIFI"));
#endif // DEBUG
  }

  return success;
}
