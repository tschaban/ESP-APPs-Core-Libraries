#include "ESPAPP_I2C_Container.h"

#ifdef ESPAPP_HARDWARE_I2C_INCLUDED

ESPAPP_I2C_Container::ESPAPP_I2C_Container(ESPAPP_Core *_System)
{
  this->System = _System;
  this->I2CBUSConfiguration->SDA = 18;
  this->I2CBUSConfiguration->SCL = 17;
  this->I2CBUSConfiguration->frequency = 100000;
  this->Scanner = new ESPAPP_I2C_Scanner(this->System);
}

bool ESPAPP_I2C_Container::init(void)
{

  #ifdef DEBUG
  this->System->Debugger->printInformation(F("Initializing I2C bus"), F("I2C"));
  this->System->Debugger->printBulletPoint(F("SDA: "));
  this->System->Debugger->printValue(this->I2CBUSConfiguration->SDA);
  this->System->Debugger->printBulletPoint(F("SCL: "));
  this->System->Debugger->printValue(this->I2CBUSConfiguration->SCL);
  this->System->Debugger->printBulletPoint(F("Frequency: "));
  this->System->Debugger->printValue(this->I2CBUSConfiguration->frequency);  
  #endif

  this->successfullyInitialized = Port0->begin(this->I2CBUSConfiguration->SDA, this->I2CBUSConfiguration->SCL,
                                               this->I2CBUSConfiguration->frequency);
#ifdef DEBUG
  if (!this->successfullyInitialized)
  {
    this->System->Debugger->printWarning(F("Doesn't work"), F("I2C[0]"));
  }
  else
  {
    this->Scanner->scanAll(Port0);
  }
#endif // DEBUG

  // @TODO add 2nd I2C port for ESP32
  return this->successfullyInitialized;
}

#endif // ESPAPP_HARDWARE_I2C_INCLUDED