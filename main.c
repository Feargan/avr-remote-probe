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
//#include "expr.h"
#include "math.h"
#include "tinyexpr.h"
#include <string.h>

volatile uint8_t BrokerFlags = 0;

#define BROKER_FLAG_REGISTER_OK 0x1
#define BROKER_FLAG_QUERIED 0x2

/*static float f_log(struct expr_func *f, vec_expr_t *args, void *c) {
	return log(expr_eval(&vec_nth(args, 0)));
}

static struct expr_func user_funcs[] = {
	{"log", f_log, NULL, 0},
	{NULL, NULL, NULL, 0},
};*/

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
			LCD_DrawText_P(0, 1, PSTR("chip ready")); // disassemble this into smaller functions for readability
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

void str_replace(char *target, const char *needle, const char *replacement)
{
	char buffer[128] = { 0 };
	char *insert_point = &buffer[0];
	const char *tmp = target;
	size_t needle_len = strlen(needle);
	size_t repl_len = strlen(replacement);

	while (1) {
		const char *p = strstr(tmp, needle);

		// walked past last occurrence of needle; copy remaining part
		if (p == NULL) {
			strcpy(insert_point, tmp);
			break;
		}

		// copy part before needle
		memcpy(insert_point, tmp, p - tmp);
		insert_point += p - tmp;

		// copy replacement string
		memcpy(insert_point, replacement, repl_len);
		insert_point += repl_len;

		// adjust pointers, move on
		tmp = p + needle_len;
	}

	// write altered string back to target
	strcpy(target, buffer);
}

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
	
	LCD_DrawText_P(0, 2, PSTR("Termometr"));
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
		float Values[4] = { ADC_LinearV2U(ADC_Oversample(0, 3), 8196, vRef, 1.f, 0.f), ADC_LinearV2U(ADC_Oversample(0, 3), 8196, vRef, 1.f, 0.f),
							(TRIGGER_PORT>>TRIGGER1_PIN)&1 ? 1.f : 0.f,  (TRIGGER_PORT>>TRIGGER2_PIN)&1 ? 1.f : 0.f };
		{
			
			for (uint8_t i = 0; i < 4; i++)
			{
				if(((eeprom_read_byte(&CfgSwitches)>>i)&1) == 0)
					continue;
				//char ExprBuffer[128] = { 0 };
				//char FloatBuffer[16] = { 0 };
				
				//dtostrf(Values[i], 0, 1, fBuffer);
				//sprintf(fBuffer, "%f", 3.3);
				
				/*struct expr *Expression;
				struct expr_var_list vars = { 0 };
				char ExprBuffer[64] = { 0 };*/
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
				//Buffer[0] = 0;
				//sprintf_P(Buffer, PSTR("r=3.3, s1=%.3f, s2=%.3f, t1=%.3f, t2=%.3f, %s"), Values[0], Values[1], Values[2], Values[3], ExprBuffer);
				//Expression = NULL;//expr_create(Buffer, strlen(Buffer), &vars, user_funcs);
				//if(!Expression)
				//	continue;
				int Error = 0;
				Values[i] = te_interp(Buffer, &Error);//expr_eval(Expression);
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
					sprintf_P(Buffer, PSTR("DATA %s %s\n"), NameBuffer, fBuffer); // CRLF!!
					if(WLAN_TcpSend(Buffer) != WLAN_OK)
						Error = 1;
				}
				//printf("\neval: %f\n", Values[i]);
				/*struct expr_var* var = vars.head;
				for (int i = 0; i < 5; i++)
				{
					printf("%s: %f\n", var->name, var->value);
					var = var->next;
				}*/
				//expr_destroy(Expression, &vars);
			}
		}
		LCD_DrawText_P(0, 4, PSTR("AP:"));
		LCD_DrawText_P(18, 4, WLAN_GetState()&(1<<WLAN_AP_CONNECTED) ? PSTR("OK") : PSTR("X"));
		LCD_DrawText_P(40, 4, PSTR("BR:"));
		LCD_DrawText_P(58, 4, WLAN_GetState()&(1<<WLAN_TCP_CONNECTED) ? PSTR("OK") : PSTR("X"));
		
		
		/*float Sensors[2];
		dtostrf(ADC_LinearV2U(ADC_Oversample(0, 3), 8196, vRef, 0.01f, 0.5f), 0, 1, fBuffer); // move such blocks into a function
		sprintf_P(Buffer, PSTR("%s st. C"), fBuffer);
		LCD_DrawText(0, 0, Buffer);
		if(BrokerFlags&BROKER_FLAG_QUERIED)
		{
			sprintf_P(Buffer, PSTR("DATA temp %s\r\n"), fBuffer); // add CRLF!!
			if(WLAN_TcpSend(Buffer) != WLAN_OK)
				Error = 1;
		}
		
		uint16_t hVoltage = ADC_Oversample(1, 3);
		dtostrf(ADC_LinearV2U(hVoltage, 8196, vRef, 1.f, 0.f), 0, 2, fBuffer);
		sprintf_P(Buffer, PSTR("%s V"), fBuffer);
		LCD_DrawText(0, 1, Buffer);
		if(BrokerFlags&BROKER_FLAG_QUERIED)
		{
			sprintf_P(Buffer, PSTR("DATA hvolt %s\r\n"), fBuffer); // CRLF!!
			if(WLAN_TcpSend(Buffer) != WLAN_OK)
				Error = 1;
		}
		
		dtostrf(ADC_LinearV2U(hVoltage, 8196, vRef, 1.f, 0.f)-vRef/2, 0, 3, fBuffer);
		sprintf_P(Buffer, PSTR("%s V err"), fBuffer);
		LCD_DrawText(0, 2, Buffer);*/
		

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

