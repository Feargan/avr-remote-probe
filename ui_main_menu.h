#pragma once

#include "interface.h"
#include "ui_broker.h"
#include "ui_sensors.h"

void Network_Menu()
{
	Interface_Menu_P(&Menu_Network);
}

const struct TFunction ConfigApFunction PROGMEM =
{
	.Function=Network_Menu,
};

const struct TMenuEntry Menu_Main_Network PROGMEM = 
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&ConfigApFunction, //Interface_Network_Config;
	.Text = Menu_Network_Text,
};

//

void Sensors_Menu()
{
	Interface_Menu_P(&Menu_Sensors);
}

const struct TFunction SensorsMenuFunction PROGMEM =
{
	.Function=Sensors_Menu,
};

const struct TMenuEntry Menu_Main_Sensors PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&SensorsMenuFunction,  //Interface_Broker_Config,
	.Text = Menu_Sensors_Text,
};


//

void Screen_SystemName()
{
	char Name[16];
	eeprom_read_block(Name, CfgSystemName, 16);
	if(Interface_Keyboard_P(Name, 16, PSTR("Enter name")))
		eeprom_update_block(Name, CfgSystemName, 16);
}

const char Menu_Main_System_Text[] PROGMEM = "System name";

const struct TFunction SystemFunction PROGMEM =
{
	.Function=Screen_SystemName,
};

const struct TMenuEntry Menu_Main_System PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&SystemFunction,  //Interface_Broker_Config,
	.Text = Menu_Main_System_Text,
};


//

void BrightnessScrollbar()
{
	uint8_t Brightness = eeprom_read_byte(&CfgBrightness);
	Interface_Scrollbar_P(PSTR("Brightness"), &Brightness, MIN_BRIGHTNESS, MAX_BRIGHTNESS, 1, LCD_SetBrightness);
	eeprom_update_byte(&CfgBrightness, Brightness);
}

const char Menu_Main_Brightness_Text[] PROGMEM = "Brightness";

const struct TFunction BrightnessScrollbarFunction PROGMEM =
{
	.Function=BrightnessScrollbar,
};

const struct TMenuEntry Menu_Main_Brightness PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&BrightnessScrollbarFunction,  //Interface_System_Config, // non-safe cast, but we handle it with care in the menu interfaces
	.Text = Menu_Main_Brightness_Text,
};

//

//

void BacklightScrollbar()
{
	uint8_t Brightness = eeprom_read_byte(&CfgBacklight);
	Interface_Scrollbar_P(PSTR("Backlight"), &Brightness, 0, 100, 10, LCD_SetBacklightBrightness);
	eeprom_update_byte(&CfgBacklight, Brightness);
}

const char Menu_Main_Backlight_Text[] PROGMEM = "Backlight";

const struct TFunction BacklightScrollbarFunction PROGMEM =
{
	.Function=BacklightScrollbar,
};

const struct TMenuEntry Menu_Main_Backlight PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&BacklightScrollbarFunction,  //Interface_System_Config, // non-safe cast, but we handle it with care in the menu interfaces
	.Text = Menu_Main_Backlight_Text,
};

//

const char Menu_Main_Title[] PROGMEM = "Settings";

const struct TMenu Menu_Main PROGMEM =
{
	.NumEntries=5,
	.Entries={&Menu_Main_Network,  &Menu_Main_Sensors, &Menu_Main_System, &Menu_Main_Brightness, &Menu_Main_Backlight},
	.Title=Menu_Main_Title,
};