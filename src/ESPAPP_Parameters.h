#ifndef _ESPAPP_Parameters_h
#define _ESPAPP_Parameters_h

#include <Arduino.h>

#include <ESPAPP_Parameters_HTML_UI.h>

#define ESPAPP_NONE 255
#define ESPAPP_EMPTY_STRING ""
#define ESPAPP_DEFAULT_DEVICE_NAME "ESP Device"

/* Configuration of Terminal messages outputs */
#ifdef DEBUG

/* Colors of fonts for Teminal messages */
#define ESPAPP_TERMINAL_FONT_RED "\u001b[31m"
#define ESPAPP_TERMINAL_FONT_BLUE "\u001b[34m"
#define ESPAPP_TERMINAL_FONT_GREEN "\u001b[32m"
#define ESPAPP_TERMINAL_FONT_RESET_COLOR "\u001b[0m"

/* Header types */
#define ESPAPP_MSG_HEADER_TYPE_HASH 0
#define ESPAPP_MSG_HEADER_TYPE_DASH 1
#define ESPAPP_MSG_HEADER_TYPE_SPACE 2

#endif /* End of Terminal output messages */

/* Boot modes */
#define ESPAPP_BOOT_MODE_FIRST_TIME_BOOT 0
#define ESPAPP_BOOT_MODE_CONFIGURATION 10
#define ESPAPP_BOOT_MODE_NORMAL 20

/* Network: Parameters */
#define ESPAPP_NETWORK_CONNECTION_MODE_NO_CONNECTION 0
#define ESPAPP_NETWORK_CONNECTION_MODE_HOTSPOT 1
#define ESPAPP_NETWORK_CONNECTION_MODE_CLIENT 2

#define ESPAPP_NETWORK_DEFAULT_NONE_SSID "<nOOn>"
#define ESPAPP_NETWORK_DEFAULT_CONNECTION_TIMEOUT 15
#define ESPAPP_NETWORK_DEFAULT_SLEEP_TIMEOUT 1
#define ESPAPP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER 2
#define ESPAPP_NETWORK_DEFAULT_DHCP 1
#define ESPAPP_NETWORK_DEFAULT_GATEWAY ""
#define ESPAPP_NETWORK_DEFAULT_SUBNET ""
#define ESPAPP_NETWORK_DEFAULT_DNS1 "8.8.8.8"
#define ESPAPP_NETWORK_DEFAULT_DNS2 "8.8.4.4"
#define ESPAPP_NETWORK_DEFAULT_MDNS 1

#ifndef ESP32
#define ESPAPP_NETWORK_DEFAULT_RADIO_MODE ESPAPP_NONE
#define ESPAPP_NETWORK_DEFAULT_OUTPUT_POWER ESPAPP_NONE
#define ESPAPP_NETWORK_DEFAULT_OUTPUT_POWER_MIN 0
#define ESPAPP_NETWORK_DEFAULT_OUTPUT_POWER_MAX 20.5
#endif

/* HTTP Server */
#include <ESPAPP_Parameters_HTTP_Server.h>

/* File system */
#include <ESPAPP_Parameters_FS.h>


/* Data structures */

struct NETWORK_SETTING
{
  char ssid[33];
  char password[33];
  bool isDHCP; // = ESPAPP_NETWORK_DEFAULT_DHCP;
  char ip[16];
  char gateway[16]; // = ESPAPP_NETWORK_DEFAULT_GATEWAY;
  char subnet[16];  // = ESPAPP_NETWORK_DEFAULT_SUBNET;
  char dns1[16];    // = ESPAPP_NETWORK_DEFAULT_DNS1;
  char dns2[16];    // = ESPAPP_NETWORK_DEFAULT_DNS2;
};

struct NETWORK
{
  NETWORK_SETTING primary;
  NETWORK_SETTING secondary;
  uint16_t connectionTimeout;       // = ESPAPP_NETWORK_DEFAULT_CONNECTION_TIMEOUT;
  uint16_t sleepTimeout;            // = ESPAPP_NETWORK_DEFAULT_SLEEP_TIMEOUT;
  uint8_t failuresToSwitch; // = ESPAPP_NETWORK_DEFAULT_SWITCH_NETWORK_AFTER; 
  bool mDNS;
#if !defined(ESP32)
  uint8_t radioMode ESPAPP_NETWORK_DEFAULT_RADIO_MODE;
  float outputPower = ESPAPP_NETWORK_DEFAULT_OUTPUT_POWER_MAX;
#endif
};

typedef struct 
{
  char name[ESPAPP_FILE_MAX_FILE_NAME_LENGTH];
  bool isDirectory;
  size_t size;
} ESPAPP_FILE;

struct ESPAPP_HTTP_REQUEST
{
  uint8_t siteId = ESPAPP_NONE;
  uint8_t command = ESPAPP_NONE;
  uint8_t action = ESPAPP_NONE;
  uint8_t option = ESPAPP_NONE;
  char parameter1[ESPAPP_HTTP_REQUEST_PARAMETER_1_MAX_LENGTH];
  int HTTPResponseCode = ESPAPP_HTTP_RESPONSE_CODE_OK;
  ESPAPP_FILE *file = new ESPAPP_FILE;
};

#endif // _ESPAPP_Parameters_h