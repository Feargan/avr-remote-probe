#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>

#include "lcd.h"
#include "adc.h"
#include "wlan.h"
#include "ui_main_menu.h"
#include "buttons.h"
#include "str_util.h"
#include "math.h"
#include "tinyexpr.h"
#include <string.h>

volatile uint8_t BrokerFlags = 0;

#define BROKER_FLAG_REGISTER_OK 0x1
#define BROKER_FLAG_QUERIED 0x2

void recv(uint8_t Length, char* Buffer)
{
	char* Line = strtok_P(Buffer, PSTR("\n"));
	do 
	{
		if(!strncmp_P(Buffer, PSTR("QUERY"), 5))
		{
			BrokerFlags |= BROKER_FLAG_QUERIED;
		}
		else if(!strncmp_P(Buffer, PSTR("REGISTER OK"), 11))
		{
			BrokerFlags |= BROKER_FLAG_REGISTER_OK;
		}
		Length-=strlen(Line)+1;
	} while ((Line = strtok_P(NULL, PSTR("\n"))));
	
}

void Broker_Connect()
{
	BrokerFlags &= ~(BROKER_FLAG_REGISTER_OK);
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
			LCD_DrawText_P(0, 1, PSTR("chip ready"));
			LCD_Render();
			_delay_ms(500);
			if(!(eeprom_read_byte(&CfgSwitches)>>CFG_NETWORK_ENABLED)&1)
				return;
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
					char Name[16];
					eeprom_read_block(Name, CfgSystemName, 16);
					sprintf_P(eeBuf, PSTR("REGISTER %s\n"), Name);
					WLAN_TcpSend(eeBuf);
					_delay_ms(500);
					WLAN_Listen(recv);
					if(BrokerFlags&BROKER_FLAG_REGISTER_OK)
					{
						LCD_DrawText_P(0, 3, PSTR("Broker conn."));
						LCD_DrawText_P(0, 4, PSTR("OK"));
					}
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

#define TRIGGER1_PIN PC4
#define TRIGGER2_PIN PC5
#define TRIGGER_DDR DDRC
#define TRIGGER_PORT PORTC

int main(void)
{
	TRIGGER_DDR |= (1 << TRIGGER1_PIN);
	TRIGGER_DDR |= (1 << TRIGGER2_PIN);
	TRIGGER_PORT &= ~(1 << TRIGGER1_PIN);
	TRIGGER_PORT &= ~(1 << TRIGGER2_PIN);
	
	SetupButtons();
	const float vRef = 3.3f;
	LCD_Init();
	LCD_SetupBacklight();
	LCD_SetBrightness(eeprom_read_byte(&CfgBrightness));
	LCD_SetBacklightBrightness(eeprom_read_byte(&CfgBacklight));
	ADC_Init();
	
	LCD_DrawText_P(24, 3, PSTR("uProbe"));
	LCD_Render();
	_delay_ms(2000);

	Broker_Connect();
	
	char Buffer[90] = { 0 };
	char fBuffer[16] = { 0 };
	uint8_t Error = 0;
	uint8_t RemainingTime = 60;
    while (1) 
    {
		LCD_Clear();
		if(Error && !(eeprom_read_byte(&CfgSwitches)>>CFG_NETWORK_ENABLED)&1)
		{
			Error = 0;
			RemainingTime = 60;
		}
		if(RemainingTime == 0)
		{
			Error = 0;
			RemainingTime = 60;
			Broker_Connect();
		}
		if((eeprom_read_byte(&CfgSwitches)>>CFG_NETWORK_ENABLED)&1 && RemainingTime > 0 &&	(Error || !BrokerFlags&BROKER_FLAG_REGISTER_OK || !(WLAN_GetState()&(1<<WLAN_TCP_CONNECTED))))
		{
			sprintf_P(Buffer, PSTR("restart in %ds"), RemainingTime);
			LCD_DrawText(0, 5, Buffer);
			RemainingTime--;
		}	
		if(!Error && WLAN_Listen(recv) == WLAN_ERROR_UART_TIMEOUT)
			Error = 1;
		float Values[4] = { ADC_LinearV2U(ADC_Oversample(0, 3), 8196, vRef, 1.f, 0.f), ADC_LinearV2U(ADC_Oversample(1, 3), 8196, vRef, 1.f, 0.f),
							(TRIGGER_PORT>>TRIGGER1_PIN)&1 ? 1.f : 0.f,  (TRIGGER_PORT>>TRIGGER2_PIN)&1 ? 1.f : 0.f };
		{
			
			for (uint8_t i = 0; i < 4; i++)
			{
				if(((eeprom_read_byte(&CfgSwitches)>>i)&1) == 0)
					continue;

				eeprom_read_block(Buffer, &CfgExprs[i][0], 64);
				
				str_replace(Buffer, "r", "3.3");
				
				dtostrf(Values[0], 0, 4, fBuffer);
				str_replace(Buffer, "s1", fBuffer);

				dtostrf(Values[1], 0, 4, fBuffer);
				str_replace(Buffer, "s2", fBuffer);

				dtostrf(Values[2], 0, 4, fBuffer);
				str_replace(Buffer, "t1", fBuffer);

				dtostrf(Values[3], 0, 4, fBuffer);
				str_replace(Buffer, "t2", fBuffer);

				int Error = 0;
				Values[i] = te_interp(Buffer, &Error);
				Buffer[0] = 0;
				char NameBuffer[8];
				eeprom_read_block(NameBuffer, &CfgSensorNames[i][0], 8);
				dtostrf(Values[i], 0, i < 2 ? 3 : 0, fBuffer);
				if(i < 2) // inputs
					sprintf_P(Buffer, PSTR("%s=%s"), NameBuffer, Error ? "err" : fBuffer);
				else // outputs
					sprintf_P(Buffer, PSTR("%s: %s"), NameBuffer, Values[i] >= 1.f ? "ON" : "OFF");
				if(i == 2)
					TRIGGER_PORT =  Values[i] > 0.9f ? TRIGGER_PORT|(1<<TRIGGER1_PIN) : TRIGGER_PORT&(~(1<<TRIGGER1_PIN));
				if(i == 3)
					TRIGGER_PORT =  Values[i] > 0.9f ? TRIGGER_PORT|(1<<TRIGGER2_PIN) : TRIGGER_PORT&(~(2<<TRIGGER1_PIN));
				LCD_DrawText(0, i, Buffer);
				if(BrokerFlags&BROKER_FLAG_QUERIED)
				{
					sprintf_P(Buffer, PSTR("DATA %s %s\n"), NameBuffer, fBuffer);
					if(WLAN_TcpSend(Buffer) != WLAN_OK)
						Error = 1;
				}
			}
		}
		LCD_DrawText_P(0, 4, PSTR("AP:"));
		LCD_DrawText_P(18, 4, WLAN_GetState()&(1<<WLAN_AP_CONNECTED) ? PSTR("OK") : PSTR("X"));
		LCD_DrawText_P(40, 4, PSTR("BR:"));
		LCD_DrawText_P(58, 4, WLAN_GetState()&(1<<WLAN_TCP_CONNECTED) ? PSTR("OK") : PSTR("X"));

		BrokerFlags &= ~(BROKER_FLAG_QUERIED);
		
		LCD_Render();
		
		if(Button_OK_Pressed())
		{
			LCD_Clear();
			LCD_DrawText_P(0, 2, PSTR("Please wait..."));
			LCD_Render();
			if(WLAN_GetState()&(1<<WLAN_AP_CONNECTED))
			{
				WLAN_TcpDisconnect();
				WLAN_Disconnect();
			}
			Interface_Menu_P(&Menu_Main);
			if(eeprom_read_byte(&CfgSwitches)&(1 << CFG_NETWORK_ENABLED))
				Broker_Connect();
		}
		_delay_ms(200);
    }
}

