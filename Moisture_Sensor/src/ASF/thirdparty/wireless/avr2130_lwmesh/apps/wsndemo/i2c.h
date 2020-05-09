#ifndef i2c
#define i2c

#include <i2c_master.h>


void i2c_init(void);
uint8_t i2c_read(uint8_t Addr,uint8_t *read_buffer, uint8_t length);
void i2c_write(uint8_t Addr, uint8_t Reg,uint8_t length);
#define TIMEOUT 1000
#define DATA_LENGTH 8

struct i2c_master_config config_i2c_master;
struct i2c_master_module i2c_master_instance;

static uint8_t write_buffer[DATA_LENGTH] = {
	0xE5, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
};
static uint8_t read_buffer[DATA_LENGTH];




#endif