/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#include "ESPAPP_WirelessConnection.h"

boolean ESPAPP_WirelessConnection::isConnected = false;
boolean ESPAPP_WirelessConnection::eventConnectionEstablished = false;
boolean ESPAPP_WirelessConnection::eventConnectionLost = true;

ESPAPP_WirelessConnection::ESPAPP_WirelessConnection(ESPAPP_Core *_System)
{
  this->System = _System;
}

bool ESPAPP_WirelessConnection::init(void)
{

#ifdef DEBUG
  this->System->Debugger->printInformation(F("Starting network"), F("WIFI"));
#endif

  /** Read network configuration  */
  this->ready = this->System->File->read(configuration);

  /** Checking if Connection Mode is set to Client and the Primary Wireless network confiiguration is set */
  if (this->ready)
  {
    if (this->System->getConnectionMode() == ESPAPP_NETWORK_CONNECTION_MODE::CLIENT && (strlen(this->configuration->primary->ssid) == 0 ||
                                                                                        (strlen(this->configuration->primary->ssid) > 0 &&
                                                                                         strcmp(this->configuration->primary->ssid,
                                                                                                ESPAPP_NETWORK_DEFAULT_NONE_SSID) == 0) ||
                                                                                        strlen(this->configuration->primary->password) == 0))
    {

#ifdef DEBUG
      this->System->Debugger->printError(F("WiFi not configured, switching to Hotspot mode"), F("WIFI"));
#endif
      this->System->setConnectionMode(ESPAPP_NETWORK_CONNECTION_MODE::ACCESS_POINT);
      this->System->Events->triggerEvent(EVENT_REBOOT);

      this->ready = false;
    }
  }

  /** Setting: Global parameters */
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
    this->System->Debugger->printInformation(F("Secondary WiFi configuration exist: "), F("WIFI"));
#endif

    if (strlen(this->configuration->secondary->ssid) > 0 &&
        strcmp(this->configuration->secondary->ssid,
               ESPAPP_NETWORK_DEFAULT_NONE_SSID) != 0 &&
        strlen(this->configuration->secondary->password) > 0)
    {
      this->isBackupConfigurationSet = true;
    }

#ifdef DEBUG
    this->System->Debugger->printValue(this->isBackupConfigurationSet);
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
      this->System->Debugger->printInformation(F("Setting Radio mode (1:B 2:G 3:N) to: "),
                                          F("WIFI"));
      this->System->Debugger->printValue(this->configuration->radioMode);
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
      this->System->Debugger->printInformation(F("Setting TX Output power to: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->outputPower, F("dBm"));
#endif
    }

#endif

    if (this->System->getConnectionMode() == ESPAPP_NETWORK_CONNECTION_MODE::ACCESS_POINT)
    {
#ifdef DEBUG
      this->System->Debugger->printInformation(F("Starting Hotspot..."), F("WIFI"));
#endif

      this->WirelessNetwork.mode(WIFI_AP_STA);

      this->ready = this->WirelessNetwork.softAPConfig(IPAddress(192, 168, 5, 1),
                                                       IPAddress(192, 168, 5, 200),
                                                       IPAddress(255, 255, 255, 0));

      if (this->ready)
      {
        this->WirelessNetwork.softAP(this->System->configuration->deviceName);
#ifdef DEBUG
        this->System->Debugger->printValue(F("Ready"));
      }
      else
      {
        this->System->Debugger->printValue(F("Failed"));
#endif
      }
    }
    else
    {
      /* Add additional configuration parameters */
      this->switchConfiguration();
    }
#ifdef DEBUG
    this->System->Debugger->printInformation(F("Network initialized..."), F("BOOT"));
#endif
  }
#ifdef DEBUG
  else
  {
    this->System->Debugger->printError(F("Network cannot be initialized"), F("WIFI"));
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
  if (this->isPrimaryConfiguration && !this->configuration->primary->isDHCP)
  {

    IPAddress dns1;
    if (!dns1.fromString(this->configuration->primary->dns1))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with DNS1: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->primary->dns1);
#endif
    }
    IPAddress dns2;
    if (!dns2.fromString(this->configuration->primary->dns2))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with DNS2: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->primary->dns2);
#endif
    }

    IPAddress ip;
    if (!ip.fromString(this->configuration->primary->ip))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with WIFI IP: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->primary->ip);
#endif
    }
    IPAddress gateway;
    if (!gateway.fromString(this->configuration->primary->gateway))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with WIFI gateway: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->primary->gateway);
#endif
    }
    IPAddress subnet;
    if (!subnet.fromString(this->configuration->primary->subnet))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with WIFI subnet: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->primary->subnet);
#endif
    }

    // @TODO chyba trzeba sprawdzic DNSjak powyzsze

    this->WirelessNetwork.config(ip, gateway, subnet, dns1, dns2);
#ifdef DEBUG
    this->System->Debugger->printInformation(F("Fixed IP set"), F("WIFI"));
#endif
  }
  /**
   * @brief set IPs to 0 for configuration over DHCP
   *
   */

  else if ((this->isPrimaryConfiguration && this->configuration->primary->isDHCP) ||
           (!this->isPrimaryConfiguration && this->configuration->secondary->isDHCP))
  {
    this->WirelessNetwork.config((uint32_t)0x00000000, (uint32_t)0x00000000, (uint32_t)0x00000000);
  }

  /**
   * @brief Setting fixed IP for backup WiFi configurations
   *
   */
  else if (!this->isPrimaryConfiguration && !this->configuration->secondary->isDHCP)
  {
#ifdef DEBUG
    this->System->Debugger->printInformation(F("Setting fixed IP ("), F("WIFI"));
    this->System->Debugger->printValue(this->configuration->secondary->ip);
    this->System->Debugger->printValue(F(") address for backup WiFi configuration"));
#endif

    IPAddress dns1;
    if (!dns1.fromString(this->configuration->secondary->dns1))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with WIFI DNS1: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->secondary->dns1);
#endif
    }
    IPAddress dns2;
    if (!dns2.fromString(this->configuration->secondary->dns2))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with WIFI DNS2: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->secondary->dns2);
#endif
    }

    IPAddress ip;
    if (!ip.fromString(this->configuration->secondary->ip))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with WIFI IP: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->secondary->ip);
#endif
    }
    IPAddress gateway;
    if (!gateway.fromString(this->configuration->secondary->gateway))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with WIFI gateway: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->secondary->gateway);
#endif
    }
    IPAddress subnet;
    if (!subnet.fromString(this->configuration->secondary->subnet))
    {
#ifdef DEBUG
      this->System->Debugger->printError(F("Problem with WIFI subnet: "), F("WIFI"));
      this->System->Debugger->printValue(this->configuration->secondary->subnet);
#endif
    }

    this->WirelessNetwork.config(ip, gateway, subnet, dns1, dns2);
#ifdef DEBUG
    this->System->Debugger->printInformation(F("Fixed IP set"), F("WIFI"));
#endif
  } /* Endif: Setting Fixed IP for Primary Configuration if set */

  this->WirelessNetwork.mode(WIFI_STA);
  this->System->Events->triggerEvent(EVENT_WIFI_CONFIG_CHANGED);

#ifdef DEBUG
  this->System->Debugger->printInformation(
      (this->isPrimaryConfiguration ? F("Primary") : F("Backup")), F("WIFI"));
  this->System->Debugger->printValue(F(" configuration set"));
#endif
}

void ESPAPP_WirelessConnection::listener()
{
  // If network configuration is not set then exit
  if (!ready || this->System->getConnectionMode() == ESPAPP_NETWORK_CONNECTION_MODE::OFFLINE)
  {
    return;
  }

  /** Trigger connected event */
  this->connectionEvent();

  /** Working mode: executes network related services */
  if (this->connection() == ESPAPP_NETWORK_CONNECTION_MODE::CLIENT)
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

    return;
  }

  /** Network connection: Establishing network connecton */
  if (this->connection() == ESPAPP_NETWORK_CONNECTION_MODE::OFFLINE)
  {
    /** Sleep Mode
     *  1. If sleep mode is set then check if timeout is reached
     *  2. If timeout is reached then resume connection attempts
     *  3. If timeout is not reached then exit
     */
    if (this->sleepMode)
    {
      if (millis() - this->sleepStartTime >= this->configuration->sleepTimeout * 1000)
      {
        this->sleepMode = false;
#ifdef DEBUG
        this->System->Debugger->printBulletPoint(F("Sleep timeout completed, resuming connection attempts"));
#endif
      }
      return;
    }

    /** Starting connection: Starting or restarting after sleep period
     *  1. Resetting timeout for connection attempts
     *  2. Starting secondsTimer
     *  3. Starting connection to primary or secondary network
     */
    if (this->delayStartTime == 0)
    {
      this->delayStartTime = millis();
      this->secondsTimer = this->delayStartTime;
      if (this->isPrimaryConfiguration)
      {
        this->WirelessNetwork.begin(this->configuration->primary->ssid, this->configuration->primary->password);
#ifdef DEBUG
        this->System->Debugger->printBulletPoint(F("Attempting connection to primary network"));
        this->System->Debugger->printBulletPoint(F("SSID: "));
        this->System->Debugger->printValue(this->configuration->primary->ssid);
#endif
      }
      else
      {
        this->WirelessNetwork.begin(this->configuration->secondary->ssid, this->configuration->secondary->password);
#ifdef DEBUG
        this->System->Debugger->printBulletPoint(F("Attempting connection to secondary network"));
        this->System->Debugger->printBulletPoint(F("SSID: "));
        this->System->Debugger->printValue(this->configuration->secondary->ssid);
#endif
      }
    }

    /** Timeout: after exceededing connection time
     *    1. Disconnect from network
     *    2. Enter sleep mode
     *    3. If backup configuration if set
     *       a. Increase number of failures
     *       b. If number of failures is greater than defined in configuration switch to backup configuration
     */
    if (millis() > this->delayStartTime + (this->configuration->connectionTimeout * 1000))
    {

      this->delayStartTime = 0;
      this->secondsCounter = 0;
      this->sleepMode = true;
      this->WirelessNetwork.disconnect();
      this->sleepStartTime = millis();

#ifdef DEBUG
      this->System->Debugger->printBulletPoint(F("Connection failed. Entering sleep mode for "));
      this->System->Debugger->printValue(this->configuration->sleepTimeout, F(" seconds"));
#endif

      if (this->isBackupConfigurationSet)
      {
        this->noOfFailures++;

        if (this->noOfFailures >= this->configuration->failuresToSwitch)
        {
#ifdef DEBUG
          this->System->Debugger->printBulletPoint(F("Switching network configuration to "));
          this->System->Debugger->printValue(this->isPrimaryConfiguration ? F("secondary") : F("primary"));
#endif
          this->switchConfiguration();
        }
      }
    }

    /** Seconds counter while establishing connection
     *  Debugging informaton is printed every second
     */
    if (millis() > this->secondsTimer + 1000)
    {
      // Serial << "MILLES: " << millis();
      // Serial << "   SECONDS: " << this->secondsTimer << endl;

      this->secondsTimer = millis();
      this->secondsCounter++;
#ifdef DEBUG
      this->System->Debugger->printBulletPoint(F("Establishing connection: "));
      this->System->Debugger->printValue(this->secondsCounter);
      this->System->Debugger->printValue(F("s, Status="));
      this->System->Debugger->printValue((int)this->WirelessNetwork.status());
      if (this->isBackupConfigurationSet)
      {
        this->System->Debugger->printValue(F("Failures: "));
        this->System->Debugger->printValue(this->noOfFailures + 1);
        this->System->Debugger->printValue(F("/ "));
        this->System->Debugger->printValue(this->configuration->failuresToSwitch);
      }
#endif
    }
  }
}

void ESPAPP_WirelessConnection::connectionEvent(void)
{
  if (ESPAPP_WirelessConnection::eventConnectionEstablished == true)
  {

    this->secondsCounter = 0;
    this->noOfFailures = 0;
    this->delayStartTime = 0;
    this->secondsTimer = 0;
    this->sleepMode = false;

    /**
     @TODO Workaround for DNS issue
    IPAddress dns1(8, 8, 8, 8);
    IPAddress dns2(8, 8, 4, 4);

#ifdef DEBUG
    this->System->Debugger->printWarning(F("Setting Fixed DNS"), F("WIFI"));
#endif

    this->WirelessNetwork.config(this->WirelessNetwork.localIP(), this->WirelessNetwork.gatewayIP(), this->WirelessNetwork.subnetMask(), dns1, dns2);
      */

#ifdef DEBUG
    this->System->Debugger->printHeader(2, 0, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
    this->System->Debugger->printBulletPoint(F("Connection established successfully"));
    this->System->Debugger->printBulletPoint(F("Setting hostname to: "));
    this->System->Debugger->printValue(this->System->configuration->deviceName);
#endif

    if (this->WirelessNetwork.hostname(this->System->configuration->deviceName))
    {
#ifdef DEBUG
      this->System->Debugger->printValue(F(" ... Success"));
#endif
    }
#ifdef DEBUG
    else
    {
      this->System->Debugger->printValue(F(" ... Error"));
    }
    this->System->Debugger->printBulletPoint(F("MAC: "));
    Serial << this->WirelessNetwork.macAddress();
    this->System->Debugger->printBulletPoint(F("IP: "));
    Serial << this->WirelessNetwork.localIP();
#ifdef ESP32
    this->System->Debugger->printBulletPoint(F("IPv6: "));
    Serial << this->WirelessNetwork.localIPv6();
#endif
    this->System->Debugger->printBulletPoint(F("Subnet: "));
    Serial << this->WirelessNetwork.subnetMask();
#ifdef ESP32
    Serial << F("/") << this->WirelessNetwork.subnetCIDR();
#endif
    this->System->Debugger->printBulletPoint(F("Gateway: "));
    Serial << this->WirelessNetwork.gatewayIP();
#ifdef ESP32
    this->System->Debugger->printBulletPoint(F("Broadcast: "));
    Serial << this->WirelessNetwork.broadcastIP();
#endif
    this->System->Debugger->printBulletPoint(F("DNS1: "));
    Serial << this->WirelessNetwork.dnsIP(0);
    this->System->Debugger->printBulletPoint(F("DNS2: "));
    Serial << this->WirelessNetwork.dnsIP(1);
#ifdef ESP32
    this->System->Debugger->printBulletPoint(F("Network ID: "));
    Serial << this->WirelessNetwork.networkID();
#endif
    this->System->Debugger->printBulletPoint(F("RSSI: "));
    Serial << this->WirelessNetwork.RSSI();
#ifdef ESP32
    this->System->Debugger->printBulletPoint(F("Hostname: "));
    Serial << this->WirelessNetwork.getHostname();
#else
    this->System->Debugger->printBulletPoint(F("Hostname: "));
    Serial << this->WirelessNetwork.hostname();
#endif
    this->System->Debugger->printHeader(1, 1, ESPAPP_MSG_HEADER_DEFAULT_LENGTH, ESPAPP_DEBUGGER_MESSAGE_HEADER_TYPE::DASH);
#endif

    if (this->configuration->mDNS)
    {
      // @TODO imlement this
      // char _mDNSDeviceID[AFE_CONFIG_DEVICE_ID_SIZE + 4]; // extended deviceId "afe-1234-5678"
      char _mDNSDeviceID[ESPAPP_DEVICE_ID_LENGTH]; // extended deviceId "afe-1234-5678"
      this->System->getDeviceID(_mDNSDeviceID, true);

      if (MDNS.begin(_mDNSDeviceID))
      {
#ifdef DEBUG
        this->System->Debugger->printBulletPoint(F("mDNS responder started at: "));
        this->System->Debugger->printValue(_mDNSDeviceID);
#endif

        MDNS.addService("http", "tcp", 80);
#ifdef DEBUG
        this->System->Debugger->printBulletPoint(F("mDNS service added: http"));
      }
      else
      {
        this->System->Debugger->printError(F("mDNS responder failed to start"), F("WIFI"));
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

#ifdef AFE_CONFIG_HARDWARE_LED
  ledStartTime = 0;
  Led->off();
#endif
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

uint8_t ESPAPP_WirelessConnection::connection()
{

  if (ESPAPP_WirelessConnection::isConnected)
  {
    return ESPAPP_NETWORK_CONNECTION_MODE::CLIENT;
  } // Check if device is in access point mode
  else if (this->WirelessNetwork.getMode() == WIFI_AP || this->WirelessNetwork.getMode() == WIFI_AP_STA)
  {
    return ESPAPP_NETWORK_CONNECTION_MODE::ACCESS_POINT;
  }
  // Not connected to any network
  else
  {
    return ESPAPP_NETWORK_CONNECTION_MODE::OFFLINE;
  }
}