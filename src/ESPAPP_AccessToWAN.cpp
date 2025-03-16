#include <ESPAPP_AccessToWAN.h>

ESPAPP_AccessToWAN::ESPAPP_AccessToWAN(ESPAPP_Core *_System)
{
    this->System = _System;
    this->init();
}

ESPAPP_AccessToWAN::~ESPAPP_AccessToWAN()
{
    delete configuration;
}

bool ESPAPP_AccessToWAN::connected(void)
{
    return this->connectedToWAN;
}

void ESPAPP_AccessToWAN::setDisconnected(void)
{
    this->connectedToWAN = false;
}

bool ESPAPP_AccessToWAN::init(void)
{
#ifdef DEBUG
    this->System->Msg->printInformation(F("Initializing WAN access configuration"), F("WAN"));
#endif

    bool success = this->readConfiguration();

    if (!success)
    {
#ifdef DEBUG
        this->System->Msg->printWarning(F("Failed to read WAN configuration, creating default"), F("WAN"));
#endif
        success = this->createDefaultConfiguration();
    }

    /** Setup autocheck */
    if (this->configuration->autoCheck)
    {
        this->System->Events->scheduleEventIn(EVENT_WAN_CHECK, this->configuration->checkInterval, SCHEDULE_REPEAT, this->configuration->checkInterval);
        this->System->Events->addEventListener(EVENT_WAN_CHECK, [this](void *data)
                                               { 
            if (this->System->getConnectionMode()== 2) {this->checkAccessToWAN(); } });
    }

    return success;
}

bool ESPAPP_AccessToWAN::readConfiguration(void)
{
    bool success = false;

#ifdef DEBUG
    this->System->Msg->printInformation(F("Reading WAN configuration"), F("WAN"));
#endif

    StaticJsonDocument<256> doc;
    success = this->System->Flash->getJSON(F("/cfg/wanCheck.json"), doc);

    if (success)
    {
        strncpy(this->configuration->ipAddress, doc["ip"] | ESPAPP_WAN_ACCSSS_IP, sizeof(this->configuration->ipAddress) - 1);
        this->configuration->ipAddress[sizeof(this->configuration->ipAddress) - 1] = '\0';

        this->configuration->pingAttempts = doc["pings"] | ESPAPP_WAN_ACCSSS_NO_OF_PING_ATTEMPTS;
        this->configuration->checkInterval = doc["interval"] | ESPAPP_WAN_DEFAULT_CHECK_INTERVAL;
        this->configuration->autoCheck = doc["autoCheck"] | ESPAPP_WAN_DEFAULT_AUTO_CHECK;

#ifdef DEBUG
        this->System->Msg->printBulletPoint(F("IP Address: "));
        this->System->Msg->printValue(this->configuration->ipAddress);
        this->System->Msg->printBulletPoint(F("Ping Attempts: "));
        this->System->Msg->printValue(this->configuration->pingAttempts);
        this->System->Msg->printBulletPoint(F("Check Interval: "));
        this->System->Msg->printValue(this->configuration->checkInterval, F(" seconds"));
        this->System->Msg->printBulletPoint(F("Auto Check: "));
        this->System->Msg->printValue(this->configuration->autoCheck);
#endif
    }

    return success;
}

bool ESPAPP_AccessToWAN::createDefaultConfiguration(void)
{
#ifdef DEBUG
    this->System->Msg->printInformation(F("Creating default WAN configuration"), F("WAN"));
#endif

    // Default values are already set in the structure definition
    return this->saveConfiguration();
}

bool ESPAPP_AccessToWAN::saveConfiguration(void)
{
#ifdef DEBUG
    this->System->Msg->printInformation(F("Saving WAN configuration"), F("WAN"));
#endif

    StaticJsonDocument<256> doc;

    doc["ip"] = this->configuration->ipAddress;
    doc["pings"] = this->configuration->pingAttempts;
    doc["interval"] = this->configuration->checkInterval;
    doc["autoCheck"] = this->configuration->autoCheck;

    return this->System->Flash->saveJSON(F("/cfg/wanCheck.json"), doc);
}

uint16_t ESPAPP_AccessToWAN::getCheckInterval(void)
{
    return this->configuration->checkInterval;
}

bool ESPAPP_AccessToWAN::autoCheck(void)
{
    return this->configuration->autoCheck;
}

void ESPAPP_AccessToWAN::checkAccessToWAN(void)
{
#ifdef DEBUG
    this->System->Msg->printHeader(1, 0, 72, ESPAPP_MSG_HEADER_TYPE_DASH);
    this->System->Msg->printInformation(F("Checking access to the Internet"), F("WAN"));
#endif
    IPAddress ip;
    ip.fromString(this->configuration->ipAddress);
    this->connectedToWAN = this->Ping->ping(ip, this->configuration->pingAttempts);
    if (this->connectedToWAN != this->previousConnectionStatus)
    {
        this->previousConnectionStatus = this->connectedToWAN;
        this->System->Events->triggerEvent(this->connectedToWAN ? EVENT_WAN_CONNECTED : EVENT_WAN_DISCONNECTED);
    }
#ifdef DEBUG
    if (connectedToWAN)
    {
        this->System->Msg->printInformation(F("Access to the Internet: "), F("WAN"));
    }
    else
    {
        this->System->Msg->printWarning(F("No access to the Internet: "), F("WAN"));
    }
    Serial << (uint16_t)Ping->averageTime() << F(" msec");
    this->System->Msg->printHeader(1, 0, 72, ESPAPP_MSG_HEADER_TYPE_DASH);
#endif
}