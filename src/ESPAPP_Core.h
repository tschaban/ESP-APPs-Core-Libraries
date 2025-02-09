#ifndef _ESPAPP_CORE_h
#define _ESPAPP_CORE_h


#include <ESPAPP_Parameters.h>
#include <ESPAPP_API_Flash.h>
#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

struct CORE_CONFIGURATION
{
    char deviceName[33] = ESP_APP_DEFAULT_DEVICE_NAME;
};


class ESPAPP_Core
{
private:
    /* data */
public:
    
    CORE_CONFIGURATION *configuration = new CORE_CONFIGURATION;

    ESPAPP_Core();
    ~ESPAPP_Core();


#ifdef DEBUG
    ESPAPP_SerialMessages *Msg = new ESPAPP_SerialMessages();
    ESPAPP_API_Flash *Flash = new ESPAPP_API_Flash(Msg);
#else
    ESPAPP_API_Flash *Flash = new ESPAPP_API_Flash();
#endif


  /* Method reboots device to specyfic mode  define by MODE_.. */
  void reboot(uint8_t mode = ESP_APP_NETWORK_CONNECTION_MODE_CLIENT);

  /* Method returns current device mode */
  uint8_t connectionMode(void);



};

#endif