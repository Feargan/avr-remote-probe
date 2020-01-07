#ifndef ADC_H_
#define ADC_H_

#include <inttypes.h>

void ADC_Init(); // options
uint16_t ADC_Read(uint8_t Port);
float ADC_LinearV2U(uint16_t adc, int16_t Resolution, float Ref,  float UnitsPerV, float ZeroVoltage); // inline??
//float ADC_ExponentialV2u(uint16_t adc);

#endif /* ADC_H_ */