#ifndef _ESPAPP_Parameters_h
#define _ESPAPP_Parameters_h

#include <Arduino.h>

#include <ESPAPP_Parameters_HTML_UI.h>

#define ESP_APP_NONE 255
#define ESP_APP_EMPTY_STRING ""
#define ESP_APP_DEFAULT_DEVICE_NAME "ESP Device"

/* Configuration of Terminal messages outputs */
#ifdef DEBUG

/* Colors of fonts for Teminal messages */
#define ESP_APP_TERMINAL_FONT_RED "\u001b[31m"
#define ESP_APP_TERMINAL_FONT_BLUE "\u001b[34m"
#define ESP_APP_TERMINAL_FONT_GREEN "\u001b[32m"
#define ESP_APP_TERMINAL_FONT_RESET_COLOR "\u001b[0m"

/* Header types */
#define ESP_APP_MSG_HEADER_TYPE_HASH 0
#define ESP_APP_MSG_HEADER_TYPE_DASH 1
#define ESP_APP_MSG_HEADER_TYPE_SPACE 2

#endif /* End of Terminal output messages */

/* Boot modes */
#define ESP_APP_BOOT_MODE_FIRST_TIME_BOOT 0
#define ESP_APP_BOOT_MODE_CONFIGURATION 10
#define ESP_APP_BOOT_MODE_NORMAL 20

/* Network: Parameters */
#define ESP_APP_NETWORK_CONNECTION_MODE_NO_CONNECTION 0
#define ESP_APP_NETWORK_CONNECTION_MODE_HOTSPOT 1
#define ESP_APP_NETWORK_CONNECTION_MODE_CLIENT 2

#define ESP_APP_NETWORK_DEFAULT_NONE_SSID "<nOOn>"
#define ESP_APP_NETWORK_DEFAULT_CONNECTION_ATTEMPTS 20
#define ESP_APP_NETWORK_DEFAULT_WAIT_TIME 1
#define ESP_APP_NETWORK_DEFAULT_WAIT_SERIES 20
#define ESP_APP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER 2
#define ESP_APP_NETWORK_DEFAULT_DHCP 1
#define ESP_APP_NETWORK_DEFAULT_GATEWAY ""
#define ESP_APP_NETWORK_DEFAULT_SUBNET ""
#define ESP_APP_NETWORK_DEFAULT_DNS1 "8.8.8.8"
#define ESP_APP_NETWORK_DEFAULT_DNS2 "8.8.4.4"
#define ESP_APP_NETWORK_DEFAULT_MDNS 1

#ifndef ESP32
#define ESP_APP_NETWORK_DEFAULT_RADIO_MODE ESP_APP_NONE
#define ESP_APP_NETWORK_DEFAULT_OUTPUT_POWER ESP_APP_NONE
#define ESP_APP_NETWORK_DEFAULT_OUTPUT_POWER_MIN 0
#define ESP_APP_NETWORK_DEFAULT_OUTPUT_POWER_MAX 20.5
#endif

/* HTTP Server */
#include <ESPAPP_Parameters_HTTP_Server.h>
#define ESP_APP_HTTP_HTML_MAX_RESPONSE_SIZE 2048
#define ESP_APP_HTTP_RESPONSE_CODE_OK 200
#define ESP_APP_HTTP_RESPONSE_CODE_NOT_FOUND 404

/* File system */
#define ESP_APP_FILE_MAX_FILE_NAME_LENGTH 30
#define ESP_APP_FILE_MAX_DIRECTORY_NAME_LENGTH 20
#define ESP_APP_FILE_MAX_SIZE 10*1024 // 10kB

const char path_root[] PROGMEM = "";
const char path_configuration[] PROGMEM = "cfg";
const char path_data[] PROGMEM = "data";
const char *const ESP_APP_DIRECTORIES[] PROGMEM = {path_root, path_configuration, path_data};

/* Data structures */

struct NETWORK_SETTING
{
  char ssid[33];
  char password[33];
  bool isDHCP; // = ESP_APP_NETWORK_DEFAULT_DHCP;
  char ip[16];
  char gateway[16]; // = ESP_APP_NETWORK_DEFAULT_GATEWAY;
  char subnet[16];  // = ESP_APP_NETWORK_DEFAULT_SUBNET;
  char dns1[16];    // = ESP_APP_NETWORK_DEFAULT_DNS1;
  char dns2[16];    // = ESP_APP_NETWORK_DEFAULT_DNS2;
};

struct NETWORK
{
  NETWORK_SETTING primary;
  NETWORK_SETTING secondary;
  uint8_t noConnectionAttempts;      // = ESP_APP_NETWORK_DEFAULT_CONNECTION_ATTEMPTS;
  uint8_t waitTimeConnections;       // = ESP_APP_NETWORK_DEFAULT_WAIT_TIME;
  uint8_t waitTimeSeries;            // = ESP_APP_NETWORK_DEFAULT_WAIT_TIME;
  uint8_t noFailuresToSwitchNetwork; // = ESP_APP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER;
  bool mDNS;
#if !defined(ESP32)
  uint8_t radioMod ESP_APP_NETWORK_DEFAULT_RADIO_MODE;
  float outputPower = ESP_APP_NETWORK_DEFAULT_OUTPUT_POWER_MAX;
#endif
};

typedef struct 
{
  char name[ESP_APP_FILE_MAX_DIRECTORY_NAME_LENGTH];
  bool isDirectory;
  size_t size;
} ESPAPP_FILE;

struct ESPAPP_HTTP_REQUEST
{
  uint8_t siteId = ESP_APP_NONE;
  uint8_t command = ESP_APP_NONE;
  uint8_t action = ESP_APP_NONE;
  uint8_t option = ESP_APP_NONE;
  int HTTPResponseCode = ESP_APP_HTTP_RESPONSE_CODE_OK;
  ESPAPP_FILE *file = new ESPAPP_FILE;
};

#endif // _ESPAPP_Parameters_h