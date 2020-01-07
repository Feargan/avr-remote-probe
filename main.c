#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "adc.h"
#include "wlan.h"
//#include "interface.h"
#include "buttons.h"
#include "ui.h"

char EEMEM CfgSSID[64] = { "Zhone_D4EF" };
char EEMEM CfgPassword[64]  = { "KKKKKKKKKK" };
char EEMEM CfgBrokerAddr[64] = { "192.168.1.18" };
uint16_t EEMEM CfgBrokerPort = 7750;

volatile uint8_t BrokerFlags = 0;

#define BROKER_FLAG_REGISTER_OK 0x1
#define BROKER_FLAG_QUERIED 0x2

void recv(uint8_t Length, char* Buffer)
{
	if(!strncmp_P(Buffer, PSTR("QUERY"), Length))
	{
		BrokerFlags |= BROKER_FLAG_QUERIED;
	}
	else if(!strncmp_P(Buffer, PSTR("REGISTER OK"), Length))
	{
		BrokerFlags |= BROKER_FLAG_REGISTER_OK;
	}
}

static void Broker_Connect()
{
	LCD_Clear();
	LCD_DrawText_P(0, 0, PSTR("WLAN startup"));
	LCD_Render();
	switch(WLAN_Init())
	{
		case WLAN_INIT_ERROR_NO_ANSWER:
			LCD_DrawText_P(0, 1, PSTR("err: no answer"));
			break;
		case WLAN_INIT_ERROR_CORRUPTED_ANSWER:
			LCD_DrawText_P(0, 1, PSTR("err: protocol"));
			break;
		case WLAN_OK:
		{
			LCD_DrawText_P(0, 1, PSTR("chip ready")); // disassemble this into smaller functions for readability
			LCD_Render();
			_delay_ms(500);
			char eeBuf[64];
			eeprom_read_block(WLAN_AccessPoints[0].SSID, CfgSSID, sizeof(WLAN_AccessPoints[0].SSID));
			eeprom_read_block(eeBuf, CfgPassword, sizeof(eeBuf));
			if(WLAN_JoinAccessPoint(WLAN_AccessPoints[0].SSID, eeBuf) == WLAN_OK)
			{
				LCD_DrawText_P(0, 2, PSTR("AP connected"));
				LCD_Render();
				eeprom_read_block(eeBuf, CfgBrokerAddr, sizeof(CfgBrokerAddr));
				if(WLAN_TcpConnect(eeBuf, eeprom_read_word(&CfgBrokerPort)) == WLAN_OK)
				{
					WLAN_TcpSend_P(PSTR("REGISTER test"));
					_delay_ms(500);
					WLAN_Listen(recv);
					if(BrokerFlags&BROKER_FLAG_REGISTER_OK)
						LCD_DrawText_P(0, 3, PSTR("Broker conn."));
					else
						LCD_DrawText_P(0, 3, PSTR("Broker timeout"));
				}
				else
					LCD_DrawText_P(0, 3, PSTR("Broker fail"));
			
			}
			else
				LCD_DrawText_P(0, 2, PSTR("AP conn. fail"));
		}
		break;
	}
	LCD_Render();
	_delay_ms(1000);
}

int main(void)
{
	SetupButtons();
	uint8_t Error = 0;
	//DDRD |= (1 << PD6);
	//PORTD &= ~(1 << PD6);
	const float vRef = 3.3f;
	char Buffer[32];
	char fBuffer[16];
	LCD_Init();
	LCD_SetupBacklight();
	ADC_Init();
	
	LCD_DrawText_P(0, 2, PSTR("Termometr"));
	LCD_Render();
	_delay_ms(2000);
	
	UI_Main();
	/*Broker_Connect();
	
	uint8_t RemainingTime = 60;
    while (1) 
    {
		LCD_Clear();
		if((Error || !BrokerFlags&BROKER_FLAG_REGISTER_OK) && RemainingTime == 0)
		{
			Error = 0;
			RemainingTime = 60;
			Broker_Connect();
		}
		if(RemainingTime > 0 &&	(Error || !BrokerFlags&BROKER_FLAG_REGISTER_OK))
		{
			sprintf_P(Buffer, PSTR("restart in %ds"), RemainingTime);
			LCD_DrawText(0, 5, Buffer);
			RemainingTime--;
		}	
		PORTD &= ~(1 << PD6);
		if(!Error && WLAN_Listen(recv) == WLAN_ERROR_UART_TIMEOUT)
			Error = 1;
		dtostrf(ADC_LinearV2U(ADC_Read(0), 1024, vRef, 0.01f, 0.5f), 0, 1, fBuffer); // move such blocks into a function
		sprintf_P(Buffer, PSTR("%s st. C"), fBuffer);
		LCD_DrawText(0, 0, Buffer);
		if(BrokerFlags&BROKER_FLAG_QUERIED)
		{
			PORTD |= (1 << PD6);
			sprintf_P(Buffer, PSTR("DATA temp %s\r\n"), fBuffer); // add CRLF!!
			if(WLAN_TcpSend(Buffer) != WLAN_OK)
				Error = 1;
		}
		
		uint16_t hVoltage = ADC_Read(1);
		dtostrf(ADC_LinearV2U(hVoltage, 1024, vRef, 1.f, 0.f), 0, 2, fBuffer);
		sprintf_P(Buffer, PSTR("%s V"), fBuffer);
		LCD_DrawText(0, 1, Buffer);
		if(BrokerFlags&BROKER_FLAG_QUERIED)
		{
			sprintf_P(Buffer, PSTR("DATA hvolt %s\r\n"), fBuffer); // CRLF!!
			if(WLAN_TcpSend(Buffer) != WLAN_OK)
				Error = 1;
		}
		
		dtostrf(ADC_LinearV2U(hVoltage, 1024, vRef, 1.f, 0.f)-vRef/2, 0, 3, fBuffer);
		sprintf_P(Buffer, PSTR("%s V err"), fBuffer);
		LCD_DrawText(0, 2, Buffer);
		

		BrokerFlags &= ~(BROKER_FLAG_QUERIED);
		
		LCD_Render();
		
		_delay_ms(1000);
    }*/
}

