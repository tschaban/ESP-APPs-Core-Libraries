/* ESPAPP Firmware for smarthome devices, More info: https://ESPAPP.smartnydom.pl/ */

#ifndef _ESPAPP_Firmware_h
#define _ESPAPP_Firmware_h

#include <ESPAPP_Parameters.h>
#include <ESPAPP_Core.h>
// #include <ESPAPP-API-JSONRPC.h>
// #include <ESPAPP-Data-Access.h>
#include <ESPAPP_WirelessConnection.h>
#include <ESPAPP_AccessToWAN.h>
#include <ESPAPP_HTTP_Server_Container.h>

//#include <ESPAPP_HTML_SitesGenerator.h>

class ESPAPP_Firmware //: public ESPAPPCoreHardware
{

private:
  unsigned long milliseconds = 0;
  unsigned long minutes = 0;

  bool initializeNetwork(void);
  // void firstBooting(void);

public:
  ESPAPP_Core *System = new ESPAPP_Core();
  ESPAPP_HTTPServerContainer *Web;

  struct GLOBAL_API_OBJECTS
  {
    ESPAPP_WirelessConnection *Network;
    ESPAPP_AccessToWAN *WAN;

    // ESPAPPJSONRPC *REST = new ESPAPPJSONRPC();
  };

    struct GLOBAL_CONFIGURATION_OBJECTS
  {
    //  PRO_VERSION *Pro = new PRO_VERSION;
    //  FIRMWARE *Version = new FIRMWARE;
  };

  struct TIMER_OBJECT
  {
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

  // ESPAPPDevice *Device = new ESPAPPDevice();

  /* Constructor */
  ESPAPP_Firmware();

  bool init(void);

  void begin();
  uint8_t getBootMode(void);

  // boolean initializeFS(void);


  void listenerNetwork(void);
  
  // void checkFirmwareVersion(void);
  // void validateProVersion(void);
  // void synchronizeTime(void);
  /* Currently not used. If needed uncomment it */
  // void getCurrentTime(char *timestamp);
};

#endif
