#ifndef _ACS758_h
#define _ACS758_h

#include <ESPAPP_Core.h>

// Forward declaration of the ESPAPP_Core class
class ESPAPP_Core;

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED

#include <ADS1115.h>

class ACS758 {
private:
  ACS758_CONFIG *config = new ACS758_CONFIG; // Configuration structure for the sensor
  ESPAPP_Core *System;
  ADS1115 *ads1115 = nullptr;               // Reference to ADS1115 instance

  bool successfullyInitialized = false;     // Flag to check if the sensor is initialized
  
  float currentReading = 0;                 // Last calculated current value
  float lastVoltageReading = 0;             // Last raw voltage reading
  float _tempVoltage = 0;                   // Temporary variable for voltage reading
  uint16_t scheduledEventId = 0;            // ID of the scheduled event for measurements
  uint32_t lastReadTime = 0;                // Last time the sensor was read
  bool isBidirectional = false;
  float sensitivity = 0.0;                  // Sensitivity in mV/A based on model
  float zeroPoint = 0;                      // Zero point
  ADS1115_CHANNEL adcChannel;               // ADC channel for the sensor

  // Helper methods
  float getSensitivity();                   // Get sensitivity in mV/A based on model
  void readRawVoltage();                    // Read raw voltage from sensor
  void takeReading();                       // Take a reading from sensor
  

public:

  ACS758(ESPAPP_Core *_system, ADS1115 *_ads1115);

  ~ACS758();
  
  // Initialization
  bool init();
  // Measurement methods
  float getCurrentInAmps();                 // Get the current value in amps
  float getRawVoltage();                    // Get the last raw voltage reading
  
  // Getters and setters for configuration parameters
  ACS758_MODEL getSensorModel();
  void setSensorModel(ACS758_MODEL model);
  
  // Start/stop monitoring
  bool startMonitoring();
  bool stopMonitoring();
  
  // Manual calibration
  bool calibrateZeroPoint();                // Calibrate zero current reference point
};

#endif // ESPAPP_HARDWARE_ACS758_INCLUDED
#endif // _ACS758_h