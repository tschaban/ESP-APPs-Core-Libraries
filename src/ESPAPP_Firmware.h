/* ESPAPP Firmware for smarthome devices, More info: https://ESPAPP.smartnydom.pl/ */

#ifndef _ESPAPP_Firmware_h
#define _ESPAPP_Firmware_h

#include <ESPAPP_Parameters.h>
#include <ESPAPP_Core.h>
// #include <ESPAPP-API-JSONRPC.h>
#include <ESPAPP_WirelessConnection.h>
#include <ESPAPP_AccessToWAN.h>
#include <ESPAPP_HTTP_Server_Container.h>

class ESPAPP_Firmware //: public ESPAPPCoreHardware
{

private:
  bool initializeNetwork(void);
  // void firstBooting(void);

  // Event handlers
  void handleMinuteEvent(void *data);
  void handleHourEvent(void *data);
  void handleDayEvent(void *data);
  void handleRebootEvent(void *data);
  void handleNetworkConnectedEvent(void *data);

  // Initialize event listeners
  void initializeEventListeners(void);


      // Internal time event tracking
      void checkTimeEvents(void);

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

  GLOBAL_API_OBJECTS *API = new GLOBAL_API_OBJECTS;

  GLOBAL_CONFIGURATION_OBJECTS *Configuration =
      new GLOBAL_CONFIGURATION_OBJECTS;

  // ESPAPPDevice *Device = new ESPAPPDevice();

  /* Constructor */
  ESPAPP_Firmware();

  bool init(void);
  void coreListener(void);

};

#endif
