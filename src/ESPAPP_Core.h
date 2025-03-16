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
    uint8_t connectionMode = ESPAPP_NETWORK_CONNECTION_MODE_NO_CONNECTION;

    bool readConnectionModeConfiguration(void);
    bool createDefaultConnectionModeConfiguration(void);
    bool saveConnectionModeConfiguration(void);

public:
    CORE_CONFIGURATION *configuration = new CORE_CONFIGURATION;

    ESPAPP_Core();
    ~ESPAPP_Core();

    ESPAPP_Time *Time;

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
    
    bool init(void);

    /* Method reboots device to specyfic mode  define by MODE_.. */
    void reboot(void);

    uint8_t getConnectionMode(void);
    bool setConnectionMode(uint8_t mode);
};

#endif