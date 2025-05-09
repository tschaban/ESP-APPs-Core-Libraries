/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#ifndef _ESPAPP_WirelessConnection_h
#define _ESPAPP_WirelessConnection_h

#include <ArduinoJson.h>
#include <ESPAPP_Core.h>
#ifdef AFE_CONFIG_HARDWARE_LED
#include <AFE-LED.h>
#endif // AFE_CONFIG_HARDWARE_LED

#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#else // ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif // ESP32/ESP8266


class ESPAPP_WirelessConnection
{

private:
  ESPAPP_Core *System;
  unsigned long delayStartTime = 0;
  bool ready = false; // is set to true when device is ready to connect to WiFi

#ifdef AFE_CONFIG_HARDWARE_LED
  unsigned long ledStartTime = 0;
  AFELED *Led;
#endif // AFE_CONFIG_HARDWARE_LED

  /**
   * @brief Used to count connection attemps before going to sleep mode
   *
   */
  uint16_t secondsCounter = 0;

  /**
   * @brief Used to switch between Primary and Backup configuration
   *
   */
  uint8_t noOfFailures = 0;

  /**
   * @brief It's set to true after defined in configuration X number of
   * connection
   * failures
   *
   */
  boolean sleepMode = false;
  unsigned long sleepStartTime = 0;
  uint16_t secondsTimer = 0;



  /**
   * @brief is set to true when connection to WiFi is established
   *
   */
  static boolean eventConnectionEstablished;

  /**
   * @brief it's set to true when disconects from the WiFi
   *
   */
  static boolean eventConnectionLost;

  /**
   * @brief is true when is connected to WiFi
   *
   */
  static boolean isConnected;

  /**
   * @brief Is set to true is backup configuratio exists
   *
   */
  boolean isBackupConfigurationSet = false;

  /**
   *@brief Indicates on to which router the device is connected
   *
   */
  boolean isPrimaryConfiguration = false;

  /**
   * @brief swiches between primary and secondary (and vice-versa) configuration
   *
   */
  void switchConfiguration();

  void connectionEvent(void);

public:
#ifndef ESP32
  /**
   * @brief Handlders for WiFi connected and disconmected events
   *
   */
  WiFiEventHandler wifiConnectHandler;
  WiFiEventHandler wifiDisconnectHandler;
  WiFiEventHandler wifiAPStationConnectedHandler;

#endif

  /* Constructor */
  ESPAPP_WirelessConnection(ESPAPP_Core *_System);

  bool init(void);

  ESPAPP_NETWORK *configuration = new ESPAPP_NETWORK;

#ifdef ESP32
  WiFiClass WirelessNetwork;
#else  // ESP8266
  ESP8266WiFiClass WirelessNetwork;
#endif // ESP32/ESP8266

  uint8_t connection();

  /**
   * @brief Method checks if device is connected to WiFi - if it's not then it
   * connects
   * to it
   *
   */
  void listener();


#ifdef ESP32
  /**
   * @brief triggered on each WiFi event
   *
   * @param  event            desc
   */
  static void onWiFiEvent(WiFiEvent_t event);
#else
  /**
   * @brief triggers on WiFi Connection
   *
   * @param  event            desc
   */
  static void onWifiConnect(const WiFiEventStationModeGotIP &event);

  /**
   * @brief triggers on WiFi disconnection
   *
   * @param  event            desc
   */
  static void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);

#ifdef DEBUG
  static void onWiFiAPStationConnected(const WiFiEventSoftAPModeStationConnected &event);
#endif

#endif

  
  

};

#endif // _ESPAPP_WirelessConnection_h
