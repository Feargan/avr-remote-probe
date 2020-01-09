/*
 * ui_broker.h
 *
 * Created: 08.01.2020 17:37:57
 *  Author: Piotr
 */ 


#ifndef UI_BROKER_H_
#define UI_BROKER_H_

#include "buttons.h"
#include "eecfg.h"
#include "sprites.h"
#include "lcd.h"
#include "interface.h"

void Screen_SelectAP()
{
	LCD_Clear();
	LCD_DrawText_P(0, 3, PSTR("Scanning APs.."));
	LCD_Render();
	_delay_ms(500);
	int8_t Entries = WLAN_ListAccessPoints();
	int8_t Selection = 0;
	while(!Button_Return_Pressed())
	{
		LCD_Clear();
		LCD_DrawText_P(0, 0, PSTR("AP List:"));
		for(uint8_t i = 0; i < Entries && i < 4; i++)
		{
			LCD_DrawText(0, i+2, WLAN_AccessPoints[i].SSID);
			LCD_DrawSprite_P(6*12, (i+2)*8, WLAN_AccessPoints[i].Encryption ? WlanEncrypted : WlanOpen, 6, 8);
			if(WLAN_AccessPoints[i].RSSID < -85)
			LCD_DrawSprite_P(6*13, (i+2)*8, WlanSigVeryWeak, 6, 8);
			else if(WLAN_AccessPoints[i].RSSID >= -85 && WLAN_AccessPoints[i].RSSID < -75)
			LCD_DrawSprite_P(6*13, (i+2)*8, WlanSigWeak, 6, 8);
			else if(WLAN_AccessPoints[i].RSSID >= -75 && WLAN_AccessPoints[i].RSSID < -55)
			LCD_DrawSprite_P(6*13, (i+2)*8, WlanSigGood, 6, 8);
			else if(WLAN_AccessPoints[i].RSSID >= -55)
			LCD_DrawSprite_P(6*13, (i+2)*8, WlanSigStrong, 6, 8);
			if(i == Selection)
			for(uint8_t j = 0; j<14; j++)
			LCD_InvertCell(j, i+2);
		}
		LCD_Render();
		if(Button_Up_Pressed())
		if(Selection > 0)
		Selection--;
		if(Button_Down_Pressed())
		if(Selection < Entries-1)
		Selection++;
		if(Button_OK_Pressed())
		{
			LCD_Clear();
			LCD_DrawText_P(0, 1, PSTR("Setup AP as:"));
			LCD_DrawText(0, 2, WLAN_AccessPoints[Selection].SSID);
			LCD_DrawText_P(0, 4, PSTR("Confirm/return"));
			LCD_Render();
			while(!Button_Return_Pressed())
			{
				if(Button_OK_Pressed())
				{
					char Password[64] = "\0";
					if(WLAN_AccessPoints[Selection].Encryption)
					{
						if(!Interface_Keyboard_P(Password, 64, PSTR("Enter password")))
							break;
					}

					eeprom_update_block(WLAN_AccessPoints[Selection].SSID, CfgSSID, 32);
					eeprom_update_block(Password, CfgPassword, 64);
					return;
				}
			}
		}
	}
}

const char Menu_Network_AP_Text[] PROGMEM = "Scan for APs";

const struct TFunction APFunction PROGMEM =
{
	.Function=Screen_SelectAP,
};

const struct TMenuEntry Menu_Network_APScan PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&APFunction,
	.Text = Menu_Network_AP_Text,
};

/*

*/

void Screen_ManualSSID()
{
	char SSID[32];
	eeprom_read_block(SSID, CfgSSID, 32);
	if(Interface_Keyboard_P(SSID, 32, PSTR("Enter SSID")))
		eeprom_update_block(SSID, CfgSSID, 32);
}

const char Menu_Network_ManualSSID_Text[] PROGMEM = "Manual SSID";

const struct TFunction ManualSSIDFunction PROGMEM =
{
	.Function=Screen_ManualSSID,
};

const struct TMenuEntry Menu_Network_ManualSSID PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&ManualSSIDFunction,
	.Text = Menu_Network_ManualSSID_Text,
};

/*

*/

void Screen_Password()
{
	char Password[64];
	eeprom_read_block(Password, CfgPassword, 64);
	if(Interface_Keyboard_P(Password, 64, PSTR("Enter password")))
		eeprom_update_block(Password, CfgPassword, 64);
}

const char Menu_Network_Password_Text[] PROGMEM = "Password";

const struct TFunction PasswordFunction PROGMEM =
{
	.Function=Screen_Password,
};

const struct TMenuEntry Menu_Network_Password PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&PasswordFunction,
	.Text = Menu_Network_Password_Text,
};

/*

*/

void Screen_Broker()
{
	char Address[64];
	eeprom_read_block(Address, CfgBrokerAddr, 64);
	if(Interface_Keyboard_P(Address, 64, PSTR("Enter address")))
	eeprom_update_block(Address, CfgBrokerAddr, 64);
}

const char Menu_Network_Broker_Text[] PROGMEM = "Broker IP";

const struct TFunction BrokerFunction PROGMEM =
{
	.Function=Screen_Broker,
};

const struct TMenuEntry Menu_Network_Broker PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&BrokerFunction,
	.Text = Menu_Network_Broker_Text,
};

/*

*/
const char Menu_Network_Connection_Text[] PROGMEM = "Connection";

void Screen_Connection()
{
	uint8_t Sel = (eeprom_read_byte(&CfgSwitches)>>CFG_NETWORK_ENABLED)&1;
	Interface_Switch_P(PSTR("DISABLED\rENABLED"), &Sel, Menu_Network_Connection_Text);
	if(Sel)
		Sel = eeprom_read_byte(&CfgSwitches)|(Sel<<CFG_NETWORK_ENABLED);
	else
		Sel = eeprom_read_byte(&CfgSwitches)&(~(Sel<<CFG_NETWORK_ENABLED));
	eeprom_update_byte(&CfgSwitches, Sel);
}

const struct TFunction ConnectionFunction PROGMEM =
{
	.Function=Screen_Connection,
};

const struct TMenuEntry Menu_Network_Connection PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&ConnectionFunction,
	.Text = Menu_Network_Connection_Text,
};

/*

*/

const char Menu_Network_Test_Text[] PROGMEM = "Test";

const struct TFunction TestFunction PROGMEM =
{
	.Function=0,
};

const struct TMenuEntry Menu_Network_Test PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = 0,
	.Text = Menu_Network_Test_Text,
};

/*

*/

const char Menu_Network_Text[] PROGMEM = "Network";

const struct TMenu Menu_Network PROGMEM =
{
	.NumEntries=6,
	.Entries={&Menu_Network_APScan, &Menu_Network_ManualSSID, &Menu_Network_Password, &Menu_Network_Broker, &Menu_Network_Connection, &Menu_Network_Test},
	.Title=Menu_Network_Text,
};

/*static void Screen_SelectAP()
{
	LCD_Clear();
	LCD_DrawText_P(0, 3, PSTR("Scanning APs.."));
	LCD_Render();
	_delay_ms(500);
	int8_t Entries = WLAN_ListAccessPoints();
	int8_t Selection = 0;
	uint8_t ScreenLock = 1;
	while(!Button_Return_Pressed())
	{
		LCD_Clear();
		LCD_DrawText_P(0, 0, PSTR("AP List:"));
		for(uint8_t i = 0; i < Entries && i < 4; i++)
		{
			LCD_DrawText(0, i+2, WLAN_AccessPoints[i].SSID);
			LCD_DrawSprite_P(6*12, (i+2)*8, WLAN_AccessPoints[i].Encryption ? WlanEncrypted : WlanOpen, 6, 8);
			if(WLAN_AccessPoints[i].RSSID < -85)
			LCD_DrawSprite_P(6*13, (i+2)*8, WlanSigVeryWeak, 6, 8);
			else if(WLAN_AccessPoints[i].RSSID >= -85 && WLAN_AccessPoints[i].RSSID < -75)
			LCD_DrawSprite_P(6*13, (i+2)*8, WlanSigWeak, 6, 8);
			else if(WLAN_AccessPoints[i].RSSID >= -75 && WLAN_AccessPoints[i].RSSID < -55)
			LCD_DrawSprite_P(6*13, (i+2)*8, WlanSigGood, 6, 8);
			else if(WLAN_AccessPoints[i].RSSID >= -55)
			LCD_DrawSprite_P(6*13, (i+2)*8, WlanSigStrong, 6, 8);
			if(i == Selection)
			for(uint8_t j = 0; j<14; j++)
			LCD_InvertCell(j, i+2);
		}
		LCD_Render();
		if(Button_Up_Pressed())
		if(Selection > 0)
		Selection--;
		if(Button_Down_Pressed())
		if(Selection < Entries-1)
		Selection++;
		if(Button_OK_Pressed())
		{
			LCD_Clear();
			LCD_DrawText_P(0, 1, PSTR("Setup AP as:"));
			LCD_DrawText(0, 2, WLAN_AccessPoints[Selection].SSID);
			LCD_DrawText_P(0, 4, PSTR("Confirm/return"));
			LCD_Render();
			while(!Button_Return_Pressed())
			{
				if(Button_OK_Pressed())
				{
					char Password[64] = "\0";
					if(WLAN_AccessPoints[Selection].Encryption)
					{
						if(!Interface_Keyboard_P(Password, 64, PSTR("Enter password")))
						break;
					}
					LCD_Clear();
					LCD_DrawText_P(0, 2, PSTR("     Testing"));
					LCD_DrawText_P(0, 3, PSTR("connection..."));
					LCD_Render();
					LCD_Clear();
					_delay_ms(500);
					switch(WLAN_JoinAccessPoint(WLAN_AccessPoints[Selection].SSID, Password))
					{
						case WLAN_ERROR:
						LCD_DrawText_P(0, 2, PSTR("Failed to           connect"));
						break;
						case WLAN_OK:
						LCD_DrawText_P(0, 2, PSTR("  Connected!  "));
						eeprom_update_block(WLAN_AccessPoints[Selection].SSID, CfgSSID, 32);
						eeprom_update_block(Password, CfgPassword, 64);
						ScreenLock = false;
						break;
					}
					LCD_Render();
					_delay_ms(1000);
					break;
				}
			}
			if(!ScreenLock)
			break;
		}
	}
}*/

static void Screen_ConfigAP()
{
	int8_t Selection = 0;
	eeprom_read_block(WLAN_AccessPoints[0].SSID, CfgSSID, 32);
	while(!Button_Return_Pressed())
	{
		LCD_Clear();
		LCD_DrawText_P(0, 0, PSTR("Network"));
		//if(WLAN_AccessPoints[0].SSID[0])
		if((char)eeprom_read_byte((uint8_t*)&CfgSSID[0]) != '\0')
		LCD_DrawText(0, 2, WLAN_AccessPoints[0].SSID);
		else
		LCD_DrawText_P(0, 2, PSTR("Not configured"));
		LCD_DrawText_P(0, 4, PSTR("Connect"));
		LCD_DrawText_P(0, 5, PSTR("Change"));
		for(uint8_t i = 0; i<14; i++)
		LCD_InvertCell(i, Selection+4);
		LCD_Render();
		if(Button_Up_Pressed())
		if(Selection > 0)
		Selection--;
		if(Button_Down_Pressed())
		if(Selection < 1)
		Selection++;
		if(Button_OK_Pressed())
		{
			switch(Selection)
			{
				case 0:
				if((char)eeprom_read_byte((uint8_t*)&CfgSSID[0]) != '\0')
				{
					char Password[64];
					eeprom_read_block(WLAN_AccessPoints[0].SSID, CfgSSID, sizeof(WLAN_AccessPoints[0].SSID));
					eeprom_read_block(Password, CfgPassword, sizeof(Password));
					LCD_Clear();
					LCD_DrawText_P(0, 2, PSTR("Connecting..."));
					LCD_Render();
					LCD_Clear();
					_delay_ms(500);
					switch(WLAN_JoinAccessPoint(WLAN_AccessPoints[0].SSID, Password))
					{
						case WLAN_ERROR:
						LCD_DrawText_P(0, 2, PSTR("Failed to           connect"));
						break;
						case WLAN_OK:
						LCD_DrawText_P(0, 2, PSTR("  Connected!  "));
						break;
					}
					LCD_Render();
					_delay_ms(1000);
				}
				else
				{
					LCD_Clear();
					LCD_DrawText_P(0, 2, PSTR("Not configured"));
					LCD_Render();
					_delay_ms(1000);
				}
				break;
				case 1:
				Screen_SelectAP();
				eeprom_read_block(WLAN_AccessPoints[0].SSID, CfgSSID, 32);
				break;
			}
		}
	}
}

static void Screen_ConfigBroker()
{
	char BrokerAddr[32] = { 0 };
	eeprom_read_block(BrokerAddr, CfgBrokerAddr, 32);
	int8_t Selection = 0;
	while(!Button_Return_Pressed())
	{
		LCD_Clear();
		LCD_DrawText_P(0, 0, PSTR("Remote broker"));
		//if(BrokerAddr[0])
		if((char)eeprom_read_byte((uint8_t*)&CfgBrokerAddr[0]) != '\0')
		LCD_DrawText(0, 2, BrokerAddr);
		else
		LCD_DrawText_P(0, 2, PSTR("Not configured"));
		LCD_DrawText_P(0, 4, PSTR("Connect"));
		LCD_DrawText_P(0, 5, PSTR("Change"));
		for(uint8_t i = 0; i<14; i++)
		LCD_InvertCell(i, Selection+4);
		LCD_Render();
		if(Button_Up_Pressed())
		if(Selection > 0)
		Selection--;
		if(Button_Down_Pressed())
		if(Selection < 1)
		Selection++;
		if(Button_OK_Pressed())
		{
			//if(Selection == 0 && !BrokerAddr[0])
			if(Selection == 0 && (char)eeprom_read_byte((uint8_t*)&CfgBrokerAddr[0]) == '\0')
			{
				LCD_Clear();
				LCD_DrawText_P(0, 2, PSTR("Not configured"));
				LCD_Render();
				_delay_ms(1000);
				continue;
			}
			else if(Selection == 1)
			{
				if(!Interface_Keyboard_P(BrokerAddr, sizeof(BrokerAddr), PSTR("Broker address")))
				{
					eeprom_read_block(BrokerAddr, CfgBrokerAddr, 32);
					continue;
				}
			}
			LCD_Clear();
			LCD_DrawText_P(0, 2, PSTR("Connecting.."));
			LCD_Render();
			_delay_ms(500);
			LCD_Clear();
			if(WLAN_TcpConnect(BrokerAddr, eeprom_read_word(&CfgBrokerPort)) == 0)
			{
				LCD_DrawText_P(0, 3, PSTR("Connected!"));
				WLAN_TcpSend_P(PSTR("REGISTER test"));
				eeprom_update_block(BrokerAddr, CfgBrokerAddr, 32);
			}
			else
			LCD_DrawText_P(0, 3, PSTR("Failed to connect"));
			LCD_Render();
			_delay_ms(1000);
			break;
		}
	}
}



#endif /* UI_BROKER_H_ */