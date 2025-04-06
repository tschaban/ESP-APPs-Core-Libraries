#include "ESPAPP_Firmware.h"

ESPAPP_Firmware::ESPAPP_Firmware()
{
  this->API->Network = new ESPAPP_WirelessConnection(System);
  Web = new ESPAPP_HTTPServerContainer(System);
};

bool ESPAPP_Firmware::init(void)
{
  bool success = this->System->init();

  if (success)
  {
        
    /** Initialize event listeners */
    this->initializeEventListeners();

    success = this->initializeNetwork();

    if (success)
    {
      this->API->WAN = new ESPAPP_AccessToWAN(System);
    }

    if (success)
    {
      success = this->Web->init();
    }
  }
  
  this->System->Message->addMessage(F("Firmware initialized successfully."));
  this->System->Message->addMessage(F("Firmware version: 1.0.0")); // Example version, replace with actual version

  return success;
}

void ESPAPP_Firmware::initializeEventListeners(void)
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Initializing event listeners"), F("EVENT"));
#endif

  // Set up event for time synchronization
  if (this->System->Time->autoSynchronize())
  {
    this->System->Events->scheduleEventIn(EVENT_SYNC_TIME, this->System->Time->getSynchronizationInterval(), SCHEDULE_REPEAT, this->System->Time->getSynchronizationInterval());

    this->System->Events->addEventListener(EVENT_SYNC_TIME,
                                           [this](void *data)
                                           { 
                                            if (this->API->Network->connection() == 2)
                                              {this->System->Time->synchronize();} });
  }

  // Set up event handlers for timer events
  this->System->Events->addEventListener(EVENT_TIMER_MINUTE,
                                         [this](void *data)
                                         { this->handleMinuteEvent(data); });

  this->System->Events->addEventListener(EVENT_TIMER_HOUR,
                                         [this](void *data)
                                         { this->handleHourEvent(data); });

  this->System->Events->addEventListener(EVENT_TIMER_DAY,
                                         [this](void *data)
                                         { this->handleDayEvent(data); });

  // Set up event handler for reboot
  this->System->Events->addEventListener(EVENT_REBOOT,
                                         [this](void *data)
                                         { this->handleRebootEvent(data); });

  this->System->Events->addEventListener(EVENT_NETWORK_CONNECTED,
                                         [this](void *data)
                                         { this->handleNetworkConnectedEvent(data); });

  /** Firmware events */
  this->System->Events->addEventListener(EVENT_DOWNLOAD_UI_COMPONENTS,
                                         [this](void *data)
                                         { this->handleDownloadUIComponentsEvent(data); });
}

bool ESPAPP_Firmware::initializeNetwork(void)
{
  bool success = false;
  success = this->API->Network->init();
  this->API->Network->listener();

  return success;
}

void ESPAPP_Firmware::coreListener(void)
{
  this->API->Network->listener();

  // Check for time-based events (minute, hour, day triggers)
  this->checkTimeEvents();

  // Check for scheduled events

  time_t currentTime = time(nullptr);
  this->System->Events->checkScheduledEvents(currentTime);
}

void ESPAPP_Firmware::checkTimeEvents()
{

  time_t now = time(nullptr);
  struct tm *currentTime = localtime(&now);

  if (currentTime->tm_min != this->System->Time->lastTimeInfo->minute)
  {
    this->System->Time->lastTimeInfo->minute = currentTime->tm_min;
    this->System->Events->triggerEvent(EVENT_TIMER_MINUTE);

    if (currentTime->tm_hour != this->System->Time->lastTimeInfo->hour)
    {
      this->System->Time->lastTimeInfo->hour = currentTime->tm_hour;
      this->System->Events->triggerEvent(EVENT_TIMER_HOUR);

      if (currentTime->tm_mday != this->System->Time->lastTimeInfo->day)
      {
        this->System->Time->lastTimeInfo->day = currentTime->tm_mday;
        this->System->Events->triggerEvent(EVENT_TIMER_DAY);
      }
    }
  }
}

// Event handlers implementation
void ESPAPP_Firmware::handleMinuteEvent(void *data)
{
  // Empty implementation - to be overridden by derived classes if needed
#ifdef DEBUG
  this->System->Msg->printInformation(F("Minute event handled: "), F("SYSTEM EVENT"));
  char buffer[50];
  this->System->Time->getCurrentTimeFormatted(buffer, sizeof(buffer));
  this->System->Msg->printValue(buffer);
#endif
}

void ESPAPP_Firmware::handleHourEvent(void *data)
{
  // Empty implementation - to be overridden by derived classes if needed
#ifdef DEBUG
  this->System->Msg->printInformation(F("Hour event handled"), F("SYSTEM EVENT"));
  char buffer[50];
  this->System->Time->getCurrentTimeFormatted(buffer, sizeof(buffer));
  this->System->Msg->printBulletPoint(buffer);

#endif
}

void ESPAPP_Firmware::handleDayEvent(void *data)
{
  // Empty implementation - to be overridden by derived classes if needed
#ifdef DEBUG
  this->System->Msg->printInformation(F("Day event handled"), F("SYSTEM EVENT"));
  char buffer[50];
  this->System->Time->getCurrentTimeFormatted(buffer, sizeof(buffer));
  this->System->Msg->printBulletPoint(buffer);

#endif
}

void ESPAPP_Firmware::handleRebootEvent(void *data)
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Reboot event handled"), F("SYSTEM EVENT"));
#endif

  this->System->Events->addEventListener(EVENT_CUSTOM_START + 1,
                                         [this](void *)
                                         { this->System->reboot(); });

  uint16_t eventId1 = this->System->Events->scheduleEventIn(EVENT_CUSTOM_START + 1, 3);
}

void ESPAPP_Firmware::handleNetworkConnectedEvent(void *data)
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Network connected event handle"), F("SYSTEM EVENT"));
#endif
  this->System->Events->triggerEvent(EVENT_SYNC_TIME);
}

void ESPAPP_Firmware::handleDownloadUIComponentsEvent(void *data)
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Download UI components event handled"), F("SYSTEM EVENT"));
#endif

  this->System->Events->addEventListener(EVENT_CUSTOM_START + 2,
                                         [this](void *)
                                         { 
                                        
                                          ESPAPP_FirmwareInstalator *Instalator = new ESPAPP_FirmwareInstalator(System);
                                          InstallationStats stats;
                                          Instalator->install("http://files.smartnydom.pl/espapp/config/ui-configuration.json", stats);
                                          delete Instalator;


                                        });

  uint16_t eventId1 = this->System->Events->scheduleEventIn(EVENT_CUSTOM_START + 2, 3);


}