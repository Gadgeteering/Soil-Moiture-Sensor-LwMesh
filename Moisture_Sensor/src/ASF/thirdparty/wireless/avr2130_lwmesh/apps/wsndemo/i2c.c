#include <user_board.h>
#include <i2c_master.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <port.h>
#include "i2c.h"
#include <delay.h>

/* Init i2c packet. */
struct i2c_master_packet packet = {
.address = 00,
.data_length = 1,
.data = write_buffer,
.ten_bit_address = false,
.high_speed = false,
.hs_master_code = 0x0,
};
void i2c_init(void){
	

	/* Initialize I2C in master mode to access the transceiver */

	//struct port_config pin_conf;
	//port_get_config_defaults(&pin_conf);
	//pin_conf.direction = PORT_PIN_DIR_OUTPUT_WTH_READBACK;
	//port_pin_set_config(PIN_PA08,&pin_conf);
	//pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	//port_pin_set_config(PIN_PA09,&pin_conf);
	//port_pin_set_output_level(PIN_PA08, true);
	//port_pin_set_output_level(PIN_PA08, false);
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout =10000;
	config_i2c_master.generator_source = GCLK_GENERATOR_1;
	config_i2c_master.pinmux_pad0=PINMUX_PA08C_SERCOM0_PAD0;
	config_i2c_master.pinmux_pad1=PINMUX_PA09C_SERCOM0_PAD1;
	/* Initialize and enable device with config. */
	i2c_master_init(&i2c_master_instance, CONF_I2C_MASTER_MODULE,&config_i2c_master);
	i2c_master_enable(&i2c_master_instance);

}
void i2c_write(uint8_t Addr,uint8_t Reg,uint8_t length)
{
	uint16_t timeout = 0;	
	
	/* Write buffer to slave until success. */
	packet.data[0] = Reg;
	packet.data_length=1;
	packet.address = Addr;
	
while (i2c_master_write_packet_wait(&i2c_master_instance, &packet) !=
STATUS_OK) {
/* Increment timeout counter and check if timed out. */
if (timeout++ == TIMEOUT) {
printf("i2c failed");
break;
}
}
	
}

uint8_t i2c_read(uint8_t Addr,uint8_t *read_buffer,uint8_t length)
{
uint16_t timeout = 0;	
uint8_t Status = 1;	
	/* Read buffer to slave until success. */

	packet.data_length=length; //Read Length
	packet.address = Addr;
	packet.data = read_buffer;
	

while (i2c_master_read_packet_wait(&i2c_master_instance, &packet) !=
STATUS_OK) {
/* Increment timeout counter and check if timed out. */
if (timeout++ == TIMEOUT) {
printf("I2C Read Fail");
Status=0;
break;
}
}

/* Read from slave until success. */
read_buffer = packet.data;
return Status;

}


