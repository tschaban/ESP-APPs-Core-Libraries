#include "ESPAPP_Time.h"

#ifdef DEBUG
ESPAPP_Time::ESPAPP_Time(ESPAPP_API_Flash *_Flash, ESPAPP_SerialMessages *_Msg)
{
    this->Msg = _Msg;
    this->Flash = _Flash;
};
#else
ESPAPP_Time::ESPAPP_Time(ESPAPP_API_Flash *_Flash)
{
    this->Flash = _Flash;
};
#endif


bool ESPAPP_Time::init(void)
{
#ifdef DEBUG
    this->Msg->printInformation(F("Initializing Time system"), F("TIME"));
#endif
    
    // Initialize last time info
    this->lastTimeInfo->isValid = false;
    this->lastTimeInfo->isAM = false;
    this->lastTimeInfo->year = 0;
    this->lastTimeInfo->month = 0;
    this->lastTimeInfo->day = 0;
    this->lastTimeInfo->hour = 0;
    this->lastTimeInfo->minute = 0;
    this->lastTimeInfo->second = 0;


    bool success = readConfiguration();

    if (!success)
    {
#ifdef DEBUG
        this->Msg->printWarning(F("Failed to read time configuration, creating default"), F("TIME"));
#endif
        success = createDefaultConfiguration();
    }

    if (success)
    {
        // Configure timezone
        setTimeZone(this->configuration->timezone);

        // Configure NTP servers
        setNTPServers(this->configuration->primaryNTP, this->configuration->secondaryNTP);

        // If connected to network, attempt initial sync
        if (this->configuration->autoSync)
        {
#ifdef DEBUG
            this->Msg->printInformation(F("Auto time sync is enabled"), F("TIME"));
#endif
        }
    }

    return success;
}

bool ESPAPP_Time::readConfiguration(void)
{
    bool success = false;

#ifdef DEBUG
    this->Msg->printInformation(F("Reading time configuration"), F("TIME"));
#endif

    StaticJsonDocument<512> doc;
    success = this->Flash->getJSON(F("/etc/time.json"), doc);

    if (success)
    {
        strncpy(this->configuration->timezone, doc["tz"] | ESPAPP_TIME_DEFAULT_TIMEZONE, sizeof(this->configuration->timezone) - 1);
        this->configuration->timezone[sizeof(this->configuration->timezone) - 1] = '\0';

        strncpy(this->configuration->primaryNTP, doc["ntp1"] | ESPAPP_TIME_DEFAULT_PRIMARY_NTP, sizeof(this->configuration->primaryNTP) - 1);
        this->configuration->primaryNTP[sizeof(this->configuration->primaryNTP) - 1] = '\0';

        strncpy(this->configuration->secondaryNTP, doc["ntp2"] | ESPAPP_TIME_DEFAULT_SECONDARY_NTP, sizeof(this->configuration->secondaryNTP) - 1);
        this->configuration->secondaryNTP[sizeof(this->configuration->secondaryNTP) - 1] = '\0';

        this->configuration->syncInterval = doc["interval"] | ESPAPP_TIME_DEFAULT_SYNC_INTERVAL;

        this->configuration->autoSync = doc["autoSync"] | true;

        this->configuration->syncTimeout = doc["timeout"] | ESPAPP_TIME_DEFAULT_SYNC_TIMEOUT;

#ifdef DEBUG
        this->Msg->printBulletPoint(F("Timezone: "));
        this->Msg->printValue(this->configuration->timezone);
        this->Msg->printBulletPoint(F("Primary NTP: "));
        this->Msg->printValue(this->configuration->primaryNTP);
        this->Msg->printBulletPoint(F("Secondary NTP: "));
        this->Msg->printValue(this->configuration->secondaryNTP);
        this->Msg->printBulletPoint(F("Sync Interval: "));
        this->Msg->printValue(this->configuration->syncInterval, F(" seconds"));
        this->Msg->printBulletPoint(F("Auto Sync: "));
        this->Msg->printValue(this->configuration->autoSync);
        this->Msg->printBulletPoint(F("Sync Timeout: "));
        this->Msg->printValue(this->configuration->syncTimeout, F(" ms"));

#endif
    }

    return success;
}

bool ESPAPP_Time::createDefaultConfiguration(void)
{
#ifdef DEBUG
    this->Msg->printInformation(F("Creating default time configuration"), F("TIME"));
#endif

    // Set default values
    strncpy(this->configuration->timezone, ESPAPP_TIME_DEFAULT_TIMEZONE, sizeof(this->configuration->timezone));
    strncpy(this->configuration->primaryNTP, ESPAPP_TIME_DEFAULT_PRIMARY_NTP, sizeof(this->configuration->primaryNTP));
    strncpy(this->configuration->secondaryNTP, ESPAPP_TIME_DEFAULT_SECONDARY_NTP, sizeof(this->configuration->secondaryNTP));
    this->configuration->syncInterval = ESPAPP_TIME_DEFAULT_SYNC_INTERVAL;
    this->configuration->autoSync = true;
    this->configuration->syncTimeout = ESPAPP_TIME_DEFAULT_SYNC_TIMEOUT;

    // Save configuration
    return saveConfiguration();
}

bool ESPAPP_Time::saveConfiguration(void)
{
#ifdef DEBUG
    this->Msg->printInformation(F("Saving time configuration"), F("TIME"));
#endif

    StaticJsonDocument<512> doc; // @TODO check the size

    doc["tz"] = this->configuration->timezone;
    doc["ntp1"] = this->configuration->primaryNTP;
    doc["ntp2"] = this->configuration->secondaryNTP;
    doc["interval"] = this->configuration->syncInterval;
    doc["autoSync"] = this->configuration->autoSync;
    doc["sync"] = this->configuration->syncTimeout;

    return this->Flash->saveJSON(F("/etc/time.json"), doc);
}

void ESPAPP_Time::setTimeZone(const char *tz_info)
{
    if (tz_info && strlen(tz_info) > 0)
    {
        strncpy(this->configuration->timezone, tz_info, sizeof(this->configuration->timezone) - 1);
        this->configuration->timezone[sizeof(this->configuration->timezone) - 1] = '\0';

        setenv("TZ", this->configuration->timezone, 1);
        tzset();

#ifdef DEBUG
        this->Msg->printInformation(F("Time zone set to"), F("TIME"));
        this->Msg->printBulletPoint(this->configuration->timezone);
#endif
    }
}

void ESPAPP_Time::setNTPServers(const char *primary, const char *secondary)
{
    if (primary && strlen(primary) > 0)
    {
        strncpy(this->configuration->primaryNTP, primary, sizeof(this->configuration->primaryNTP) - 1);
        this->configuration->primaryNTP[sizeof(this->configuration->primaryNTP) - 1] = '\0';
    }

    if (secondary && strlen(secondary) > 0)
    {
        strncpy(this->configuration->secondaryNTP, secondary, sizeof(this->configuration->secondaryNTP) - 1);
        this->configuration->secondaryNTP[sizeof(this->configuration->secondaryNTP) - 1] = '\0';
    }
    else
    {
        // If no secondary server provided, use primary as secondary
        strncpy(this->configuration->secondaryNTP, this->configuration->primaryNTP, sizeof(this->configuration->secondaryNTP) - 1);
        this->configuration->secondaryNTP[sizeof(this->configuration->secondaryNTP) - 1] = '\0';
    }

#ifdef DEBUG
    this->Msg->printInformation(F("NTP servers configured"), F("TIME"));
    this->Msg->printBulletPoint(F("Primary: "));
    this->Msg->printValue(this->configuration->primaryNTP);
    this->Msg->printBulletPoint(F("Secondary: "));
    this->Msg->printValue(this->configuration->secondaryNTP);
#endif
}

uint16_t ESPAPP_Time::getSynchronizationInterval(void)
{
    return this->configuration->syncInterval;
}

bool ESPAPP_Time::autoSynchronize(void)
{
    return this->configuration->autoSync;
}

bool ESPAPP_Time::synchronize()
{
    bool success = false;
#ifdef DEBUG
    this->Msg->printInformation(F("Starting time synchronization"), F("TIME"));
#endif

    // Configure SNTP
    sntp_servermode_dhcp(0);
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char *)this->configuration->primaryNTP);
    sntp_setservername(1, (char *)this->configuration->secondaryNTP);
    sntp_init();

    // Wait for time to be synchronized
    unsigned long startMillis = millis();
    time_t now;

    while (millis() - startMillis < this->configuration->syncTimeout)
    {
        now = time(nullptr);
        if (now > 1000)
        { // We have a valid time
            timeSynchronized = true;
            lastSyncMillis = millis();

#ifdef DEBUG
            this->Msg->printInformation(F("Time synchronized successfully"), F("TIME"));
            char timeStr[30];
            this->getCurrentTimeFormatted(timeStr, sizeof(timeStr));
            this->Msg->printBulletPoint(F("Current time: "));
            this->Msg->printValue(timeStr);
#endif

            success = true;
            break;
        }

        delay(100);
    }

#ifdef DEBUG
    if (!success)
    {
        this->Msg->printWarning(F("Time synchronization failed"), F("TIME"));
    }
#endif
    sntp_stop();
    return success;
}

void ESPAPP_Time::getCurrentTime(ESPAPP_TIME_INFO *timeInfo)
{
    time_t now = time(nullptr);

    if (now < 1000)
    {
        // Time is not valid yet
        timeInfo->isValid = false;

#ifdef DEBUG
        this->Msg->printWarning(F("Time not synchronized yet"), F("TIME"));
#endif

        return;
    }

    struct tm timeDetails;
    localtime_r(&now, &timeDetails);

    formatTimeInfo(timeInfo, &timeDetails);
}

void ESPAPP_Time::formatTimeInfo(ESPAPP_TIME_INFO *timeInfo, struct tm *timeDetails)
{
    timeInfo->year = timeDetails->tm_year + 1900;
    timeInfo->month = timeDetails->tm_mon + 1;
    timeInfo->day = timeDetails->tm_mday;

    // Handle hour in 12-hour format with AM/PM
    if (timeDetails->tm_hour == 0)
    {
        timeInfo->hour = 12;
        timeInfo->isAM = true;
    }
    else if (timeDetails->tm_hour < 12)
    {
        timeInfo->hour = timeDetails->tm_hour;
        timeInfo->isAM = true;
    }
    else if (timeDetails->tm_hour == 12)
    {
        timeInfo->hour = 12;
        timeInfo->isAM = false;
    }
    else
    {
        timeInfo->hour = timeDetails->tm_hour - 12;
        timeInfo->isAM = false;
    }

    timeInfo->minute = timeDetails->tm_min;
    timeInfo->second = timeDetails->tm_sec;
    timeInfo->isValid = true;
}

void ESPAPP_Time::getCurrentTimeFormatted(char* buffer, size_t bufferSize, const char* format) {
    time_t now = time(nullptr);
    
    if (now < 1000) {
        // Time is not valid yet
        strncpy(buffer, "Time not synchronized", bufferSize - 1);
        buffer[bufferSize - 1] = '\0';
        return;
    }
    
    struct tm timeDetails;
    localtime_r(&now, &timeDetails);
    strftime(buffer, bufferSize, format, &timeDetails);
}

bool ESPAPP_Time::isTimeValid() {
    time_t now = time(nullptr);
    return now > 1000;
}

uint32_t ESPAPP_Time::getEpochTime() {
    return (uint32_t)time(nullptr);
}

void ESPAPP_Time::printTimeInfo(ESPAPP_TIME_INFO* timeInfo) {
    if (!timeInfo->isValid) {
#ifdef DEBUG
        this->Msg->printWarning(F("Time not synchronized yet"), F("TIME"));
#endif
        return;
    }
    
#ifdef DEBUG
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d %s", 
        timeInfo->year, timeInfo->month, timeInfo->day,
        timeInfo->hour, timeInfo->minute, timeInfo->second,
        timeInfo->isAM ? "AM" : "PM");
    
    this->Msg->printInformation(F("Current time"), F("TIME"));
    this->Msg->printBulletPoint(buffer);
#endif
}

