/***************************************************
  This is a library for the HTU21DF Humidity & Temp Sensor

  Designed specifically to work with the HTU21DF sensor from Adafruit
  ----> https://www.adafruit.com/products/1899

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "HTU21D.h"


static uint8_t i2c_read_buffer[10];
///**
 //* Constructor for the HTU21DF driver.
 //*/
////Adafruit_HTU21DF::Adafruit_HTU21DF() {
  /////* Assign default values to internal tracking variables. */
  ////_last_humidity = 0.0f;
  ////_last_temp = 0.0f;
////}
////
/////**
 //* Initialises the I2C transport, and configures the IC for normal operation.
 //*
 //* @return true (1) if the device was successfully initialised, otherwise
 //*         false (0).
 //*/

/**
 * Sends a 'reset' request to the HTU21DF, followed by a 15ms delay.
 */
void HTU21D_reset(void) {
  i2c_write(HTU21DF_I2CADDR,HTU21DF_RESET,1);
  delay_ms(15);
}


void HTU21D_Init(void) {
 
  HTU21D_reset();
 // i2c_write_reg(HTU21DF_I2CADDR,HTU21DF_READREG,1);
 // delay_ms(10);
 // return  (i2c_read_reg(HTU21DF_I2CADDR,HTU21DF_READREG,1) == 0x2);
  
   // after reset should be 0x2
}


/**
 * Performs a single temperature conversion in degrees Celsius.
 *
 * @return a single-precision (32-bit) float value indicating the measured
 *         temperature in degrees Celsius.
 */
float HTU21D_readTemperature(void) {
  // OK lets ready!
  i2c_write(HTU21DF_I2CADDR,HTU21DF_READTEMP,1);
  delay_ms(50); // add delay between request and actual read!

  uint8_t count = i2c_read(HTU21DF_I2CADDR,i2c_read_buffer,3);

  /* Make sure we got 3 bytes back. */
  if (count == 0) {
    return 0.0f;
  }

  ///* Read 16 bits of data, dropping the last two status bits. */
  uint16_t t = i2c_read_buffer[0];
  t <<= 8;
  t |= i2c_read_buffer[1] & 0b11111100;
//
  uint8_t crc = i2c_read_buffer[2];
  //(void)crc;
  //int t=0;
  float temp = t;
  temp *= 175.72f;
  temp /= 65536.0f;
  temp -= 46.85f;

  /* Track the value internally in case we need to access it later. */
  //_last_temp = temp;

  return temp;
}

///**
 //* Performs a single relative humidity conversion.
 //*
 //* @return A single-precision (32-bit) float value indicating the relative
 //*         humidity in percent (0..100.0%).
 //*/
float HTU21D_readHumidity(void) {
  ///* Prepare the I2C request. */
  i2c_write(HTU21DF_I2CADDR,HTU21DF_READHUM,1);
  ///* Wait a bit for the conversion to complete. */
  delay_ms(50);

  ///* Read the conversion results. */
  uint8_t count = i2c_read(HTU21DF_I2CADDR,i2c_read_buffer,3);

  ///* Make sure we got 3 bytes back. */
  if (count == 0) {
    return 0.0f;
  }
//
  ///* Read 16 bits of data, dropping the last two status bits. */
  ///* Read 16 bits of data, dropping the last two status bits. */
  uint16_t h = i2c_read_buffer[0];
  h <<= 8;
  h |= i2c_read_buffer[1] & 0b11111100;
//
  uint8_t crc =i2c_read_buffer[2];
  //(void)crc;
//
  float hum = h;
  hum *= 125.0f;
  hum /= 65536.0f;
  hum -= 6.0f;
//
  ///* Track the value internally in case we need to access it later. */
  //_last_humidity = hum;
//
 return hum;
}

