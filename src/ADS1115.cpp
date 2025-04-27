#include "ADS1115.h"

ADS1115::ADS1115(ESPAPP_Core *_System, ESPAPP_I2C_Container *_I2C)
{
    this->System = _System;
    this->I2C = _I2C;
    this->i2cAddress = 0x48; // @TODO Hardcoded add reading configuration
    // this->gain = ADS1115_GAIN_TWO; // Default gain (±2.048V)
    this->gain = ADS1115_GAIN_TWOTHIRDS; // (±6.144V)
}

ADS1115::~ADS1115()
{
    // Cleanup if needed
}

bool ADS1115::init()
{

    this->successfullyInitialized = this->isConnected();

    if (this->successfullyInitialized)
    {
#ifdef DEBUG
        this->System->Debugger->printInformation(F("Initialized"), F("ADS1115"));
        this->System->Debugger->printBulletPoint(F("Address: 0x"));
        Serial.print(this->i2cAddress, HEX);
        Serial.println();
#endif

        // Configure with default settings
        uint16_t config = ADS1115_CFG_OS_SINGLE |       // Start single conversion
                          ADS1115_CFG_MUX_SINGLE_0 |    // Default to channel 0
                          this->gain |                  // Set to default PGA/gain value
                          ADS1115_CFG_MODE_SINGLE |     // Single-shot mode
                          ADS1115_CFG_DR_128SPS |       // 128 samples per second
                          ADS1115_CFG_COMP_QUE_DISABLE; // Disable comparator

        writeRegister(ADS1115_REG_CONFIG, config);
    }

#ifdef DEBUG
    else
    {
        this->System->Debugger->printWarning(F("Device not found"), F("ADS1115"));
    }
#endif

    return this->successfullyInitialized;
}

bool ADS1115::initalized()
{
    return this->successfullyInitialized;
}

bool ADS1115::isConnected()
{
    this->I2C->Port0->beginTransmission(this->i2cAddress);
    return (this->I2C->Port0->endTransmission() == 0);
}

void ADS1115::setGain(ADS1115_GAIN gain)
{
    this->gain = gain;
}

ADS1115_GAIN ADS1115::getGain()
{
    return this->gain;
}

float ADS1115::getMultiplier()
{
    // Convert gain setting to voltage multiplier
    switch (this->gain)
    {
    case ADS1115_GAIN_TWOTHIRDS:
        return 6.144f / 32768.0f;
    case ADS1115_GAIN_ONE:
        return 4.096f / 32768.0f;
    case ADS1115_GAIN_TWO:
        return 2.048f / 32768.0f;
    case ADS1115_GAIN_FOUR:
        return 1.024f / 32768.0f;
    case ADS1115_GAIN_EIGHT:
        return 0.512f / 32768.0f;
    case ADS1115_GAIN_SIXTEEN:
        return 0.256f / 32768.0f;
    default:
        return 2.048f / 32768.0f; // Default to GAIN_TWO
    }
}

uint16_t ADS1115::readRegister(uint8_t reg)
{
    this->I2C->Port0->beginTransmission(this->i2cAddress);
    this->I2C->Port0->write(reg);
    this->I2C->Port0->endTransmission();

    this->I2C->Port0->requestFrom(this->i2cAddress, (uint8_t)2);
    return ((this->I2C->Port0->read() << 8) | this->I2C->Port0->read());
}

void ADS1115::writeRegister(uint8_t reg, uint16_t value)
{
    this->I2C->Port0->beginTransmission(this->i2cAddress);
    this->I2C->Port0->write(reg);
    this->I2C->Port0->write((value >> 8) & 0xFF); // MSB
    this->I2C->Port0->write(value & 0xFF);        // LSB
    this->I2C->Port0->endTransmission();
}

int16_t ADS1115::readADC(ADS1115_CHANNEL channel)
{

    if (!this->successfullyInitialized)
    {
#ifdef DEBUG
        this->System->Debugger->printWarning(F("Not initialized"), F("ADS1115"));
#endif
        return 0;
    }

    // Ensure channel is valid
    if (channel > ADS1115_CHANNEL_3)
    {
#ifdef DEBUG
        this->System->Debugger->printError(F("Invalid channel"), F("ADS1115"));
#endif
        return 0;
    }

    // Configure the device for the specified channel
    uint16_t config = ADS1115_CFG_OS_SINGLE |                           // Start single conversion
                      (ADS1115_CFG_MUX_SINGLE_0 + (channel * 0x1000)) | // Select channel
                      this->gain |                                      // Set PGA/gain
                      ADS1115_CFG_MODE_SINGLE |                         // Single-shot mode
                      ADS1115_CFG_DR_128SPS |                           // 128 samples per second
                      ADS1115_CFG_COMP_QUE_DISABLE;                     // Disable comparator

    writeRegister(ADS1115_REG_CONFIG, config);

    // Wait for the conversion to complete
    delay(8); // Wait longer than 1/128 = 7.8ms

    // Read the conversion result
    return readRegister(ADS1115_REG_CONVERSION);
}

float ADS1115::getVoltage(ADS1115_CHANNEL channel)
{
    int16_t adc = readADC(channel);
    return adc * getMultiplier();
}

void ADS1115::startComparator_SingleEnded(ADS1115_CHANNEL channel, int16_t threshold)
{
    // Configure the device for specified channel in continuous mode
    uint16_t config = ADS1115_CFG_OS_SINGLE |                           // Start single conversion
                      (ADS1115_CFG_MUX_SINGLE_0 + (channel * 0x1000)) | // Select channel
                      this->gain |                                      // Set PGA/gain
                      ADS1115_CFG_MODE_CONTIN |                         // Continuous mode
                      ADS1115_CFG_DR_128SPS |                           // 128 samples per second
                      ADS1115_CFG_COMP_MODE_TRAD |                      // Traditional comparator
                      ADS1115_CFG_COMP_POL_ACTIV_LOW |                  // Active low
                      ADS1115_CFG_COMP_LAT_NONLAT |                     // Non-latching
                      ADS1115_CFG_COMP_QUE_1CONV;                       // Assert after 1 conversion

    writeRegister(ADS1115_REG_CONFIG, config);

    // Set the high threshold register
    writeRegister(ADS1115_REG_HI_THRESH, threshold);

    // Set the low threshold register to minimum to trigger on threshold exceeded
    writeRegister(ADS1115_REG_LO_THRESH, -32768);
}

int16_t ADS1115::getLastConversionResults()
{
    return readRegister(ADS1115_REG_CONVERSION);
}