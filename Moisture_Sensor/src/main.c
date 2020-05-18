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


int main (void)
{
	/* Configure device and enable. */
	system_init();

	delay_init();	
	
	//stdio_usb_init();
	wsndemo_init();
	
	while (1)
	{
		wsndemo_task();
	}
}
