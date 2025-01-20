#include <ESPAPP_AccessToWAN.h>

ESPAPP_AccessToWAN::ESPAPP_AccessToWAN(ESPAPP_Core *_System)
{
    System = _System;
}

ESPAPP_AccessToWAN::~ESPAPP_AccessToWAN()
{
}

bool ESPAPP_AccessToWAN::connected(void)
{
    return connectedToWAN;
}

void ESPAPP_AccessToWAN::setDisconnected(void) {
    connectedToWAN = false;
}

void ESPAPP_AccessToWAN::checkAccessToWAN(void)
{

#ifdef DEBUG
    System->Msg->printHeader(1, 0, 72, ESP_APP_MSG_HEADER_TYPE_DASH);
    System->Msg->printInformation(F("Checking access to the Internet"), F("WAN"));
#endif
    IPAddress ip;
    ip.fromString(ESPAPP_WAN_ACCSSS_IP);
    connectedToWAN = Ping->ping(ip, ESPAPP_WAN_ACCSSS_NO_OF_PING_ATTEMPTS);
#ifdef DEBUG
    if (connectedToWAN)
    {
        System->Msg->printInformation(F("Access to the Internet: "), F("WAN"));
    }
    else
    {
        System->Msg->printWarning(F("No access to the Internet: "), F("WAN"));
    }
    Serial << (uint16_t)Ping->averageTime() << F(" msec");
    System->Msg->printHeader(1, 0, 72, ESP_APP_MSG_HEADER_TYPE_DASH);
#endif
}