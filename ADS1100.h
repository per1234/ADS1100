/**************************************************************************/
/*
        Library for ADS1100
*/
/**************************************************************************/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

/**************************************************************************
    I2C ADDRESS/BITS
**************************************************************************/
    #define ADS1100_DEFAULT_ADDRESS                       (0x48)    

/**************************************************************************
    CONFIGURATION REGISTER
**************************************************************************/
   #define ADS1100_START_SINGLE_CONVERSION				(0x80)		// In single conversion mode, writing causes a conversion to start, and writing a 0 has no effect. No effect in continuous
	
	#define ADS1100_CONVERSION_MASK							(0x10)		// single conversion mode. IF not set - continuous conversion mode
	#define ADS1100_CONVERSION_SINGLE						(0x10)		// single conversion mode. IF not set - continuous conversion mode
	#define ADS1100_CONVERSION_CONTINOUS					(0x00)		// single conversion mode. IF not set - continuous conversion mode
	
	#define ADS1100_DATA_RATE_MASK							(0x0C)		// ADS1100’s data rate 128 - 8 SPS
	#define ADS1100_DATA_RATE_128SPS							(0x00)					
	#define ADS1100_DATA_RATE_32SPS							(0x04)
	#define ADS1100_DATA_RATE_16SPS							(0x08)
	#define ADS1100_DATA_RATE_8SPS							(0x0C)
	
	#define ADS1100_GAIN_MASK									(0x03)		// ADS1100’s gain 1-8x
	#define ADS1100_GAIN_1X										(0x00)
	#define ADS1100_GAIN_2X										(0x01)
	#define ADS1100_GAIN_4X										(0x02)
	#define ADS1100_GAIN_8X										(0x03)

/**************************************************************************
    DATA READING
**************************************************************************/

class ADS1100
{
    public:
			uint8_t  	ads1100_i2cAddress;					// I2C address of the unit
			uint8_t   	currentRegister=B00001100;       // the last register used
			int16_t     value;                          	// value of the ADC returned

			
			ADS1100(byte address) 
			{
				ads1100_i2cAddress=address;
			}
			bool begin(void)
			{
				Wire.begin();
			}
			
			int16_t getvalue(void)		// reading the value regardless of the fact it is available
			{
				uint8_t highbyte,lowbyte;
				Wire.requestFrom((uint8_t) ads1100_i2cAddress, (uint8_t) 2);  
				while(Wire.available()) // ensure all the data comes in
				{ 
					highbyte  = Wire.read(); // high byte * B11111111
					lowbyte   = Wire.read(); // low byte
				}
				value=(((int16_t) highbyte) *256 + lowbyte);
				return value;
			}
			
			void configure(uint8_t singleMode, uint8_t dataRate, uint8_t gain) {
			  singleMode  &= ADS1100_CONVERSION_MASK;
			  dataRate    &= ADS1100_DATA_RATE_MASK;
			  gain        &= ADS1100_GAIN_MASK;
			  currentRegister = singleMode+dataRate+gain;

			  Wire.beginTransmission(ads1100_i2cAddress);
			  #if ARDUINO >= 100
			  Wire.write((uint8_t)currentRegister);
			  #else
			  Wire.send(currentRegister);
			  #endif
			  Wire.endTransmission();
			}
			
   		uint8_t conversionDone() { 	// check if conversion was completed. Reasonable only in the single conversion mode
			  uint8_t highbyte,lowbyte,regbyte;
			  Wire.requestFrom((uint8_t) ads1100_i2cAddress, (uint8_t) 3);
			  while(Wire.available()) // ensure all the data comes in
				 {   
					highbyte = Wire.read(); // high byte * B11111111
					lowbyte  = Wire.read(); // low byte
					regbyte  = Wire.read(); // register byte
				 }
				 value=(highbyte*256+lowbyte);
				 regbyte &= ADS1100_START_SINGLE_CONVERSION;
				 return (regbyte==0);
			}   

			void startSingleConversion() {
			  // start the conversion mode
			  currentRegister=currentRegister|ADS1100_START_SINGLE_CONVERSION;
			  Wire.beginTransmission(ads1100_i2cAddress);
			  #if ARDUINO >= 100
			  Wire.write((uint8_t) currentRegister);
			  #else
			  Wire.send(currentRegister);
			  #endif
			  Wire.endTransmission();
			}			
    private:
};
