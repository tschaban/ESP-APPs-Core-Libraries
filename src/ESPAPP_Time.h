
#ifndef _ESPAPP_TIME_H
#define _ESPAPP_TIME_H

#include <ESPAPP_Parameters.h>
#include <ESPAPP_API_Flash.h>
#include <ArduinoJson.h>
#include <esp_sntp.h>
#include <time.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

class ESPAPP_Time
{
private:
    ESPAPP_API_Flash *Flash;
#ifdef DEBUG
    ESPAPP_SerialMessages *Msg;
#endif

    TIME_CONFIG *configuration = new TIME_CONFIG;

    bool timeSynchronized = false;
    unsigned long lastSyncMillis = 0;

    void formatTimeInfo(TIME_INFO *timeInfo, struct tm *timeDetails);
    bool readConfiguration(void);
    bool createDefaultConfiguration(void);

public:
#ifdef DEBUG
    ESPAPP_Time(ESPAPP_API_Flash *_Flash, ESPAPP_SerialMessages *_Msg);
#else
    ESPAPP_Time(ESPAPP_API_Flash *_Flash);
#endif

    TIME_INFO *lastTimeInfo = new TIME_INFO;

    bool init(void);

    // Configuration methods
    void setTimeZone(const char *tz_info);
    void setNTPServers(const char *primary, const char *secondary = nullptr);

    // NTP synchronization methods
    bool synchronize();
    bool isSynchronized() { return timeSynchronized; }
    uint16_t getSynchronizationInterval(void);
    bool autoSynchronize(void);

    // Time retrieval methods
    void getCurrentTime(TIME_INFO *timeInfo);
    void getCurrentTimeFormatted(char *buffer, size_t bufferSize, const char *format = "%Y-%m-%d %H:%M:%S");

    // Utility methods
    bool isTimeValid();
    uint32_t getEpochTime();
    void printTimeInfo(TIME_INFO *timeInfo);

    // Save configuration
    bool saveConfiguration();
};

#endif // _ESPAPP_TIME_H