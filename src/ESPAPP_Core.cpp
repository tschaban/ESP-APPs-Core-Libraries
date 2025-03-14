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

void ESPAPP_Core::reboot(uint8_t mode)
{
  //@TODO: Implement this function
  // saveMode(mode);
  yield();
#ifdef DEBUG
  this->Msg->printHeader(2);
  this->Msg->printValue(F("Rebooting device"));
  this->Msg->printHeader(1, 1);
#endif
  ESP.restart();
}

uint8_t ESPAPP_Core::connectionMode() { return ESPAPP_NETWORK_CONNECTION_MODE_CLIENT; }
