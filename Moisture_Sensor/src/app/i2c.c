#include <user_board.h>
#include <i2c_master.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <port.h>
#include "i2c.h"
#include <delay.h>


void i2c_init(void){
	struct i2c_master_module i2c_master_instance;

	/* Initialize I2C in master mode to access the transceiver */

	
	
	struct port_config pin_conf;
	port_get_config_defaults(&pin_conf);
	pin_conf.direction = PORT_PIN_DIR_OUTPUT_WTH_READBACK;
	port_pin_set_config(EXT1_I2C_SERCOM_PINMUX_PAD0,&pin_conf);
	pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(EXT1_I2C_SERCOM_PINMUX_PAD1,&pin_conf);
	struct i2c_master_config config_i2c_master;
	i2c_master_get_config_defaults(&config_i2c_master);
	config_i2c_master.buffer_timeout =10000;
	config_i2c_master.pinmux_pad0=EXT1_I2C_SERCOM_PINMUX_PAD0;
	config_i2c_master.pinmux_pad1=EXT1_I2C_SERCOM_PINMUX_PAD1;
	/* Initialize and enable device with config. */
	i2c_master_init(&i2c_master_instance, CONF_I2C_MASTER_MODULE,&config_i2c_master);
	i2c_master_enable(&i2c_master_instance);

uint16_t timeout = 0;	
	/* Write buffer to slave until success. */
while (i2c_master_write_packet_wait(&i2c_master_instance, &packet) !=
STATUS_OK) {
/* Increment timeout counter and check if timed out. */
if (timeout++ == TIMEOUT) {
break;
}
}
delay_ms(150);
/* Read from slave until success. */
packet.data = read_buffer;
while (i2c_master_read_packet_wait(&i2c_master_instance, &packet) !=
STATUS_OK) {
/* Increment timeout counter and check if timed out. */
if (timeout++ == TIMEOUT) {
break;
}
}

}

