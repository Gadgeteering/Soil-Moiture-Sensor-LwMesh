
#include "adc_sensors.h"
#include <asf.h>
#include <adc_feature.h>
struct adc_module adc_instance;


static uint8_t adc_read_buffer[2];
void configure_adc_averaging(void)
{
	
struct adc_config conf_adc;

adc_get_config_defaults(&conf_adc);

conf_adc.clock_source = GCLK_GENERATOR_1;
conf_adc.reference = ADC_REFERENCE_INTVCC1;
conf_adc.clock_prescaler = ADC_;
conf_adc.positive_input = ADC_POSITIVE_INPUT_PIN6;
conf_adc.negative_input = ADC_NEGATIVE_INPUT_GND;
conf_adc.resolution = ADC_RESOLUTION_8BIT;
conf_adc.accumulate_samples = ADC_ACCUMULATE_SAMPLES_128;
conf_adc.divide_result = ADC_DIVIDE_RESULT_DISABLE;
adc_init(&adc_instance, ADC, &conf_adc);
adc_enable(&adc_instance);    


 
while(1){            // enable ADC		
if( adc_read(&adc_instance,adc_read_buffer)== STATUS_OK)
{
	uint16_t sample = adc_read_buffer[0]+ (adc_read_buffer[1]<<8);
	adc_start_conversion(&adc_instance);  
	}


}

}

void adc_oversampling(void)
{

//raw_result = adc_start_read_result();
//result = ((float)raw_result * (float)ADC_REFERENCE_INTVCC1_VALUE)/(float)ADC_12BIT_FULL_SCALE_VALUE;
//printf("\n\rADC Input Voltage before Oversampling = %f", result);
//adc_disable(&adc_instance);
//configure_adc_sampling();
//raw_result = adc_start_read_result();
//result = ((float)raw_result * (float)ADC_REFERENCE_INTVCC1_VALUE)/(float)ADC_16BIT_FULL_SCALE_VALUE;
//printf("\n\rADC Input Voltage after Oversampling = %f", result);
//adc_disable(&adc_instance);
}