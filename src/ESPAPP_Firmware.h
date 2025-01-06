/* ESPAPP Firmware for smarthome devices, More info: https://ESPAPP.smartnydom.pl/ */

#ifndef _ESPAPP_Firmware_h
#define _ESPAPP_Firmware_h

#include <ESPAPP_Parameters.h>
#include <ArduinoJson.h>
//#include <ESPAPP-API-JSONRPC.h>
//#include <ESPAPP-Data-Access.h>
//#include <ESPAPP-Device.h>
//#include <ESPAPP_WirlessConnection.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

class ESPAPP_Firmware //: public ESPAPPCoreHardware
{

private:
  unsigned long milliseconds = 0;
  unsigned long minutes = 0;
  /**
   * @brief Checking if the device is launched for a first time. If so it loads
   * default configuration
   *
   */
  //void firstBooting(void);

public:

#ifdef DEBUG
  ESPAPP_SerialMessages *Msg = new ESPAPP_SerialMessages();
#endif

  struct GLOBAL_API_OBJECTS {
   // ESPAPP_WirlessConnection *Network = new ESPAPP_WirlessConnection();
   // ESPAPPJSONRPC *REST = new ESPAPPJSONRPC();
    //ESPAPPDataAccess *Flash = new ESPAPPDataAccess();
  };

  struct GLOBAL_CONFIGURATION_OBJECTS {
  //  PRO_VERSION *Pro = new PRO_VERSION;
  //  FIRMWARE *Version = new FIRMWARE;
  };

  struct TIMER_OBJECT {
    unsigned long milliseconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t days;
    uint8_t months;
    // time_t currentTime;
  };

  TIMER_OBJECT *timer = new TIMER_OBJECT{0, 0, 0, 0, 0};

  GLOBAL_API_OBJECTS *API = new GLOBAL_API_OBJECTS;
  
  GLOBAL_CONFIGURATION_OBJECTS *Configuration =
      new GLOBAL_CONFIGURATION_OBJECTS;

  //ESPAPPDevice *Device = new ESPAPPDevice();

  /* Constructor */
  ESPAPP_Firmware();

  void begin();
  uint8_t getBootMode(void);

  //boolean initializeFS(void);

  //void initializeNetwork(void);
  //void checkFirmwareVersion(void);
  //void validateProVersion(void);
  //void synchronizeTime(void);
  /* Currently not used. If needed uncomment it */
  //void getCurrentTime(char *timestamp);
};

#endif
