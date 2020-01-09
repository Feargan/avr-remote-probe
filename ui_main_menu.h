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

const char Menu_Main_Triggers_Text[] PROGMEM = "Triggers";

const struct TFunction TriggersFunction PROGMEM =
{
	.Function=0,//Screen_ConfigBroker,
};

const struct TMenuEntry Menu_Main_Triggers PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param =0,  //Interface_System_Config,
	.Text = Menu_Main_Triggers_Text,
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

const char Menu_Main_Status_Text[] PROGMEM = "Status";

const struct TMenuEntry Menu_Main_Status PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = 0,  //Interface_Status,
	.Text = Menu_Main_Status_Text,
};

//

const char Menu_Main_Title[] PROGMEM = "Settings";

const struct TMenu Menu_Main PROGMEM =
{
	.NumEntries=6,
	.Entries={&Menu_Main_Network, &Menu_Main_Triggers,  &Menu_Main_Sensors, &Menu_Main_Brightness, &Menu_Main_Backlight, &Menu_Main_Status},
	.Title=Menu_Main_Title,
};