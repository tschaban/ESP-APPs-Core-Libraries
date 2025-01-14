#ifndef _ESPAPP_AccessToWAN_h
#define _ESPAPP_AccessToWAN_h

#include <ESP32Ping.h>
#include <ESPAPP_Core.h>

#define ESPAPP_WAN_ACCSSS_IP "1.1.1.1"

class ESPAPP_AccessToWAN
{
private:
    /* data */
    bool connectedToWAN = false;
    ESPAPP_Core *System;
public:
    ESPAPP_AccessToWAN(ESPAPP_Core *_System);
    ~ESPAPP_AccessToWAN();

    PingClass *Ping = new PingClass();

    bool connected(void);
    void checkAccessToWAN(void);
    

};

#endif // _ESPAPP_AccessToWAN_h