/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
extern void system_init(void);
extern void wsndemo_init(void);
extern void wsndemo_task(void);
extern void i2c_init(void);

int main (void)
{
	/* Configure device and enable. */
	system_init();
	//struct port_config pin_conf;
	//port_get_config_defaults(&pin_conf);
	//pin_conf.direction = PORT_PIN_DIR_OUTPUT;
	//port_pin_set_config(PIN_PA27,&pin_conf);
	//port_pin_set_output_level(PIN_PA27, true);
	//port_pin_set_output_level(PIN_PA27, false);
	struct system_pinmux_config config_pinmux;
	system_pinmux_get_config_defaults(&config_pinmux);
	config_pinmux.mux_position =PINMUX_PA27H_GCLK_IO0;
	config_pinmux.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	system_pinmux_pin_set_config(PIN_PA27,&config_pinmux);
	
	
	
	i2c_init();
	


	delay_init();
	//udc_start();
	wsndemo_init();
	
	while (1)
	{
		wsndemo_task();
	}
}
