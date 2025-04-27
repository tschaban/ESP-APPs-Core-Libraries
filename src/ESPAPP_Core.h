#ifndef _ESPAPP_CORE_h
#define _ESPAPP_CORE_h

#include <ESPAPP_Parameters.h>
#include <ESPAPP_API_Flash.h>
#include <ESPAPP_API_Files.h>
#include <ESPAPP_EventManager.h>
#include <ESPAPP_Time.h>
#include <ESPAPP_Message.h>

#ifdef ESP32 // @TODO it'll be better not having here WiFi it's just neede to make the DeviceId
#include <WiFi.h>
#else // ESP8266
#include <ESP8266WiFi.h>
#endif // ESP32/ESP8266

#ifdef DEBUG
#include <ESPAPP_SerialDebugger.h>
#endif

struct CORE_CONFIGURATION
{
    char deviceName[33] = ESPAPP_DEFAULT_DEVICE_NAME;
};

class ESPAPP_Core
{
private:
    ESPAPP_NETWORK_CONNECTION_MODE *connectionMode = new ESPAPP_NETWORK_CONNECTION_MODE;
    ESPAPP_OPERATING_MODE *operatingMode = new ESPAPP_OPERATING_MODE;

public:
    CORE_CONFIGURATION *configuration = new CORE_CONFIGURATION;

    ESPAPP_Core();
    ~ESPAPP_Core();

    ESPAPP_Time *Time;

#ifdef DEBUG
    ESPAPP_SerialDebugger *Debugger = new ESPAPP_SerialDebugger();
    ESPAPP_API_Flash *Flash = new ESPAPP_API_Flash(Debugger);
    ESPAPP_API_Files *File = new ESPAPP_API_Files(Flash, Debugger);
    ESPAPP_Message *Message = new ESPAPP_Message(Flash, Debugger);
#else
    ESPAPP_API_Flash *Flash = new ESPAPP_API_Flash();
    ESPAPP_API_Files *File = new ESPAPP_API_Files(Flash);
    ESPAPP_Message *Message = new ESPAPP_Message(Flash);
#endif

#ifdef DEBUG
    ESPAPP_EventManager *Events = nullptr;
#else
    ESPAPP_EventManager *Events = new ESPAPP_EventManager();
#endif

    bool init(void);

    /* Method reboots device to specyfic mode  define by MODE_.. */
    void reboot(void);

    ESPAPP_OPERATING_MODE getOperatingMode(void);
    bool setOperatingMode(ESPAPP_OPERATING_MODE mode);

    ESPAPP_NETWORK_CONNECTION_MODE getConnectionMode(void);
    bool setConnectionMode(ESPAPP_NETWORK_CONNECTION_MODE mode);

    void getDeviceID(char *id, boolean extended = false);
};

#endif