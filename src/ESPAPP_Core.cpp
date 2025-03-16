#include <ESPAPP_Core.h>

ESPAPP_Core::ESPAPP_Core()
{

#ifdef DEBUG
  this->Events = new ESPAPP_EventManager(this->Msg);
  this->Time = new ESPAPP_Time(this->Flash, this->Msg);
#else
  this->Time = new ESPAPP_Time(this->Flash);
#endif
}
ESPAPP_Core::~ESPAPP_Core() {}

bool ESPAPP_Core::init(void)
{
#ifdef DEBUG
  this->Msg->printInformation(F("Initializing: firmware kernel"), F("CORE"));
#endif

  bool success = this->Flash->init();

  if (success)
  {
    success = this->readConnectionModeConfiguration();
    if (!success)
    {
      success = this->createDefaultConnectionModeConfiguration();
    }
    this->Time->init();
  }
  return success;
}

void ESPAPP_Core::reboot()
{
  yield();
#ifdef DEBUG
  this->Msg->printHeader(2);
  this->Msg->printValue(F("Rebooting device"));
  this->Msg->printHeader(1, 1);
#endif
  delay(100);
  ESP.restart();
}

bool ESPAPP_Core::readConnectionModeConfiguration(void)
{
  bool success = false;

#ifdef DEBUG
  this->Msg->printInformation(F("Reading connection mode configuration"), F("WIFI"));
#endif

  StaticJsonDocument<256> doc;
  success = this->Flash->getJSON(F("/cfg/connectionMode.json"), doc);

  if (success)
  {
    this->connectionMode = doc["mode"] | ESPAPP_NETWORK_CONNECTION_MODE_NO_CONNECTION;

#ifdef DEBUG
    this->Msg->printBulletPoint(F("Connection mode: "));
    this->Msg->printValue(this->connectionMode);
#endif
  }
#ifdef DEBUG
  else
  {
    this->Msg->printError(F("Failed to load connection mode configuration"), F("WIFI"));
  }
#endif

  return success;
}

bool ESPAPP_Core::createDefaultConnectionModeConfiguration(void)
{
#ifdef DEBUG
  this->Msg->printInformation(F("Creating default connection mode configuration"), F("WIFI"));
#endif

  // Set default connection mode config values
  this->connectionMode = ESPAPP_NETWORK_CONNECTION_MODE_ACCESS_POINT;

  return saveConnectionModeConfiguration();
}

bool ESPAPP_Core::saveConnectionModeConfiguration(void)
{
#ifdef DEBUG
  this->Msg->printInformation(F("Saving connection mode configuration: "), F("WIFI"));
  this->Msg->printValue(this->connectionMode);
#endif
  StaticJsonDocument<256> doc;
  doc["mode"] = this->connectionMode;
  return this->Flash->saveJSON(F("/cfg/connectionMode.json"), doc);
}

uint8_t ESPAPP_Core::getConnectionMode(void)
{
  return this->connectionMode;
}

bool ESPAPP_Core::setConnectionMode(uint8_t mode)
{
#ifdef DEBUG
  this->Msg->printInformation(F("Setting connection mode"), F("WIFI"));
  this->Msg->printBulletPoint(F("New mode: "));
  this->Msg->printValue(mode);
#endif

  if (this->connectionMode == mode)
  {
#ifdef DEBUG
    this->Msg->printBulletPoint(F("Mode unchanged, skipping save"));
#endif
    return true; // No change needed, return success
  }

  this->connectionMode = mode;
  return saveConnectionModeConfiguration();
}
