
#ifndef _ESPAPP_I2C_Container_h
#define _ESPAPP_I2C_Container_h

#include <ESPAPP_Parameters.h>
#ifdef ESPAPP_HARDWARE_I2C_INCLUDED


#include <ESPAPP_I2C_Scanner.h>
#include <Wire.h>

class ESPAPP_I2C_Container {

private:

  ESPAPP_Core *System = nullptr;
  I2CPORT_CONFIG *I2CBUSConfiguration = new I2CPORT_CONFIG;
  bool successfullyInitialized = false;


public:

  TwoWire *Port0 = new TwoWire(0);
  TwoWire *Port1 = new TwoWire(1);
  ESPAPP_I2C_Scanner *Scanner = nullptr;

  ESPAPP_I2C_Container(ESPAPP_Core *_System);

  bool init(void);
  

};

#endif
#endif