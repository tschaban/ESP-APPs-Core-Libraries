#include "ACS758.h"

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED

ACS758::ACS758(ESPAPP_Core *_System, uint8_t analogPin)
{
  this->System = _System;

  // Set default values for config (will be overridden by loadConfiguration)
  this->config.analogPin = analogPin;
  this->config.sensorModel = ACS758_100B;
  this->config.vRef = 3.3;
  this->config.adcResolution = 12;
  this->config.zeroPoint = this->config.vRef / 2.0;
  this->config.calibration = 1.0;
  this->config.readInterval = 1000;
  this->config.samplesCount = 10;
  this->config.enabled = false;
  this->config.eventId = EVENT_CUSTOM_START + 50;

  this->currentReading = 0.0;
  this->lastVoltageReading = 0.0;
  this->scheduledEventId = 0;
  this->lastReadTime = 0;
}

ACS758::~ACS758()
{
  // Stop monitoring if active
  this->stopMonitoring();
}

bool ACS758::init()
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Initializing ACS758 sensor"), F("ACS758"));
#endif

  // Try to load configuration from file
  bool success = this->loadConfiguration();

  // If loading failed, save default configuration
  if (!success)
  {
#ifdef DEBUG
    this->System->Msg->printWarning(F("Failed to load configuration, using defaults"), F("ACS758"));
#endif
    success = this->saveConfiguration();
  }

  // Set up the ADC pin
  pinMode(this->config.analogPin, INPUT);

  // Start monitoring if enabled
  if (this->config.enabled)
  {
    success = this->startMonitoring();
  }

  return success;
}

bool ACS758::loadConfiguration()
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Loading ACS758 configuration"), F("ACS758"));
#endif

  return this->System->File->read(&this->config);
}

bool ACS758::saveConfiguration()
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Saving ACS758 configuration"), F("ACS758"));
#endif

  return this->System->File->save(&this->config);
}

bool ACS758::resetConfiguration()
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Resetting ACS758 configuration to defaults"), F("ACS758"));
#endif

  // Stop monitoring if active
  this->stopMonitoring();

  // Reset to default values using ESPAPP_API_Files
  bool success = this->System->File->resetToDefault(&this->config);

  // Save the default configuration
  return success;
}

float ACS758::getSensitivity()
{
  // Return sensitivity in mV/A based on model
  switch (this->config.sensorModel)
  {
  case ACS758_50B:
    return 40.0;
  case ACS758_100B:
    return 20.0;
  case ACS758_150B:
    return 13.3;
  case ACS758_200B:
    return 10.0;
  case ACS758_50U:
    return 60.0;
  case ACS758_100U:
    return 40.0;
  case ACS758_150U:
    return 26.7;
  case ACS758_200U:
    return 20.0;
  default:
    return 20.0; // Default to 100B model
  }
}

float ACS758::readRawVoltage()
{
  // Read multiple samples and average
  float total = 0.0;
  for (int i = 0; i < this->config.samplesCount; i++)
  {
    // Read raw value and convert to voltage
    int rawValue = analogRead(this->config.analogPin);
    float voltage = (float)rawValue * this->config.vRef / (float)((1 << this->config.adcResolution) - 1);
    total += voltage;
    delay(1); // Small delay between readings
  }

  // Store and return the average voltage
  this->lastVoltageReading = total / (float)this->config.samplesCount;
  return this->lastVoltageReading;
}

void ACS758::takeReading()
{
  // Read raw voltage
  float voltage = this->readRawVoltage();

  // Calculate current based on sensor type
  bool isBidirectional = this->config.sensorModel <= ACS758_200B;
  float sensitivity = this->getSensitivity() / 1000.0; // Convert from mV/A to V/A

  if (isBidirectional)
  {
    // For bidirectional sensors, current is proportional to voltage difference from zero point
    this->currentReading = (voltage - this->config.zeroPoint) / sensitivity * this->config.calibration;
  }
  else
  {
    // For unidirectional sensors, current is directly proportional to voltage
    this->currentReading = voltage / sensitivity * this->config.calibration;
  }

  this->lastReadTime = millis();

  // Trigger the custom event with the current reading as data
  float *currentData = new float(this->currentReading);
  this->System->Events->triggerEvent(this->config.eventId, (void *)currentData);

#ifdef DEBUG
  if (this->System->Events->hasListeners(this->config.eventId))
  {
    this->System->Msg->printInformation(F("Current reading event triggered"), F("ACS758"));
    this->System->Msg->printBulletPoint(F("Current: "));
    this->System->Msg->printValue(this->currentReading);
    this->System->Msg->printValue(F(" A"));
  }
#endif
}

// Static callback for event handling
void ACS758::readSensorCallback(void *instance)
{
  if (instance)
  {
    ACS758 *sensor = static_cast<ACS758 *>(instance);
    sensor->takeReading();
  }
}

bool ACS758::startMonitoring()
{
  if (!this->config.enabled)
  {
    return false;
  }

  // Stop any existing monitoring
  this->stopMonitoring();

#ifdef DEBUG
  this->System->Msg->printInformation(F("Starting ACS758 monitoring"), F("ACS758"));
#endif

  // Schedule the event to take readings at the configured interval
  this->scheduledEventId = this->System->Events->scheduleEventIn(
      EVENT_CUSTOM_START + 51, // Use a different event ID for the schedule
      1,                       // Start in 1 second
      SCHEDULE_REPEAT,
      this->config.readInterval / 1000, // Convert ms to seconds
      this);

  // Add listener for the scheduled event
  this->System->Events->addEventListener(
      EVENT_CUSTOM_START + 51,
      [this](void *data)
      {
        this->takeReading();
      });

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Monitoring started with interval: "));
  this->System->Msg->printValue(this->config.readInterval);
  this->System->Msg->printValue(F(" ms"));
#endif

  return true;
}

bool ACS758::stopMonitoring()
{
  if (this->scheduledEventId > 0)
  {
#ifdef DEBUG
    this->System->Msg->printInformation(F("Stopping ACS758 monitoring"), F("ACS758"));
#endif

    // Cancel the scheduled event
    bool success = this->System->Events->cancelScheduledEvent(this->scheduledEventId);

    // Remove the event listener
    this->System->Events->removeEventListeners(EVENT_CUSTOM_START + 51);

    this->scheduledEventId = 0;
    return success;
  }
  return true;
}

bool ACS758::calibrateZeroPoint()
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Calibrating ACS758 zero point"), F("ACS758"));
#endif

  // Ensure no current is flowing through the sensor before calling this method
  float total = 0.0;
  const int samples = 50; // Use more samples for better accuracy

  for (int i = 0; i < samples; i++)
  {
    int rawValue = analogRead(this->config.analogPin);
    float voltage = (float)rawValue * this->config.vRef / (float)((1 << this->config.adcResolution) - 1);
    total += voltage;
    delay(10); // Longer delay for stability
  }

  // Set the new zero point
  this->config.zeroPoint = total / (float)samples;

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("New zero point: "));
  this->System->Msg->printValue(this->config.zeroPoint);
  this->System->Msg->printValue(F(" V"));
#endif

  // Save the configuration
  return this->saveConfiguration();
}

// Measurement methods
float ACS758::getCurrentInAmps()
{
  // If monitoring is not active or it's been too long since last reading,
  // take a new reading immediately
  if (this->scheduledEventId == 0 || (millis() - this->lastReadTime > this->config.readInterval * 2))
  {
    this->takeReading();
  }

  return this->currentReading;
}

float ACS758::getRawVoltage()
{
  return this->lastVoltageReading;
}

// Getters and setters
ACS758_MODEL ACS758::getSensorModel()
{
  return this->config.sensorModel;
}

void ACS758::setSensorModel(ACS758_MODEL model)
{
  this->config.sensorModel = model;

  // Update zero point for bidirectional sensors
  if (model <= ACS758_200B)
  {
    this->config.zeroPoint = this->config.vRef / 2.0; // Default midpoint
  }
  else
  {
    this->config.zeroPoint = 0.0; // Zero point for unidirectional
  }
}

uint8_t ACS758::getAnalogPin()
{
  return this->config.analogPin;
}

void ACS758::setAnalogPin(uint8_t pin)
{
  this->stopMonitoring();
  this->config.analogPin = pin;
  pinMode(this->config.analogPin, INPUT);
  if (this->config.enabled)
  {
    this->startMonitoring();
  }
}

float ACS758::getVRef()
{
  return this->config.vRef;
}

void ACS758::setVRef(float vRef)
{
  this->config.vRef = vRef;
}

uint16_t ACS758::getAdcResolution()
{
  return this->config.adcResolution;
}

void ACS758::setAdcResolution(uint16_t resolution)
{
  this->config.adcResolution = resolution;
}

float ACS758::getZeroPoint()
{
  return this->config.zeroPoint;
}

void ACS758::setZeroPoint(float zero)
{
  this->config.zeroPoint = zero;
}

float ACS758::getCalibration()
{
  return this->config.calibration;
}

void ACS758::setCalibration(float factor)
{
  this->config.calibration = factor;
}

uint16_t ACS758::getReadInterval()
{
  return this->config.readInterval;
}

void ACS758::setReadInterval(uint16_t interval)
{
  bool wasEnabled = this->config.enabled;

  if (wasEnabled)
  {
    this->stopMonitoring();
  }

  this->config.readInterval = interval;

  if (wasEnabled)
  {
    this->startMonitoring();
  }
}

uint8_t ACS758::getSamplesCount()
{
  return this->config.samplesCount;
}

void ACS758::setSamplesCount(uint8_t count)
{
  this->config.samplesCount = count;
}

bool ACS758::isEnabled()
{
  return this->config.enabled;
}

void ACS758::setEnabled(bool enabled)
{
  if (this->config.enabled != enabled)
  {
    this->config.enabled = enabled;

    if (enabled)
    {
      this->startMonitoring();
    }
    else
    {
      this->stopMonitoring();
    }
  }
}

uint16_t ACS758::getEventId()
{
  return this->config.eventId;
}

void ACS758::setEventId(uint16_t id)
{
  this->config.eventId = id;
}

#endif // ESPAPP_HARDWARE_ACS758_INCLUDED