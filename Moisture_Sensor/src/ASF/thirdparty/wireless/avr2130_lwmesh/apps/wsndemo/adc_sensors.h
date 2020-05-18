
#ifndef ADC_SENSORS
#define ADC_SENSORS
#include <adc.h>
#include <adc_feature.h>
uint16_t bat_adc_read(void);
uint16_t moist_adc_read(void);
void configure_adc_averaging(void);
void config_500Khz_osc(void);
 
#endif