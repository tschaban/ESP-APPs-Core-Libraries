#ifndef _ESPAPP_PARAMETERS_HARDWARE_H
#define _ESPAPP_PARAMETERS_HARDWARE_H

/**
 * Hardware module configuration
 * Uncomment to enable specific hardware modules
 */

// Enable ACS758 Current Sensor
#define ESPAPP_HARDWARE_ACS758_INCLUDED

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
    uint16_t adcResolution;   // ADC resolution in bits (e.g., 10, 12, etc.)
    float zeroPoint;          // Zero current voltage (midpoint for bidirectional)
    float calibration;        // Calibration factor (fine adjustment)
    uint16_t readInterval;    // Read interval in milliseconds
    uint8_t samplesCount;     // Number of samples to average
    bool enabled;             // Whether the sensor is enabled
    uint16_t eventId;         // Custom event ID for current measurements
};

#endif // _ESPAPP_PARAMETERS_HARDWARE_H