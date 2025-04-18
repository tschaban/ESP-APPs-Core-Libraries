#include "ESPAPP_Hardware.h"

ESPAPP_Hardware::ESPAPP_Hardware(ESPAPP_Core *_System)
{
    this->System = _System;

#ifdef ESPAPP_HARDWARE_I2C_INCLUDED
    this->I2C = new ESPAPP_I2C_Container(this->System);
#endif

#ifdef ESPAPP_HARDWARE_ADS1115_INCLUDED
    this->ADC = new ADS1115(this->System, this->I2C);
#endif


#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
    this->ACS758Sensor = new ACS758(this->System, this->ADC);
#endif
}

ESPAPP_Hardware::~ESPAPP_Hardware()
{
}

bool ESPAPP_Hardware::init()
{
    bool success = true;

#ifdef ESPAPP_HARDWARE_I2C_INCLUDED
    success = this->I2C->init();
#endif

#ifdef ESPAPP_HARDWARE_ADS1115_INCLUDED
    success = this->ADC->init();
#endif


#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED

    success = this->ACS758Sensor->init();

#endif

    return success;
}

void ESPAPP_Hardware::listener(void)
{
}
