#include "uart.h"
#include "boolean.h"

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

uint8_t Head = 0;
uint8_t Tail = 0;
char UART_Buffer[UART_BUFFER_SIZE];
bool OverflowFlag = false;
uint16_t FrameTime;

ISR(USART_RX_vect)
{
	UART_Buffer[Head] = UDR0;
	Head = (Head + 1) % UART_BUFFER_SIZE;
	if (Head == Tail) // or check at the beginning of ISR if it will overflow, if yes - return
		OverflowFlag = true;
	/*else if(Head > Tail && PreOverflowFlag)
	{
	OverflowFlag = true;
	PreOverflowFlag = false;
	}
	else
	PreOverflowFlag = false;*/
}

void UART_Init(const uint32_t Baudrate)
{
	const uint32_t Baudreg = ((F_CPU) / (Baudrate * 16UL) - 1);
	cli();
	UBRR0H |= Baudreg >> 8;
	UBRR0L = Baudreg;
	FrameTime = 1000000/Baudrate*10 + 1 + 50; // * BitsPerFrame + Round + FrameDelay
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
	UCSR0B |= (1 << RXCIE0);
	sei();
}

void UART_Suspend()
{
	UCSR0B &= ~(1 << RXCIE0);
}

bool UART_Pending()
{
	if(Head != Tail)
		return true;
	return false;
}

uint16_t UART_Getc()
{
	if (OverflowFlag)
	{
		OverflowFlag = false;
		return UART_OVERFLOW;
	}
	if (Tail == Head)
		return UART_NONE;
	char Val = UART_Buffer[Tail];
	Tail = (Tail + 1) % UART_BUFFER_SIZE;
	return Val;
}

void UART_Putc(uint8_t Chr)
{
	while ((UCSR0A & (1 << UDRE0)) == 0) {}
	UDR0 = Chr;
}

void UART_Puts_P(const char* String)
{
	char Chr;
	for (; (Chr = pgm_read_byte(String)); String++)
		UART_Putc(Chr);
}

void UART_Puts(const char* String)
{
	char Chr;
	for (; (Chr = *String); String++)
		UART_Putc(Chr);
}

int UART_ReadUntil_P(char* Buffer, uint8_t Max, bool WaitForData, const char* Delimiters) // scale with ReadLn...?
{
	if(!Max)
		return 0;
	uint8_t n = 0;
	uint16_t Chr;
	const uint8_t nDelims = strlen_P(Delimiters);
	Chr = UART_Getc();
	while(Chr != UART_OVERFLOW && n<Max-1)
	{
		uint16_t RemainingTime = FrameTime;
		if(Chr == UART_NONE && !WaitForData) // ATTENTION HERE: Might lock the uC if transmission errors occur!!
		{
			while(Chr == UART_NONE && RemainingTime > 0)
			{
				Chr = UART_Getc();
				_delay_us(1);
				RemainingTime--;
			}
			if(Chr == UART_NONE)
				break;
		}
		RemainingTime = 30; // now seconds
		uint16_t msecs = 1000;
		while(Chr == UART_NONE && WaitForData)
		{
			Chr = UART_Getc();
			msecs--;
			_delay_ms(1);
			if(msecs <= 0)
			{
				msecs = 1000;
				RemainingTime--;
			}
			if(RemainingTime <= 0)
				return -1; // timed out
		}
		
		for(uint8_t i = 0; i<nDelims; i++)
		{
			char DelimChr = pgm_read_byte(&Delimiters[i]);
			if(Chr == DelimChr)
			{
				Buffer[n] = '\0';
				return n;
			}
		}
		Buffer[n] = Chr;
		Chr = UART_Getc();
		n++;
	}
	Buffer[n] = '\0';
	return n;
}

int UART_ReadLn(char* Buffer, uint8_t Max, bool WaitForData)
{
	if (!Max)
		return 0;
	uint8_t n = 0;
	uint16_t Chr;
	Chr = UART_Getc();
	while (Chr != UART_OVERFLOW && n<Max - 1)
	{
		uint16_t RemainingTime = FrameTime;
		if(Chr == UART_NONE && !WaitForData) // ATTENTION HERE: Might lock the uC if transmission errors occur!!
		{
			while(Chr == UART_NONE && RemainingTime > 0)
			{
				Chr = UART_Getc();
				_delay_us(1);
				RemainingTime--;
			}
			if(Chr == UART_NONE)
				break;
		}
		RemainingTime = 10; // now seconds
		uint16_t msecs = 1000;
		while(Chr == UART_NONE && WaitForData)
		{
			Chr = UART_Getc();
			msecs--;
			_delay_ms(1);
			if(msecs <= 0)
			{
				msecs = 1000;
				RemainingTime--;
			}
			if(RemainingTime <= 0)
				return -1; // timed out
		}
		
		if (Chr == '\n')
		{
			n--; // cr lf
			break;
		}
		Buffer[n] = Chr;
		Chr = UART_Getc();
		n++;
	}
	Buffer[n] = '\0';
	return n;
	//return UART_ReadUntil_P(Buffer, Max, WaitForData, PSTR("\r\n"));
}

void UART_ReadBytes(char* Buffer, uint8_t Length)
{
	if(!Length)
		return;
	uint8_t n = 0;
	uint16_t Chr;
	Chr = UART_Getc();
	while(Chr != UART_OVERFLOW && n<Length)
	{
		while(Chr == UART_NONE)
		{
			Chr = UART_Getc();
		}
		Buffer[n] = Chr;
		n++;
		Chr = UART_Getc();
	}
}