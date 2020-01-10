#include "wlan.h"
#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>

#include "boolean.h"

//#include "lcd.h"

uint8_t WLAN_States = 0;

uint8_t WLAN_GetState()
{
	return WLAN_States;
}

static int8_t WLAN_CheckErrors()
{
	char Buffer[20];
	while(true)
	{
		int Bytes = UART_ReadLn(Buffer, sizeof(Buffer), true);
		if(!Bytes)
			continue;
		else if(Bytes == -1)
		{
			WLAN_States &= ~(1 << WLAN_CHIP_OK);
			return WLAN_ERROR_UART_TIMEOUT;
		}
		else if(!strcmp_P(Buffer, PSTR("OK")))
			return WLAN_OK;
		else if(!strcmp_P(Buffer, PSTR("SEND OK")))
			return WLAN_OK;
		else if(!strcmp_P(Buffer, PSTR("FAIL")))
			break;
		else if(!strcmp_P(Buffer, PSTR("ERROR")))
			break;
	}
	return WLAN_ERROR;
}

uint8_t WLAN_Init()
{
	char Buffer[16];
	WLAN_States &= ~(1 << WLAN_CHIP_OK);
	//WLAN_RESET_DDR |= (1 << WLAN_RESET);
	//WLAN_RESET_PORT &= ~(1 << WLAN_RESET);
	_delay_ms(50);
	//WLAN_RESET_PORT |= (1 << WLAN_RESET);
    _delay_ms(1000);
	
	UART_Init(9600);
    UART_Puts_P(PSTR("ATE0\r\n"));
	_delay_ms(500);
	if(!UART_Pending())
		return WLAN_INIT_ERROR_NO_ANSWER;
	while(UART_Pending())
	{
		UART_ReadLn(Buffer, sizeof(Buffer), false);
		if(!strcmp_P(Buffer, PSTR("OK")))
		{
			WLAN_States |= (1 << WLAN_CHIP_OK);
			return WLAN_OK;
		}
	}
    return WLAN_INIT_ERROR_CORRUPTED_ANSWER;
}

int8_t WLAN_ListAccessPoints()
{
    char Buffer[84];
    int8_t Entries = 0;
    UART_Puts_P(PSTR("AT+CWLAP\r\n"));
    while(true)
    {
        //PORTD |= (1 << PD4);
        uint8_t Bytes = UART_ReadLn(Buffer, 128, true);
        //PORTD &= ~(1 << PD4);

        if(!Bytes)
            continue;
		else if(Bytes == -1)
		{
			WLAN_States &= ~(1 << WLAN_CHIP_OK);
			return WLAN_ERROR_UART_TIMEOUT;
		}
        else if(!strcmp_P(Buffer, PSTR("OK")))
            break;
        else if(!strcmp_P(Buffer, PSTR("ERROR")))
            return WLAN_ERROR;
        else if(!strncmp_P(Buffer, PSTR("+CWLAP:"), 7))
        {
            if(Entries >= WLAN_MAX_AP_ENTRIES)
                continue;
            char* Token = strtok_P(&Buffer[7], PSTR("(),\""));
            uint8_t Section = 0;
            while(Token != NULL)
            {
                switch(Section)
                {
                    case 0:
                        WLAN_AccessPoints[Entries].Encryption = atoi(Token);
                        break;
                    case 1:
                        strcpy(WLAN_AccessPoints[Entries].SSID, Token);
                        break;
                    case 2:
                        WLAN_AccessPoints[Entries].RSSID = atoi(Token);
                        break;
                    case 3:
                        // BSSID
                        break;
                    case 4:
                        // on ESP8266-01 it's probably channel, idc lol
                        break;
                }
                Section++;
                Token = strtok_P(NULL, PSTR("(),\""));
            }
            Entries++;
        }
    }
    return Entries;
}

int8_t WLAN_JoinAccessPoint(const char* SSID, const char* Password)
{
    char Buffer[16];
	WLAN_States &= ~(1 << WLAN_AP_CONNECTED);
    UART_Puts_P(PSTR("AT+CWJAP=\""));
    UART_Puts(SSID);
    UART_Puts_P(PSTR("\",\""));
    UART_Puts(Password);
    UART_Puts_P(PSTR("\"\r\n"));
    while(true)
    {
        uint8_t Bytes = UART_ReadLn(Buffer, 16, true);

        if(!Bytes)
            continue;
        else if(!strcmp_P(Buffer, PSTR("OK")))
		{
			WLAN_States |= (1 << WLAN_AP_CONNECTED);
            return WLAN_OK;
		}
        else if(!strcmp_P(Buffer, PSTR("FAIL")))
            break;
        else if(!strcmp_P(Buffer, PSTR("ERROR")))
            break;
    }
    return WLAN_ERROR;
	//return WLAN_CheckErrors();
}

int8_t WLAN_TcpConnect(const char* IP_Address, uint16_t Port)
{
    char Buffer[16];
	WLAN_States &= ~(1 << WLAN_TCP_CONNECTED);
    UART_Puts_P(PSTR("AT+CIPSTART=\"TCP\",\""));
    UART_Puts(IP_Address);
    itoa(Port, Buffer, 10);
    UART_Puts_P(PSTR("\","));
    UART_Puts(Buffer);
    UART_Puts_P(PSTR("\r\n"));
    while(true)
    {
        uint8_t Bytes = UART_ReadLn(Buffer, 16, true);

        if(!Bytes)
            continue;
        else if(!strcmp_P(Buffer, PSTR("OK")))
		{
			WLAN_States |= (1 << WLAN_TCP_CONNECTED);
            return WLAN_OK;
		}
        else if(!strcmp_P(Buffer, PSTR("FAIL")))
            break;
        else if(!strcmp_P(Buffer, PSTR("ERROR")))
            break;
    }
    return WLAN_ERROR;
	//return WLAN_CheckErrors();
}

int8_t WLAN_TcpSend_P(const char* Message)
{
    char Buffer[16]; // this should be moved to global section, and error checking loop moved to a single function
    UART_Puts_P(PSTR("AT+CIPSEND="));
    itoa(strlen_P(Message), Buffer, 10);
    UART_Puts(Buffer); // fix later
    UART_Puts_P(PSTR("\r\n"));
    _delay_ms(10);
    UART_Puts_P(Message);
    UART_Puts_P(PSTR("\r\n"));
    while(true)
    {
        uint8_t Bytes = UART_ReadLn(Buffer, 16, true);

        if(!Bytes)
            continue;
        else if(!strcmp_P(Buffer, PSTR("SEND OK")))
            return WLAN_OK;
        else if(!strcmp_P(Buffer, PSTR("FAIL")))
            break;
        else if(!strcmp_P(Buffer, PSTR("ERROR")))
            break;
    }
    return WLAN_ERROR;
	//return WLAN_CheckErrors();
}

int8_t WLAN_TcpSend(const char* Message)
{
    char Buffer[16];
    UART_Puts_P(PSTR("AT+CIPSEND="));
    itoa(strlen(Message), Buffer, 10);
    UART_Puts(Buffer); // fix later
    UART_Puts_P(PSTR("\r\n"));
    _delay_ms(10);
    UART_Puts(Message);
    UART_Puts_P(PSTR("\r\n"));
    while(true)
    {
        int Bytes = UART_ReadLn(Buffer, 16, true);

        if(!Bytes)
            continue;
		else if(Bytes == -1)
			return WLAN_ERROR_UART_TIMEOUT;
        else if(!strcmp_P(Buffer, PSTR("SEND OK")))
            return WLAN_OK;
        else if(!strcmp_P(Buffer, PSTR("FAIL")))
            break;
        else if(!strcmp_P(Buffer, PSTR("ERROR")))
            break;
    }
    return WLAN_ERROR; //WLAN_CheckErrors();*/
	//return WLAN_CheckErrors();
}

int8_t WLAN_Disconnect()
{
	char Buffer[16];
	UART_Puts_P(PSTR("AT+CWQAP\r\n"));
	WLAN_States &= ~(1 << WLAN_AP_CONNECTED);
	WLAN_States &= ~(1 << WLAN_TCP_CONNECTED);
	while(true)
	{
		int Bytes = UART_ReadLn(Buffer, 16, true);

		if(!Bytes)
			continue;
		else if(Bytes == -1)
			return WLAN_ERROR_UART_TIMEOUT;
		else if(!strcmp_P(Buffer, PSTR("OK")))
			return WLAN_OK;
		else if(!strcmp_P(Buffer, PSTR("WIFI DISCONNECT")))
			return WLAN_OK;
		else if(!strcmp_P(Buffer, PSTR("NOT CONNECTED")))
			return WLAN_OK;
		else if(!strcmp_P(Buffer, PSTR("FAIL")))
			break;
		else if(!strcmp_P(Buffer, PSTR("ERROR")))
			break;
	}
	return WLAN_ERROR; //WLAN_CheckErrors();*/
	//return WLAN_CheckErrors();
}

int8_t WLAN_TcpDisconnect()
{
	char Buffer[16];
	UART_Puts_P(PSTR("AT+CIPCLOSE\r\n"));
	WLAN_States &= ~(1 << WLAN_TCP_CONNECTED);
	while(true)
	{
		int Bytes = UART_ReadLn(Buffer, 16, true);

		if(!Bytes)
		continue;
		else if(Bytes == -1)
			return WLAN_ERROR_UART_TIMEOUT;
		else if(!strcmp_P(Buffer, PSTR("OK")))
			return WLAN_OK;
		else if(!strcmp_P(Buffer, PSTR("WIFI DISCONNECT")))
			return WLAN_OK;
		else if(!strcmp_P(Buffer, PSTR("NOT CONNECTED")))
			return WLAN_OK;
		else if(!strcmp_P(Buffer, PSTR("FAIL")))
			break;
		else if(!strcmp_P(Buffer, PSTR("ERROR")))
			break;
	}
	return WLAN_ERROR; //WLAN_CheckErrors();*/
	//return WLAN_CheckErrors();
	}

int8_t WLAN_Listen(RecvCallback onRecv)
{
    char Buffer[128];
	while(UART_Pending())
	{
		uint8_t Bytes = UART_ReadUntil_P(Buffer, 128, false, PSTR(":\r\n"));
		//uint8_t Bytes = UART_ReadLn(Buffer, 128, false);
		if(!Bytes)
			continue;
		else if(Bytes == -1)
		{
			WLAN_States &= ~(1 << WLAN_CHIP_OK);
			return WLAN_ERROR_UART_TIMEOUT;
		}
		else if(!strncmp_P(Buffer, PSTR("+IPD,"), 5))
		{
			//WLAN_TcpSend(Buffer);
			//char* Token = strtok_P(&Buffer[5], PSTR(","));
			//uint8_t Length = atoi(Token);
			uint8_t Length = atoi(&Buffer[5]);
			UART_ReadBytes(Buffer, Length);
			//WLAN_TcpSend(Buffer); // echo
			if(onRecv)
			    onRecv(Length, Buffer);

		}
		else if(!strcmp_P(Buffer, PSTR("WIFI DISCONNECT"))) // \r must be taken into account in spite of the UART_ReadUntil_P() implementation?
		{
			WLAN_States &= ~(1 << WLAN_AP_CONNECTED);
			WLAN_States &= ~(1 << WLAN_TCP_CONNECTED);
		}
		else if(!strcmp_P(Buffer, PSTR("CLOSED")))
		{
			WLAN_States &= ~(1 << WLAN_TCP_CONNECTED);
		}
	}
	return WLAN_OK;
}