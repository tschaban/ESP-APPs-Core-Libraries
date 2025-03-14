#ifndef _ESPAPP_EventManager_h
#define _ESPAPP_EventManager_h

#include <Arduino.h>
#include <functional>
#include <map>
#include <vector>
#include <time.h>
#include <ESPAPP_Parameters_Events.h>

#ifdef DEBUG
#include <ESPAPP_SerialMessages.h>
#endif

// Event callback type definition
typedef std::function<void(void *)> EventCallback;

// Structure to hold scheduled event information
struct SCHEDULED_EVENT
{
    uint16_t id;        // Unique identifier for the event
    uint16_t eventId;   // The event ID to trigger
    time_t triggerTime; // When to trigger (Unix timestamp)
    uint8_t repeatMode; // Once, repeat, daily, etc.
    uint32_t interval;  // Interval in seconds for repeating events
    void *data;         // Optional data to pass to the event handler
    bool active;        // Whether the event is active
};

class ESPAPP_EventManager
{
private:
    std::map<uint16_t, std::vector<EventCallback>> eventCallbacks;
    std::map<uint16_t, SCHEDULED_EVENT> scheduledEvents;
    uint16_t nextScheduledEventId = 1;

#ifdef DEBUG
    ESPAPP_SerialMessages *Msg;
    const char *getEventName(uint16_t eventId);
#endif

public:
#ifdef DEBUG
    ESPAPP_EventManager(ESPAPP_SerialMessages *_Msg);
#else
    ESPAPP_EventManager();
#endif
    ~ESPAPP_EventManager();

    // Regular event methods
    void addEventListener(uint16_t eventId, EventCallback callback);
    void removeEventListeners(uint16_t eventId);
    void triggerEvent(uint16_t eventId, void *data = nullptr);
    bool hasListeners(uint16_t eventId);

    // Scheduled event methods
    uint16_t scheduleEvent(uint16_t eventId, time_t triggerTime, uint8_t repeatMode = SCHEDULE_ONCE,
                           uint32_t interval = 0, void *data = nullptr);
    uint16_t scheduleEventIn(uint16_t eventId, uint32_t delaySeconds, uint8_t repeatMode = SCHEDULE_ONCE,
                             uint32_t interval = 0, void *data = nullptr);
    uint16_t scheduleEventAt(uint16_t eventId, uint8_t hour, uint8_t minute, uint8_t second,
                             uint8_t repeatMode = SCHEDULE_DAILY, void *data = nullptr);
    bool rescheduleEvent(uint16_t scheduleId, time_t newTriggerTime);
    bool rescheduleEventIn(uint16_t scheduleId, uint32_t delaySeconds);
    bool cancelScheduledEvent(uint16_t scheduleId);
    void checkScheduledEvents(time_t currentTime);
    int countActiveScheduledEvents();
    void listScheduledEvents();
};

#endif // _ESPAPP_EventManager_h