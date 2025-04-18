#ifndef _ESPAPP_PARAMETERS_HARDWARE_H
#define _ESPAPP_PARAMETERS_HARDWARE_H

/**
 * Hardware module configuration
 * Uncomment to enable specific hardware modules
 */

 // Enabled Hardwares
#define ESPAPP_HARDWARE_I2C_INCLUDED
#define ESPAPP_HARDWARE_ADS1115_INCLUDED
#define ESPAPP_HARDWARE_ACS758_INCLUDED



#ifdef ESPAPP_HARDWARE_I2C_INCLUDED
struct I2CPORT_CONFIG {
    uint8_t SDA;
    uint8_t SCL;
  #ifdef ESP32  
    uint32_t frequency;
  #endif
};
#endif // ESPAPP_HARDWARE_I2C_INCLUDED



#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED

#define ESPAPP_HARDWARE_ACS758_VOLTAGE_FOR_NOMINAL_SENSITIVITY 5.0f // Nominal voltage for sensitivity calculation (5V or 3.3V)

// ACS758 sensor models with their sensitivity values in mV/A
enum ACS758_MODEL
{
    ACS758_50B = 0,  // ±50A with 40 mV/A sensitivity
    ACS758_100B = 1, // ±100A with 20 mV/A sensitivity
    ACS758_150B = 2, // ±150A with 13.3 mV/A sensitivity
    ACS758_200B = 3, // ±200A with 10 mV/A sensitivity
    ACS758_50U = 4,  // 0-50A with 60 mV/A sensitivity
    ACS758_100U = 5, // 0-100A with 40 mV/A sensitivity
    ACS758_150U = 6, // 0-150A with 26.7 mV/A sensitivity
    ACS758_200U = 7  // 0-200A with 20 mV/A sensitivity
};

struct ACS758_CONFIG
{
    ACS758_MODEL sensorModel; // Sensor model
    uint8_t analogPin;        // Analog pin to read from
    float vRef;               // Reference voltage (typically 3.3V or 5V)
    uint8_t adcResolution;   // ADC resolution in bits (e.g., 10, 12, etc.)
    float zeroPoint;          // Zero current voltage (midpoint for bidirectional)
    float calibration;        // Calibration factor (fine adjustment)
    uint16_t readInterval;    // Read interval in milliseconds
    uint8_t samplesCount;     // Number of samples to average
};


#endif  // ESPAPP_HARDWARE_ACS758_INCLUDED

#endif // _ESPAPP_PARAMETERS_HARDWARE_H