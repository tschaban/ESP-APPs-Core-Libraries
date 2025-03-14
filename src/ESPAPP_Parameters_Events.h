#ifndef ESPAPP_PARAMETERS_EVENTS_H
#define ESPAPP_PARAMETERS_EVENTS_H

enum ESPAPP_EVENT_TYPE {
    EVENT_TYPE_STANDARD = 0,    // Standard system events
    EVENT_TYPE_SCHEDULED = 1    // Time-scheduled events
  };

enum ESPAPP_EVENT
{
    // System events
    EVENT_REBOOT = 0,
    
    /** Global: Network events */
    EVENT_NETWORK_CONNECTED,
    EVENT_NETWORK_DISCONNECTED,
    EVENT_WIFI_CONFIG_CHANGED,
    EVENT_WAN_CHECK,
    EVENT_WAN_CONNECTED,
    EVENT_WAN_DISCONNECTED,

    /** Global: Time events */
    EVENT_SYNC_TIME,
    EVENT_TIMER_MINUTE,
    EVENT_TIMER_HOUR,
    EVENT_TIMER_DAY,

    // Custom events start from 100 to avoid conflicts with system events
    EVENT_CUSTOM_START = 100
};


// Scheduled event repeat modes
enum ESPAPP_SCHEDULE_REPEAT {
    SCHEDULE_ONCE = 0,          // Run once and delete
    SCHEDULE_REPEAT = 1,        // Repeat at the interval
    SCHEDULE_DAILY = 2,         // Repeat every day at the same time
    SCHEDULE_WEEKLY = 3,        // Repeat every week at the same time
    SCHEDULE_MONTHLY = 4        // Repeat every month at the same time
  };

#endif // ESPAPP_PARAMETERS_EVENTS_H