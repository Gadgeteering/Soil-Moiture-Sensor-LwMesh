/***************************************************
  This is a library for the HTU21D-F Humidity & Temp Sensor

  Designed specifically to work with the HTU21D-F sensor from Adafruit
  ----> https://www.adafruit.com/products/1899

  These displays use I2C to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/
/*Updated for ATMEL Studio 7*/

#ifndef HTU21D
#define HTU21D
#include <delay.h>
#include "i2c.h"

int8_t HTU21D_Init(void);
void HTU21D_reset (void);
float HTU21D_readTemperature(void);
float HTU21D_readHumidity(void);
/** Default I2C address for the HTU21D. */
#define HTU21DF_I2CADDR (0x40)

/** Read temperature register. */
#define HTU21DF_READTEMP (0xE3)

/** Read humidity register. */
#define HTU21DF_READHUM (0xE5)

/** Write register command. */
#define HTU21DF_WRITEREG (0xE6)

/** Read register command. */
#define HTU21DF_READREG (0xE7)

/** Reset command. */
#define HTU21DF_RESET (0xFE)

/** CHIP ID**/
#define HTU21DF_CHIPID (0x32)


#endif 

