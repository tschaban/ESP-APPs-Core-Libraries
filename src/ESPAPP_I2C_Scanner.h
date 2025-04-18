/* ESPAPP Firmware for smarthome devices, More info: https://ESPAPP.smartnydom.pl/ */

#ifndef _ESPAPP_I2C_Scanner_h
#define _ESPAPP_I2C_Scanner_h

#include <ESPAPP_Parameters.h>
#ifdef ESPAPP_HARDWARE_I2C_INCLUDED
#include <ESPAPP_Core.h>
#include <Wire.h>

class ESPAPP_I2C_Scanner
{

private:
  boolean portSet = false;
  TwoWire *WirePort;
  ESPAPP_Core *System = nullptr;

public:
#ifdef DEBUG
  ESPAPP_I2C_Scanner(ESPAPP_Core * _System);

#else
  ESPAPP_I2C_Scanner(ESPAPP_Core * _System);
  ();
#endif

  void setWire(TwoWire * _Wire);

#ifdef DEBUG
  /**
   * @brief Method only call in the debug mode. It scans all adesses
   *
   */

  void scanAll();
  void scanAll(TwoWire * _Wire);
#endif

  bool scan(byte address);
  bool scan(TwoWire * _Wire, byte address);

  const __FlashStringHelper *getName(byte deviceAddress);
};

#endif // ESPAPP_CONFIG_HARDWARE_I2C
#endif // _ESPAPP_I2C_Scanner_h
