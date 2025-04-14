#ifndef _ACS758_h
#define _ACS758_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAPP_Parameters.h>
#include <ESPAPP_Core.h>
#include <ESPAPP_EventManager.h>

// Forward declaration of the ESPAPP_Core class
class ESPAPP_Core;

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED


class ACS758 {
private:
  ACS758_CONFIG config;
  ESPAPP_Core *System;
  
  float currentReading;        // Last calculated current value
  float lastVoltageReading;    // Last raw voltage reading
  uint16_t scheduledEventId;   // ID of the scheduled event for measurements
  uint32_t lastReadTime;       // Last time the sensor was read
  
  // Helper methods
  float getSensitivity();      // Get sensitivity in mV/A based on model
  float readRawVoltage();      // Read raw voltage from sensor
  void takeReading();          // Take a reading from sensor
  
  // Event callback methods
  static void readSensorCallback(void* instance);

public:
#ifdef DEBUG
  ACS758(ESPAPP_Core *_system, uint8_t analogPin = 34);
#else
  ACS758(ESPAPP_Core *_system, uint8_t analogPin = 34);
#endif
  ~ACS758();
  
  // Initialization
  bool init();
  
  // Configuration management
  bool loadConfiguration();
  bool saveConfiguration();
  bool resetConfiguration();
  
  // Measurement methods
  float getCurrentInAmps();   // Get the current value in amps
  float getRawVoltage();      // Get the last raw voltage reading
  
  // Getters and setters for configuration parameters
  ACS758_MODEL getSensorModel();
  void setSensorModel(ACS758_MODEL model);
  
  uint8_t getAnalogPin();
  void setAnalogPin(uint8_t pin);
  
  float getVRef();
  void setVRef(float vRef);
  
  uint16_t getAdcResolution();
  void setAdcResolution(uint16_t resolution);
  
  float getZeroPoint();
  void setZeroPoint(float zero);
  
  float getCalibration();
  void setCalibration(float factor);
  
  uint16_t getReadInterval();
  void setReadInterval(uint16_t interval);
  
  uint8_t getSamplesCount();
  void setSamplesCount(uint8_t count);
  
  bool isEnabled();
  void setEnabled(bool enabled);
  
  uint16_t getEventId();
  void setEventId(uint16_t id);
  
  // Start/stop monitoring
  bool startMonitoring();
  bool stopMonitoring();
  
  // Manual calibration
  bool calibrateZeroPoint(); // Calibrate zero current reference point
};

#endif // ESPAPP_HARDWARE_ACS758_INCLUDED
#endif // _ACS758_h