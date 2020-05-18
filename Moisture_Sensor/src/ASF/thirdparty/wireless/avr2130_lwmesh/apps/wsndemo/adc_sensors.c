
#include "adc_sensors.h"
#include <asf.h>
#include <adc_feature.h>
#define ADC_REFERENCE_INTVCC1_VALUE 3300
#define ADC_12BIT_FULL_SCALE_VALUE 4095
struct adc_module adc_instance;


static uint8_t adc_read_buffer[2];
void configure_adc_averaging(void)
{
	
struct adc_config conf_adc;

adc_get_config_defaults(&conf_adc);

conf_adc.clock_source = GCLK_GENERATOR_1;
conf_adc.reference = ADC_REFERENCE_INTVCC1;
//conf_adc.clock_prescaler = ADC_CTRLB_PRESCALER_DIV512;
conf_adc.positive_input = ADC_POSITIVE_INPUT_PIN6;
conf_adc.negative_input = ADC_NEGATIVE_INPUT_GND;
conf_adc.resolution = ADC_RESOLUTION_12BIT;
//conf_adc.accumulate_samples = ADC_ACCUMULATE_SAMPLES_128;
//conf_adc.divide_result = ADC_DIVIDE_RESULT_128;
adc_init(&adc_instance, ADC, &conf_adc);
adc_enable(&adc_instance);    
}

void config_500Khz_osc(void){
	struct system_pinmux_config config_pinmux;
	system_pinmux_get_config_defaults(&config_pinmux);
	config_pinmux.mux_position =PINMUX_PA27H_GCLK_IO0;
	config_pinmux.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
	system_pinmux_pin_set_config(PIN_PA27,&config_pinmux);
	
}

uint16_t bat_adc_read(void){
           // enable ADC
adc_start_conversion(&adc_instance);
uint16_t result;
do {

}
while ( adc_read(&adc_instance,&result) == STATUS_BUSY);

return result;
}

uint16_t moist_adc_read(void){
           // enable ADC
adc_start_conversion(&adc_instance);
uint16_t result;
uint16_t raw;
do {

}
while ( adc_read(&adc_instance,&raw) == STATUS_BUSY);
result = ((float)raw * ((float) ADC_REFERENCE_INTVCC1_VALUE)/(float) ADC_12BIT_FULL_SCALE_VALUE);
return result;
}


