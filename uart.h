#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include "boolean.h"

#include <inttypes.h>

#define UART_OVERFLOW 0xfa00
#define UART_NONE 0xfb00
#define UART_TIMEOUT -2

#define UART_BUFFER_SIZE 128

//#define BAUDRATE ((F_CPU)/(9600*16UL)-1)

void UART_Init(const uint32_t Baudrate);
void UART_Suspend();
bool UART_Pending();
void UART_Putc(uint8_t Chr);
void UART_Puts(const char* String);
void UART_Puts_P(const char* String);
uint16_t UART_Getc();
int UART_ReadLn(char* Buffer, uint8_t Max, bool WaitForData);
void UART_ReadBytes(char* Buffer, uint8_t Length);
int UART_ReadUntil_P(char* Buffer, uint8_t Max, bool WaitForData, const char* Delimiters);

#endif