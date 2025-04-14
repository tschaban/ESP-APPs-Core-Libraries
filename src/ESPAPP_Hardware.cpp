#include "ESPAPP_Hardware.h"

ESPAPP_Hardware::ESPAPP_Hardware(ESPAPP_Core *_System)
{
    this->System = _System;

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
    this->ACS758Sensor = new ACS758(this->System, 33); // Default analog pin 34
#endif

}

ESPAPP_Hardware::~ESPAPP_Hardware()
{
    // Destructor implementation (if needed)
#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
    if (this->ACS758Sensor != nullptr) {
        delete this->ACS758Sensor;
    }
#endif
}

bool ESPAPP_Hardware::init()
{
    bool success = true;
    
    // Initialize hardware components here
#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
    if (this->ACS758Sensor != nullptr) {
        success = this->ACS758Sensor->init();
    }
#endif

    return success;
}

