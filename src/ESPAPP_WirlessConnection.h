/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#ifndef _ESPAPP_WirlessConnection_h
#define _ESPAPP_WirlessConnection_h


#include <ESPAPP_Parameters.h>
#include <ArduinoJson.h>
#include <ESPAPP_API_Flash.h>

struct NETWORK_SETTING {
  char ssid[33];
  char password[33];
  uint8_t isDHCP;
  char ip[16];
  char gateway[16];
  char subnet[16];
  char dns1[16];
  char dns2[16];
};

struct NETWORK {
  NETWORK_SETTING primary;
  NETWORK_SETTING secondary;
  uint8_t noConnectionAttempts;
  uint8_t waitTimeConnections;
  uint8_t waitTimeSeries;
  uint8_t noFailuresToSwitchNetwork;
#if !defined(ESP32)
  uint8_t radioMode;
  float outputPower;
#endif
};


//#include <AFE-Data-Access.h>
//#include <AFE-Device.h>

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

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

class ESPAPP_WirlessConnection {

private:
 // AFEDevice *Device;
  //AFEDataAccess *Data;
  ESPAPP_API_Flash *Flash = new ESPAPP_API_Flash();
  unsigned long delayStartTime = 0;

#ifdef AFE_CONFIG_HARDWARE_LED
  unsigned long ledStartTime = 0;
#endif // AFE_CONFIG_HARDWARE_LED

  /**
   * @brief Used to count connection attemps before going to sleep mode
   *
   */
  uint8_t connections = 0;

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

#ifdef AFE_CONFIG_HARDWARE_LED
  AFELED *Led;
#endif // AFE_CONFIG_HARDWARE_LED

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

#ifdef DEBUG
  ESPAPP_SerialMessages *Msg;
  ESPAPP_WirlessConnection();
#endif

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


  #ifdef DEBUG
  ESPAPP_WirlessConnection(ESPAPP_SerialMessages *);
  #else
  ESPAPP_WirlessConnection();
  #endif 


  NETWORK *configuration = new NETWORK;

#ifdef ESP32
  WiFiClass WirelessNetwork;
#else  // ESP8266
  ESP8266WiFiClass WirelessNetwork;
#endif // ESP32/ESP8266


  /**
   * @brief Sets connection parameters and host name. Must be invoked before
   * connect method
   *
   * @param
   */
  //void begin(AFEDevice *, AFEDataAccess *);
  void begin();


  /**
   * @brief Return TRUE if device is connected to WiFi Access Point
   *
   * @return boolean
   */
  boolean connected();

  /**
   * @brief Returns true if device just connected to the network. It's set to
   * true each
     * time it connects
   *
   * @return boolean
   */
  boolean eventConnected();

  /**
   * @brief Returns true if device just disconnected from the network. It's set
   * to true each
   * time it disconnects
   *
   * @return boolean
   */
  boolean eventDisconnected();

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

#endif // _ESPAPP_WirlessConnection_h
