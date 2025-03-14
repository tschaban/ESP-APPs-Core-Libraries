#ifndef _ESPAPP_AccessToWAN_h
#define _ESPAPP_AccessToWAN_h

#include <ESP32Ping.h>
#include <ESPAPP_Core.h>
#include <ArduinoJson.h>


struct WAN_CONFIG
{
    char ipAddress[16] = ESPAPP_WAN_ACCSSS_IP;
    uint8_t pingAttempts = ESPAPP_WAN_ACCSSS_NO_OF_PING_ATTEMPTS;
    uint16_t checkInterval = ESPAPP_WAN_DEFAULT_CHECK_INTERVAL;
    bool autoCheck = ESPAPP_WAN_DEFAULT_AUTO_CHECK;
};

class ESPAPP_AccessToWAN
{
private:
    /* data */
    bool connectedToWAN = false;
    bool previousConnectionStatus = connectedToWAN;

    ESPAPP_Core *System;
    
    WAN_CONFIG *configuration = new WAN_CONFIG;
    
    bool readConfiguration(void);
    bool createDefaultConfiguration(void);
    bool init(void);
    
public:
    ESPAPP_AccessToWAN(ESPAPP_Core *_System);
    ~ESPAPP_AccessToWAN();

    PingClass *Ping = new PingClass();

    bool connected(void);
    void setDisconnected(void);
    void checkAccessToWAN(void);
    
    // Configuration management
    bool saveConfiguration(void);
    
    // Configuration getters
    uint16_t getCheckInterval(void);
    bool autoCheck(void);
};

#endif // _ESPAPP_AccessToWAN_h