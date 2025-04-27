#include <ESPAPP_Core.h>

ESPAPP_Core::ESPAPP_Core()
{

#ifdef DEBUG
  this->Events = new ESPAPP_EventManager(this->Debugger);
  this->Time = new ESPAPP_Time(this->Flash, this->Debugger);
#else
  this->Time = new ESPAPP_Time(this->Flash);
#endif
}
ESPAPP_Core::~ESPAPP_Core() {}

bool ESPAPP_Core::init(void)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Initializing: firmware kernel"), F("CORE"));
#endif

  bool success = this->Flash->init();

  if (success)
  {
    success = this->File->read(connectionMode);

    if (success)
    {
      success = this->File->read(operatingMode);
    }

    this->Time->init();
    
    /** Deleting all files from temporary files folder */
    this->Flash->deleteAllFilesInDirectory(FPSTR(path_temp));
    
  }
  return success;
}

void ESPAPP_Core::reboot()
{
  yield();
#ifdef DEBUG
  this->Debugger->printHeader(2);
  this->Debugger->printValue(F("Rebooting device"));
  this->Debugger->printHeader(1, 1);
#endif
  delay(100);
  ESP.restart();
}

ESPAPP_NETWORK_CONNECTION_MODE ESPAPP_Core::getConnectionMode(void)
{
  return *this->connectionMode;
}

bool ESPAPP_Core::setConnectionMode(ESPAPP_NETWORK_CONNECTION_MODE mode)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Setting connection mode"), F("KERNEL"));
  this->Debugger->printBulletPoint(F("New mode: "));
  this->Debugger->printValue(mode);
#endif

  if (*this->connectionMode == mode)
  {
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Mode unchanged, skipping save"));
#endif
    return true; // No change needed, return success
  }

  *this->connectionMode = mode;
  return this->File->save(this->connectionMode);
}

ESPAPP_OPERATING_MODE ESPAPP_Core::getOperatingMode(void)
{
  return *this->operatingMode;
}

bool ESPAPP_Core::setOperatingMode(ESPAPP_OPERATING_MODE mode)
{
#ifdef DEBUG
  this->Debugger->printInformation(F("Setting Operating mode"), F("KERNEL"));
  this->Debugger->printBulletPoint(F("New mode: "));
  this->Debugger->printValue(mode);
#endif

  if (*this->operatingMode == mode)
  {
#ifdef DEBUG
    this->Debugger->printBulletPoint(F("Mode unchanged, skipping save"));
#endif
    return true; // No change needed, return success
  }

  *this->operatingMode = mode;
  return this->File->save(this->operatingMode);
}

void ESPAPP_Core::getDeviceID(char *id, boolean extended)
{
  byte m[6];
  WiFi.macAddress(m);
  sprintf(id, "%s%x%x%x%x-%x%x%x%x",
          (extended ? (PGM_P)F(ESPAPP_DEVICE_ID_PREFIX) : (PGM_P)F(ESPAPP_EMPTY_STRING)), m[0],
          m[5], m[1], m[4], m[2], m[3], m[3], m[2]);
}