/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#ifndef _ESPAPP_API_FLASH_h
#define _ESPAPP_API_FLASH_h

#include <ArduinoJson.h>

class ESPAPP_API_Flash 
{
  public:
  ESPAPP_API_Flash();
  bool getJSON(const char *fileName, JsonDocument &doc);
};



#endif // _ESPAPP_API_FLASH_h
