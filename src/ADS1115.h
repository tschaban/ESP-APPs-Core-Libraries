#ifndef _ADS1115_h
#define _ADS1115_h

#include <ESPAPP_Parameters.h>
#include <ESPAPP_Core.h>
#include <ESPAPP_I2C_Container.h>
#include <Wire.h>


// Register addresses
#define ADS1115_REG_CONVERSION      0x00
#define ADS1115_REG_CONFIG          0x01
#define ADS1115_REG_LO_THRESH       0x02
#define ADS1115_REG_HI_THRESH       0x03

// Config register masks
#define ADS1115_CFG_OS_MASK         0x8000
#define ADS1115_CFG_MUX_MASK        0x7000
#define ADS1115_CFG_PGA_MASK        0x0E00
#define ADS1115_CFG_MODE_MASK       0x0100
#define ADS1115_CFG_DR_MASK         0x00E0
#define ADS1115_CFG_COMP_MODE_MASK  0x0010
#define ADS1115_CFG_COMP_POL_MASK   0x0008
#define ADS1115_CFG_COMP_LAT_MASK   0x0004
#define ADS1115_CFG_COMP_QUE_MASK   0x0003

// Config values
// Operational status
#define ADS1115_CFG_OS_SINGLE       0x8000 // Start single conversion
#define ADS1115_CFG_OS_BUSY         0x0000 // Device busy
#define ADS1115_CFG_OS_NOTBUSY      0x8000 // Device not busy

// Multiplexer settings
#define ADS1115_CFG_MUX_DIFF_0_1    0x0000 // Differential P=AIN0, N=AIN1
#define ADS1115_CFG_MUX_DIFF_0_3    0x1000 // Differential P=AIN0, N=AIN3
#define ADS1115_CFG_MUX_DIFF_1_3    0x2000 // Differential P=AIN1, N=AIN3
#define ADS1115_CFG_MUX_DIFF_2_3    0x3000 // Differential P=AIN2, N=AIN3
#define ADS1115_CFG_MUX_SINGLE_0    0x4000 // Single-ended AIN0
#define ADS1115_CFG_MUX_SINGLE_1    0x5000 // Single-ended AIN1
#define ADS1115_CFG_MUX_SINGLE_2    0x6000 // Single-ended AIN2
#define ADS1115_CFG_MUX_SINGLE_3    0x7000 // Single-ended AIN3

// Programmable gain amplifier
#define ADS1115_CFG_PGA_6_144V      0x0000 // +/-6.144V range = Gain 2/3
#define ADS1115_CFG_PGA_4_096V      0x0200 // +/-4.096V range = Gain 1
#define ADS1115_CFG_PGA_2_048V      0x0400 // +/-2.048V range = Gain 2 (default)
#define ADS1115_CFG_PGA_1_024V      0x0600 // +/-1.024V range = Gain 4
#define ADS1115_CFG_PGA_0_512V      0x0800 // +/-0.512V range = Gain 8
#define ADS1115_CFG_PGA_0_256V      0x0A00 // +/-0.256V range = Gain 16

// Mode of operation
#define ADS1115_CFG_MODE_CONTIN     0x0000 // Continuous conversion mode
#define ADS1115_CFG_MODE_SINGLE     0x0100 // Single-shot mode (default)

// Data rate
#define ADS1115_CFG_DR_8SPS         0x0000 // 8 samples per second
#define ADS1115_CFG_DR_16SPS        0x0020 // 16 samples per second
#define ADS1115_CFG_DR_32SPS        0x0040 // 32 samples per second
#define ADS1115_CFG_DR_64SPS        0x0060 // 64 samples per second
#define ADS1115_CFG_DR_128SPS       0x0080 // 128 samples per second (default)
#define ADS1115_CFG_DR_250SPS       0x00A0 // 250 samples per second
#define ADS1115_CFG_DR_475SPS       0x00C0 // 475 samples per second
#define ADS1115_CFG_DR_860SPS       0x00E0 // 860 samples per second

// Comparator mode
#define ADS1115_CFG_COMP_MODE_TRAD  0x0000 // Traditional comparator with hysteresis
#define ADS1115_CFG_COMP_MODE_WINDOW 0x0010 // Window comparator

// Comparator polarity
#define ADS1115_CFG_COMP_POL_ACTIV_LOW 0x0000 // Active low
#define ADS1115_CFG_COMP_POL_ACTIV_HIGH 0x0008 // Active high

// Latching comparator
#define ADS1115_CFG_COMP_LAT_NONLAT 0x0000 // Non-latching comparator
#define ADS1115_CFG_COMP_LAT_LATCH  0x0004 // Latching comparator

// Comparator queue
#define ADS1115_CFG_COMP_QUE_1CONV  0x0000 // Assert after one conversion
#define ADS1115_CFG_COMP_QUE_2CONV  0x0001 // Assert after two conversions
#define ADS1115_CFG_COMP_QUE_4CONV  0x0002 // Assert after four conversions
#define ADS1115_CFG_COMP_QUE_DISABLE 0x0003 // Disable comparator (default)

// Channel definitions
enum ADS1115_CHANNEL {
    ADS1115_CHANNEL_0 = 0,
    ADS1115_CHANNEL_1 = 1,
    ADS1115_CHANNEL_2 = 2,
    ADS1115_CHANNEL_3 = 3
};

// Gain definitions
enum ADS1115_GAIN {
    ADS1115_GAIN_TWOTHIRDS = ADS1115_CFG_PGA_6_144V, // +/-6.144V
    ADS1115_GAIN_ONE       = ADS1115_CFG_PGA_4_096V, // +/-4.096V
    ADS1115_GAIN_TWO       = ADS1115_CFG_PGA_2_048V, // +/-2.048V (default)
    ADS1115_GAIN_FOUR      = ADS1115_CFG_PGA_1_024V, // +/-1.024V
    ADS1115_GAIN_EIGHT     = ADS1115_CFG_PGA_0_512V, // +/-0.512V
    ADS1115_GAIN_SIXTEEN   = ADS1115_CFG_PGA_0_256V  // +/-0.256V
};

class ADS1115 {
private:
    ESPAPP_Core *System;
    ESPAPP_I2C_Container *I2C;
    TwoWire *Wire;
    uint8_t i2cAddress;
    ADS1115_GAIN gain;
    
    uint16_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint16_t value);
    float getMultiplier();
    bool isConnected();
    
public:
    ADS1115(ESPAPP_Core *_System, ESPAPP_I2C_Container *_I2C);
    ~ADS1115();
    
    bool init();
    void setGain(ADS1115_GAIN gain);
    ADS1115_GAIN getGain();
    
    int16_t readADC(ADS1115_CHANNEL channel);
    float getVoltage(ADS1115_CHANNEL channel);
        
    void startComparator_SingleEnded(ADS1115_CHANNEL channel, int16_t threshold);
    int16_t getLastConversionResults();
};

#endif // _ADS1115_h