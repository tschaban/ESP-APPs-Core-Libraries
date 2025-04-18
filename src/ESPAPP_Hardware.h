#ifndef _ESPAPP_HARDWARE_H
#define _ESPAPP_HARDWARE_H

#include <ESPAPP_Core.h>
#include <ESPAPP_Parameters_Hardware.h>

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
#include <ACS758.h>
#endif

#ifdef ESPAPP_HARDWARE_ADS1115_INCLUDED
#include <ADS1115.h>
#endif

#ifdef ESPAPP_HARDWARE_I2C_INCLUDED
#include <ESPAPP_I2C_Container.h>
#endif

class ESPAPP_Hardware
{

private:
    ESPAPP_Core *System = nullptr;

public:
    ESPAPP_Hardware(ESPAPP_Core *_System);
    ~ESPAPP_Hardware();

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
    ACS758 *ACS758Sensor = nullptr;
#endif

#ifdef ESPAPP_HARDWARE_I2C_INCLUDED
    ESPAPP_I2C_Container *I2C = nullptr;
#endif

#ifdef ESPAPP_HARDWARE_ADS1115_INCLUDED
    ADS1115 *ADC = nullptr;
#endif

    bool init();
    void listener(void);

};

#endif