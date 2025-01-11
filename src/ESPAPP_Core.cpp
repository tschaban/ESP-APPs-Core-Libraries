#include <ESPAPP_Core.h>

ESPAPP_Core::ESPAPP_Core() {}
ESPAPP_Core::~ESPAPP_Core() {}


void ESPAPP_Core::reboot(uint8_t mode)
{
 //@TODO: Implement this function
 // saveMode(mode);
 // yield();
#ifdef DEBUG
  Msg->printHeader(2);
  Msg->printValue(F("Rebooting device in 1sec"));
  Msg->printHeader(1, 1);
#endif
  delay(1000);
  ESP.restart();
}

uint8_t ESPAPP_Core::connectionMode() { return ESP_APP_NETWORK_CONNECTION_MODE_CLIENT; }
