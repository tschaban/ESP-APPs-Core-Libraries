#ifndef _ESPAPP_HARDWARE_H
#define _ESPAPP_HARDWARE_H

#include <ESPAPP_Core.h>
#include <ESPAPP_Parameters_Hardware.h>

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED
#include <ACS758.h>
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

    bool init();

};

#endif