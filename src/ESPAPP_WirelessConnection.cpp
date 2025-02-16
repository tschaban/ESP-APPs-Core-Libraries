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


  if (ready)
  {
    if (!System->connectionMode() == ESPAPP_NETWORK_CONNECTION_MODE_NO_CONNECTION)
    {
      if (!connected())
      {
        if (sleepMode)
        {
          if (millis() - sleepStartTime >= configuration->waitTimeSeries * 1000)
          {
            sleepMode = false;
          }
        }
        else
        {
          if (delayStartTime == 0)
          {
            delayStartTime = millis();
            if (connections == 0)
            {

              /* Checking if WiFi is configured */
              if (strlen(configuration->primary.ssid) == 0 ||
                  strlen(configuration->primary.password) == 0)
              {
#ifdef DEBUG
                System->Msg->printError(
                    F("is not configured. Going to configuration mode"),
                    F("WIFI"));
#endif
                System->reboot(ESPAPP_NETWORK_CONNECTION_MODE_HOTSPOT);
              }

              if (isPrimaryConfiguration)
              {
                WirelessNetwork.begin(configuration->primary.ssid,
                                      configuration->primary.password);
              }
              else
              {
                WirelessNetwork.begin(configuration->secondary.ssid,
                                      configuration->secondary.password);
              }

#ifdef DEBUG

              System->Msg->printInformation(
                  F("Starting establishing WiFi connection"), F("WIFI"));
              System->Msg->printBulletPoint(F("SSID: "));
              Serial << (isPrimaryConfiguration ? configuration->primary.ssid
                                                : configuration->secondary.ssid);

              System->Msg->printBulletPoint(F("Auto Connected: "));
              Serial << WirelessNetwork.getAutoConnect();

              System->Msg->printBulletPoint(F("Auto ReConnected: "));
              Serial << WirelessNetwork.getAutoReconnect();

              System->Msg->printBulletPoint(F("Mode: "));
              Serial << WirelessNetwork.getMode();

#ifndef ESP32
              System->Msg->printBulletPoint(F("Listen interval: "));
              Serial << WirelessNetwork.getListenInterval();

              System->Msg->printBulletPoint(F("Persistent: "));
              Serial << WirelessNetwork.getPersistent();

              System->Msg->printBulletPoint(F("PhyMode: "));
              Serial << WirelessNetwork.getPhyMode();

              System->Msg->printBulletPoint(F("Sleep Mode: "));
              Serial << WirelessNetwork.getSleepMode();
#endif // !ESP32
#endif
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

          if (millis() >
              delayStartTime + (configuration->waitTimeConnections * 1000))
          {
            connections++;

// yield();
// delay(10);
#ifdef DEBUG
            System->Msg->printBulletPoint(F("Connecting to: "));

            Serial << (isPrimaryConfiguration ? F("primary") : F("backup"))
                   << F(" router. Attempt: ") << connections << F("/")
                   << configuration->noConnectionAttempts << F(", IP(")
                   << WirelessNetwork.localIP() << F(")") << F(" WLStatus=")
                   << WirelessNetwork.status();
            if (isBackupConfigurationSet)
            {
              Serial << F(", Failures counter: ") << noOfFailures + 1 << F("/")
                     << configuration->noFailuresToSwitchNetwork;
            }
#endif
            delayStartTime = 0;
          }

          if (connections == configuration->noConnectionAttempts)
          {
            sleepMode = true;
            WirelessNetwork.disconnect();
            sleepStartTime = millis();
            delayStartTime = 0;

#ifdef AFE_CONFIG_HARDWARE_LED
            ledStartTime = 0;
            Led->off();
#endif

            connections = 0;
#ifdef DEBUG
            System->Msg->printWarning(
                F("Not able to connect.Going to sleep mode for "), F("WIFI"));
            System->Msg->printValue(configuration->waitTimeSeries, F("sec."));
#endif

            /**
             * @brief Switching configurations
             *
             */
            if (isBackupConfigurationSet)
            {
              noOfFailures++;
              if (noOfFailures == configuration->noFailuresToSwitchNetwork)
              {
                switchConfiguration();
              }
            } /* Endif: Switching configurations */
          }
        }
      }
      else
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
          System->Msg->printInformation(F("Setting hostname to: "), F("WIFI"));
          System->Msg->printValue(System->configuration->deviceName);
#endif

          // yield();

          if (WirelessNetwork.hostname(System->configuration->deviceName))
          {
// yield();
#ifdef DEBUG

            System->Msg->printValue(F(" ... Success"));
          }
          else
          {
            System->Msg->printValue(F(" ... Error"));
#endif
          }

#ifdef DEBUG

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

          //   System->Msg->printBulletPoint(F("LAN HTTP: http://"));
          //    char deviceIdExtended[AFE_CONFIG_DEVICE_ID_SIZE];
          //    Data->getDeviceID(deviceIdExtended, true);
          //     Serial << deviceIdExtended << F(".local");

          System->Msg->printHeader(1, 1, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_MSG_HEADER_TYPE_DASH);
#endif
        }
      }
    }
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

  if (success)
  {
    sprintf(configuration->primary.ssid, doc["p"]["s"]);
    sprintf(configuration->primary.password, doc["p"]["p"]);
    configuration->primary.isDHCP = doc["p"]["d"];
    sprintf(configuration->primary.ip, doc["p"]["i"]);
    sprintf(configuration->primary.gateway, doc["p"]["g"]);
    sprintf(configuration->primary.subnet, doc["p"]["b"]);
    sprintf(configuration->primary.dns1, doc["p"]["d1"]);
    sprintf(configuration->primary.dns2, doc["p"]["d2"]);

    sprintf(configuration->secondary.ssid, doc["s"]["s"]);
    sprintf(configuration->secondary.password, doc["s"]["p"]);
    configuration->secondary.isDHCP = doc["s"]["d"];
    sprintf(configuration->secondary.ip, doc["s"]["i"]);
    sprintf(configuration->secondary.gateway, doc["s"]["g"]);
    sprintf(configuration->secondary.subnet, doc["s"]["b"]);
    sprintf(configuration->secondary.dns1, doc["s"]["d1"]);
    sprintf(configuration->secondary.dns2, doc["s"]["d2"]);

    configuration->mDNS = doc["m"];
    configuration->noConnectionAttempts = doc["n"];
    configuration->waitTimeConnections = doc["w"];
    configuration->waitTimeSeries = doc["ws"];
    configuration->noFailuresToSwitchNetwork = doc["nf"];

#ifndef ESP32
    configuration->radioMode = doc["m"];
    configuration->outputPower = doc["op"];
#endif

#ifdef DEBUG
    System->Msg->printInformation(F("Network configuration loaded"), F("WIFI"));
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
    System->Msg->printBulletPoint(F("No of connection attempts: "));
    System->Msg->printValue(configuration->noConnectionAttempts);
    System->Msg->printBulletPoint(F("Wait time between connection attempts: "));
    System->Msg->printValue(configuration->waitTimeConnections);
    System->Msg->printBulletPoint(F("Wait time between connection series: "));
    System->Msg->printValue(configuration->waitTimeSeries);
    System->Msg->printBulletPoint(F("No of failures to switch network: "));
    System->Msg->printValue(configuration->noFailuresToSwitchNetwork);

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
