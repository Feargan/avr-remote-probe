#include "adc.h"

#include <avr/io.h>

void ADC_Init()
{
	ADMUX = 0;	// AREF as voltage reference
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // enable ADC with maximum prescaler (128)
}

uint16_t ADC_Read(uint8_t Port)
{
	ADMUX = Port; // set multiplexer to selected Port
	ADCSRA |= (1 << ADSC); // request sample
	while((ADCSRA >> ADSC)&1); // wait for sample
	return ADC; // return sample
}

uint16_t ADC_Oversample(uint8_t Port, uint8_t Bits)
{
	uint32_t Sum = 0;
	uint8_t n = 1 ;
	for(uint8_t j = Bits; j > 0; j--)
		n*=4;
	for(uint8_t i = 0; i<n; i++)
	{
		Sum += ADC_Read(Port);
	}
	Sum >>= Bits;
	return Sum;
}

float ADC_LinearV2U(uint16_t adc, int16_t Resolution, float Ref,  float VoltsPerUnit, float ZeroVoltage)
{
	return ( ( (float)adc/Resolution )*Ref-ZeroVoltage)/VoltsPerUnit;
}