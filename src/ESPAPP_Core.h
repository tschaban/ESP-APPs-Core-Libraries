#ifndef _ESPAPP_CORE_h
#define _ESPAPP_CORE_h

#include <ESPAPP_Parameters.h>
#include <ESPAPP_API_Flash.h>
#include <ESPAPP_EventManager.h>
#include <ESPAPP_Time.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

struct CORE_CONFIGURATION
{
    char deviceName[33] = ESPAPP_DEFAULT_DEVICE_NAME;
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

#ifdef DEBUG
    ESPAPP_EventManager *Events = nullptr;
#else
    ESPAPP_EventManager *Events = new ESPAPP_EventManager();
#endif

    ESPAPP_Time *Time;

    /* Method reboots device to specyfic mode  define by MODE_.. */
    void reboot(uint8_t mode = ESPAPP_NETWORK_CONNECTION_MODE_CLIENT);

    /* Method returns current device mode */
    uint8_t connectionMode(void);
};

#endif