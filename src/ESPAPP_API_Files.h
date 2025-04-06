/* AFE Firmware for smarthome devices, More info: https://afe.smartnydom.pl/ */

#ifndef _ESPAPP_API_FILES_h
#define _ESPAPP_API_FILES_h

#include <ArduinoJson.h>
#include <ESPAPP_Parameters.h>
#include <ESPAPP_API_Flash.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

class ESPAPP_API_Files
{
private:
#ifdef DEBUG
  ESPAPP_SerialMessages *Msg;
#endif

  ESPAPP_API_Flash *Flash;

  bool createDefaultConnectionModeConfiguration(void);
  bool createDefaultOperatingModeConfiguration(void);
  bool createDefaultNetworkConfiguration(void);

public:
#ifdef DEBUG
  ESPAPP_API_Files(ESPAPP_API_Flash *_Flash, ESPAPP_SerialMessages *_Msg);
#else
  ESPAPP_API_Files(ESPAPP_API_Flash *_Flash);
#endif

  bool read(ESPAPP_NETWORK_CONNECTION_MODE *data);
  bool save(ESPAPP_NETWORK_CONNECTION_MODE *data);

  bool read(ESPAPP_OPERATING_MODE *data);
  bool save(ESPAPP_OPERATING_MODE *data);

  // Network configuration methods
  bool read(ESPAPP_NETWORK *data);
  bool save(ESPAPP_NETWORK *data);


};
#endif // _ESPAPP_API_FILES_h
