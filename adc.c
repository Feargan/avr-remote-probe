#include "adc.h"

#include <avr/io.h>

void ADC_Init()
{
	ADMUX = 0;	// AREF (previous = 1 << REFS1;
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable ADC, maximum prescaler (128)
}

uint16_t ADC_Read(uint8_t Port)
{
	//ADMUX = (1 << REFS1) | Port;
	ADMUX = Port;
	ADCSRA |= (1 << ADSC);
	while((ADCSRA >> ADSC)&1);
	return ADC;//(((uint16_t)ADCH << 8)|(uint16_t)ADCL);
}

float ADC_LinearV2U(uint16_t adc, int16_t Resolution, float Ref,  float VoltsPerUnit, float ZeroVoltage)
{
	return ( ( (float)adc/Resolution )*Ref-ZeroVoltage)/VoltsPerUnit;
}