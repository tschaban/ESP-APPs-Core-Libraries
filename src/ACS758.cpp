#include "ACS758.h"

#ifdef ESPAPP_HARDWARE_ACS758_INCLUDED

ACS758::ACS758(ESPAPP_Core *_System, ADS1115 *_ads1115)
{
  this->System = _System;
  this->ads1115 = _ads1115;
  this->adcChannel = ADS1115_CHANNEL::ADS1115_CHANNEL_0; // @TODO Hardcoded;
}

ACS758::~ACS758()
{
  // Stop monitoring if active
  this->stopMonitoring();
}



bool ACS758::init()
{
  // Check if ADS1115 is available
  if (!this->ads1115) {
#ifdef DEBUG
    this->System->Msg->printError(F("ADS1115 not available"), F("ACS758"));
#endif
    return false;
  }

  this->successfullyInitialized = this->System->File->read(this->config);

  if (this->successfullyInitialized)
  {
    /** Configuring */
    this->isBidirectional = this->config->sensorModel <= ACS758_200B;
    this->sensitivity = (this->getSensitivity() / 1000.0); 
    this->zeroPoint = this->config->vRef / 2;
    this->successfullyInitialized = this->startMonitoring();
  }

#ifdef DEBUG
  if (this->successfullyInitialized)
  {
    this->System->Msg->printBulletPoint(F("Bidirectional: "));
    this->System->Msg->printValue(this->isBidirectional);
   
    this->System->Msg->printBulletPoint(F("Nominal sensitivity: "));
    this->System->Msg->printValue(this->getSensitivity());
    this->System->Msg->printValue(F(" mV/A"));

    this->System->Msg->printBulletPoint(F("Actual sensitivity: "));
    this->System->Msg->printValue(this->sensitivity);
    this->System->Msg->printValue(F(" mV/A"));

    this->System->Msg->printBulletPoint(F("Calibration: "));
    this->System->Msg->printValue(this->config->calibration);
    this->System->Msg->printValue(F(" A/V"));

    this->System->Msg->printBulletPoint(F("Zero point: "));
    this->System->Msg->printValue(this->config->zeroPoint);
    this->System->Msg->printValue(F(" V"));

    if (this->config && this->config != nullptr)
    {
      this->System->Msg->printBulletPoint(F("Sensor model: "));
      this->System->Msg->printValue(this->config->sensorModel);
    }
    
    this->System->Msg->printBulletPoint(F("ADC Channel: "));
    this->System->Msg->printValue((int)this->adcChannel);
   
    this->System->Msg->printInformation(F("ACS758 sensor initialized successfully"), F("ACS758"));
  }
  else
  {
    this->System->Msg->printError(F("Failed to initialize ACS758 sensor"), F("ACS758"));
  }
#endif

  return this->successfullyInitialized;
}

float ACS758::getSensitivity()
{
  // Return sensitivity in mV/A based on model
  // Nominal Sensitivity at 5V (mV/A)
  switch (this->config->sensorModel)
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

void ACS758::readRawVoltage()
{
  if (!this->ads1115) {
#ifdef DEBUG
    this->System->Msg->printError(F("ADS1115 not available for reading"), F("ACS758"));
#endif
    return;
  }

  this->lastVoltageReading = 0;

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Reading from ADS1115 channel: "));
  this->System->Msg->printValue((int)this->adcChannel);
#endif

  // Read voltage from ADS1115
  for (uint8_t i = 0; i < this->config->samplesCount; i++)
  {
    this->_tempVoltage = this->ads1115->getVoltage(this->adcChannel);
    this->lastVoltageReading += this->_tempVoltage;
    delay(1); // Small delay between readings
  }
  
  // Average the readings
  this->lastVoltageReading = this->lastVoltageReading / (float)this->config->samplesCount;

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Raw voltage reading: "));
  this->System->Msg->printValue(this->lastVoltageReading);
  this->System->Msg->printValue(F(" V"));
#endif
}

void ACS758::takeReading()
{
#ifdef DEBUG
  this->System->Msg->printInformation(F("Taking reading"), F("ACS758"));
#endif

  // Read raw voltage
  this->readRawVoltage();

  if (this->isBidirectional)
  {
    // For bidirectional sensors, current is proportional to voltage difference from zero point
    this->currentReading = (this->lastVoltageReading - this->zeroPoint) / this->sensitivity * this->config->calibration;
  }
  else
  {
    // For unidirectional sensors, current is directly proportional to voltage
    this->currentReading = this->lastVoltageReading / this->sensitivity * this->config->calibration;
  }

  this->lastReadTime = millis();

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Voltage: "));
  this->System->Msg->printValue(this->lastVoltageReading);
  this->System->Msg->printValue(F(" V"));
  this->System->Msg->printBulletPoint(F("Current: "));
  this->System->Msg->printValue(this->currentReading);
  this->System->Msg->printValue(F(" A"));
#endif
}


bool ACS758::startMonitoring()
{
  if (!this->successfullyInitialized || !this->ads1115)
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
      EVENT_HARDWARE_ACS758,
      this->config->readInterval,
      SCHEDULE_REPEAT,
      this->config->readInterval,
      this);

  // Add listener for the scheduled event
  this->System->Events->addEventListener(
      EVENT_HARDWARE_ACS758,
      [this](void *data)
      {
        this->takeReading();
      });

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("Monitoring started with interval: "));
  this->System->Msg->printValue(this->config->readInterval);
  this->System->Msg->printValue(F("s"));
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
    this->System->Events->removeEventListeners(EVENT_HARDWARE_ACS758);

    this->scheduledEventId = 0;
    return success;
  }
  return true;
}

bool ACS758::calibrateZeroPoint()
{
  if (!this->ads1115) {
#ifdef DEBUG
    this->System->Msg->printError(F("ADS1115 not available for calibration"), F("ACS758"));
#endif
    return false;
  }

#ifdef DEBUG
  this->System->Msg->printInformation(F("Calibrating ACS758 zero point"), F("ACS758"));
#endif

  // Ensure no current is flowing through the sensor before calling this method
  float total = 0.0;
  const int samples = 50; // Use more samples for better accuracy

  for (int i = 0; i < samples; i++)
  {
    float voltage = this->ads1115->getVoltage(this->adcChannel);
    total += voltage;
    delay(10); // Longer delay for stability
  }

  // Set the new zero point
  this->config->zeroPoint = total / (float)samples;

#ifdef DEBUG
  this->System->Msg->printBulletPoint(F("New zero point: "));
  this->System->Msg->printValue(this->config->zeroPoint);
  this->System->Msg->printValue(F(" V"));
#endif

  // Save the configuration
  return this->System->File->save(this->config);
}

// Measurement methods
float ACS758::getCurrentInAmps()
{
  // If monitoring is not active or it's been too long since last reading,
  // take a new reading immediately
  if (this->scheduledEventId == 0 || (millis() - this->lastReadTime > this->config->readInterval * 2))
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
  return this->config->sensorModel;
}

void ACS758::setSensorModel(ACS758_MODEL model)
{
  this->config->sensorModel = model;

  // Update zero point for bidirectional sensors
  if (model <= ACS758_200B)
  {
    this->config->zeroPoint = this->config->vRef / 2.0; // Default midpoint for bidirectional
  }
  else
  {
    this->config->zeroPoint = 0.0; // Zero point for unidirectional
  }
}

#endif // ESPAPP_HARDWARE_ACS758_INCLUDED