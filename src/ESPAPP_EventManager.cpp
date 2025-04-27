#include "ESPAPP_EventManager.h"
#include <time.h>

#ifdef DEBUG
ESPAPP_EventManager::ESPAPP_EventManager(ESPAPP_SerialDebugger *_Debugger)
{
    this->Debugger = _Debugger;
    this->Debugger->printInformation(F("Event Manager initialized"), F("EVENT"));
}
#else
ESPAPP_EventManager::ESPAPP_EventManager()
{
    // Constructor without debug messages
}
#endif

ESPAPP_EventManager::~ESPAPP_EventManager()
{
    // Clean up
    eventCallbacks.clear();
    scheduledEvents.clear();
}

void ESPAPP_EventManager::addEventListener(uint16_t eventId, EventCallback callback)
{
    // Add the callback to the map
    eventCallbacks[eventId].push_back(callback);

#ifdef DEBUG
    this->Debugger->printInformation(F("Event listener added"), F("EVENT"));
    this->Debugger->printBulletPoint(F("Event: "));
    this->Debugger->printValue(getEventName(eventId));
    this->Debugger->printBulletPoint(F("Total listeners: "));
    this->Debugger->printValue((int)eventCallbacks[eventId].size());
#endif
}

void ESPAPP_EventManager::removeEventListeners(uint16_t eventId)
{
    // Remove all callbacks for this event
    if (eventCallbacks.find(eventId) != eventCallbacks.end())
    {
        eventCallbacks[eventId].clear();
        eventCallbacks.erase(eventId);

#ifdef DEBUG
        this->Debugger->printInformation(F("All listeners removed"), F("EVENT"));
        this->Debugger->printBulletPoint(F("Event: "));
        this->Debugger->printValue(getEventName(eventId));
#endif
    }
}

bool ESPAPP_EventManager::hasListeners(uint16_t eventId)
{
    return (eventCallbacks.find(eventId) != eventCallbacks.end() &&
            !eventCallbacks[eventId].empty());
}

#ifdef DEBUG
const char *ESPAPP_EventManager::getEventName(uint16_t eventId)
{
    switch (eventId)
    {
    case EVENT_REBOOT:
        return "REBOOT";
    case EVENT_NETWORK_CONNECTED:
        return "NETWORK_CONNECTED";
    case EVENT_NETWORK_DISCONNECTED:
        return "NETWORK_DISCONNECTED";
    case EVENT_WIFI_CONFIG_CHANGED:
        return "WIFI_CONFIG_CHANGED";
    case EVENT_WAN_CONNECTED:
        return "WAN_CONNECTED";
    case EVENT_WAN_DISCONNECTED:
        return "WAN_DISCONNECTED";
    case EVENT_TIMER_MINUTE:
        return "TIMER_MINUTE";
    case EVENT_TIMER_HOUR:
        return "TIMER_HOUR";
    case EVENT_TIMER_DAY:
        return "TIMER_DAY";
    case EVENT_SYNC_TIME:
        return "SYNC_TIME";
    case EVENT_WAN_CHECK:
        return "WAN_CHECK";
    case EVENT_DOWNLOAD_UI_COMPONENTS:
        return "DOWNLOAD_UI_COMPONENTS";
#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
    case EVENT_HARDWARE_ACS758:
        return "HARDWARE_ACS758";
#endif
    default:
        if (eventId >= EVENT_CUSTOM_START)
        {
            return "CUSTOM_EVENT";
        }
        else
        {
            return "UNKNOWN_EVENT";
        }
    }
}
#endif

void ESPAPP_EventManager::triggerEvent(uint16_t eventId, void *data)
{
    // Check if we have any callbacks registered for this event
    if (eventCallbacks.find(eventId) != eventCallbacks.end())
    {
#ifdef DEBUG
        this->Debugger->printInformation(F("Event triggered: "), F("EVENT"));
        this->Debugger->printValue(getEventName(eventId));
        this->Debugger->printValue(F(", Listeners: "));
        this->Debugger->printValue((uint16_t)eventCallbacks[eventId].size());
#endif

        // Execute all callbacks for this event
        for (auto &callback : eventCallbacks[eventId])
        {
            callback(data);
        }
    }
#ifdef DEBUG
    else
    {
        this->Debugger->printInformation(F("Event triggered (no listeners)"), F("EVENT"));
        this->Debugger->printBulletPoint(F("Event: "));
        this->Debugger->printValue(getEventName(eventId));
    }
#endif
}

// Scheduled event methods implementation

uint16_t ESPAPP_EventManager::scheduleEvent(uint16_t eventId, time_t triggerTime,
                                            uint8_t repeatMode, uint32_t interval, void *data)
{
    uint16_t scheduleId = nextScheduledEventId++;

    SCHEDULED_EVENT event = {
        scheduleId,
        eventId,
        triggerTime,
        repeatMode,
        interval,
        data,
        true};

    scheduledEvents[scheduleId] = event;

#ifdef DEBUG
    char timeStr[25];
    struct tm *tm_info = localtime(&triggerTime);
    strftime(timeStr, 25, "%Y-%m-%d %H:%M:%S", tm_info);

    this->Debugger->printInformation(F("Event scheduled"), F("EVENT"));
    this->Debugger->printBulletPoint(F("Schedule ID: "));
    this->Debugger->printValue(scheduleId);
    this->Debugger->printBulletPoint(F("Event: "));
    this->Debugger->printValue(getEventName(eventId));
    this->Debugger->printBulletPoint(F("Trigger time: "));
    this->Debugger->printValue(timeStr);

    if (repeatMode != SCHEDULE_ONCE)
    {
        this->Debugger->printBulletPoint(F("Repeat mode: "));

        switch (repeatMode)
        {
        case SCHEDULE_REPEAT:
            this->Debugger->printValue(F("Repeating every "));
            this->Debugger->printValue(interval);
            this->Debugger->printValue(F(" seconds"));
            break;
        case SCHEDULE_DAILY:
            this->Debugger->printValue(F("Daily"));
            break;
        case SCHEDULE_WEEKLY:
            this->Debugger->printValue(F("Weekly"));
            break;
        case SCHEDULE_MONTHLY:
            this->Debugger->printValue(F("Monthly"));
            break;
        }
    }
#endif

    return scheduleId;
}

uint16_t ESPAPP_EventManager::scheduleEventIn(uint16_t eventId, uint32_t delaySeconds,
                                              uint8_t repeatMode, uint32_t interval, void *data)
{
    time_t now = time(nullptr);
    time_t triggerTime = now + delaySeconds;

    return scheduleEvent(eventId, triggerTime, repeatMode, interval, data);
}

uint16_t ESPAPP_EventManager::scheduleEventAt(uint16_t eventId, uint8_t hour, uint8_t minute,
                                              uint8_t second, uint8_t repeatMode, void *data)
{
    time_t now = time(nullptr);
    struct tm *tm_info = localtime(&now);

    // Set the time for today
    tm_info->tm_hour = hour;
    tm_info->tm_min = minute;
    tm_info->tm_sec = second;

    time_t triggerTime = mktime(tm_info);

    // If the time is already past for today, schedule for tomorrow
    if (triggerTime <= now)
    {
        triggerTime += 24 * 60 * 60; // Add one day
    }

    return scheduleEvent(eventId, triggerTime, repeatMode, 0, data);
}

bool ESPAPP_EventManager::rescheduleEvent(uint16_t scheduleId, time_t newTriggerTime)
{
    if (scheduledEvents.find(scheduleId) != scheduledEvents.end())
    {
        scheduledEvents[scheduleId].triggerTime = newTriggerTime;
        scheduledEvents[scheduleId].active = true;

#ifdef DEBUG
        char timeStr[25];
        struct tm *tm_info = localtime(&newTriggerTime);
        strftime(timeStr, 25, "%Y-%m-%d %H:%M:%S", tm_info);

        this->Debugger->printInformation(F("Event rescheduled"), F("EVENT"));
        this->Debugger->printBulletPoint(F("Schedule ID: "));
        this->Debugger->printValue((int)scheduleId);
        this->Debugger->printBulletPoint(F("New trigger time: "));
        this->Debugger->printValue(timeStr);
#endif

        return true;
    }

#ifdef DEBUG
    this->Debugger->printWarning(F("Failed to reschedule event - ID not found"), F("EVENT"));
    this->Debugger->printBulletPoint(F("Schedule ID: "));
    this->Debugger->printValue((int)scheduleId);
#endif

    return false;
}

bool ESPAPP_EventManager::rescheduleEventIn(uint16_t scheduleId, uint32_t delaySeconds)
{
    time_t now = time(nullptr);
    time_t newTriggerTime = now + delaySeconds;

    return rescheduleEvent(scheduleId, newTriggerTime);
}

bool ESPAPP_EventManager::cancelScheduledEvent(uint16_t scheduleId)
{
    if (scheduledEvents.find(scheduleId) != scheduledEvents.end())
    {
        scheduledEvents.erase(scheduleId);

#ifdef DEBUG
        this->Debugger->printInformation(F("Scheduled event cancelled"), F("EVENT"));
        this->Debugger->printBulletPoint(F("Schedule ID: "));
        this->Debugger->printValue((int)scheduleId);
#endif

        return true;
    }

#ifdef DEBUG
    this->Debugger->printWarning(F("Failed to cancel scheduled event - ID not found"), F("EVENT"));
    this->Debugger->printBulletPoint(F("Schedule ID: "));
    this->Debugger->printValue((int)scheduleId);
#endif

    return false;
}

void ESPAPP_EventManager::checkScheduledEvents(time_t currentTime)
{
    // Make a copy of the keys because we might modify the map during iteration
    std::vector<uint16_t> keys;
    for (const auto &pair : scheduledEvents)
    {
        keys.push_back(pair.first);
    }

    for (uint16_t scheduleId : keys)
    {
        // Check if this event still exists (it might have been removed by another event)
        if (scheduledEvents.find(scheduleId) == scheduledEvents.end())
        {
            continue;
        }

        SCHEDULED_EVENT &event = scheduledEvents[scheduleId];

        if (event.active && currentTime >= event.triggerTime)
        {
            // Trigger the associated event
            triggerEvent(event.eventId, event.data);

            // Handle repeat modes
            switch (event.repeatMode)
            {
            case SCHEDULE_ONCE:
                // Remove the event after it's triggered once
                scheduledEvents.erase(scheduleId);
                break;

            case SCHEDULE_REPEAT:
                // Reschedule based on the interval
                event.triggerTime = currentTime + event.interval;
                break;

            case SCHEDULE_DAILY:
                // Reschedule for tomorrow at the same time
                event.triggerTime += 24 * 60 * 60;
                break;

            case SCHEDULE_WEEKLY:
                // Reschedule for next week at the same time
                event.triggerTime += 7 * 24 * 60 * 60;
                break;

            case SCHEDULE_MONTHLY:
            {
                // Reschedule for next month at the same time
                struct tm *tm_info = localtime(&event.triggerTime);
                tm_info->tm_mon += 1;
                if (tm_info->tm_mon > 11)
                {
                    tm_info->tm_mon = 0;
                    tm_info->tm_year += 1;
                }
                event.triggerTime = mktime(tm_info);
                break;
            }
            }
        }
    }
}

int ESPAPP_EventManager::countActiveScheduledEvents()
{
    return scheduledEvents.size();
}

void ESPAPP_EventManager::listScheduledEvents()
{
#ifdef DEBUG
    this->Debugger->printInformation(F("Listing scheduled events"), F("EVENT"));
    this->Debugger->printBulletPoint(F("Total events: "));
    this->Debugger->printValue((int)scheduledEvents.size());

    if (scheduledEvents.empty())
    {
        this->Debugger->printBulletPoint(F("No scheduled events"));
        return;
    }

    time_t now = time(nullptr);

    for (const auto &pair : scheduledEvents)
    {
        const SCHEDULED_EVENT &event = pair.second;

        char timeStr[25];
        struct tm *tm_info = localtime(&event.triggerTime);
        strftime(timeStr, 25, "%Y-%m-%d %H:%M:%S", tm_info);

        int32_t secondsRemaining = (int32_t)difftime(event.triggerTime, now);

        this->Debugger->printBulletPoint(F("ID: "));
        this->Debugger->printValue((int)event.id);
        this->Debugger->printBulletPoint(F("  Event: "));
        this->Debugger->printValue(getEventName(event.eventId));
        this->Debugger->printBulletPoint(F("  Time: "));
        this->Debugger->printValue(timeStr);
        this->Debugger->printBulletPoint(F("  Seconds remaining: "));
        this->Debugger->printValue((int)secondsRemaining);

        if (event.repeatMode != SCHEDULE_ONCE)
        {
            this->Debugger->printBulletPoint(F("  Repeat mode: "));

            switch (event.repeatMode)
            {
            case SCHEDULE_REPEAT:
                this->Debugger->printValue(F("Repeating every "));
                this->Debugger->printValue((int)event.interval);
                this->Debugger->printValue(F(" seconds"));
                break;
            case SCHEDULE_DAILY:
                this->Debugger->printValue(F("Daily"));
                break;
            case SCHEDULE_WEEKLY:
                this->Debugger->printValue(F("Weekly"));
                break;
            case SCHEDULE_MONTHLY:
                this->Debugger->printValue(F("Monthly"));
                break;
            }
        }
    }
#endif
}