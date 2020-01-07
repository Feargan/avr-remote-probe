#pragma once

#include "interface.h"

const char Menu_Main_Network_Text[] PROGMEM = "Network";

const struct TMenuEntry Menu_Main_Network PROGMEM = 
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = 0, //Interface_Network_Config;
	.Text = Menu_Main_Network_Text,
};

//

const char Menu_Main_Broker_Text[] PROGMEM = "Broker";

const struct TMenuEntry Menu_Main_Broker PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = 0,  //Interface_System_Config,
	.Text = Menu_Main_Broker_Text,
};

//

const char Menu_Main_Sensors_Text[] PROGMEM = "Sensors";

const struct TMenuEntry Menu_Main_Sensors PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = 0,  //Interface_Broker_Config,
	.Text = Menu_Main_Sensors_Text,
};


//

int8_t Test = MIN_BRIGHTNESS;

void TestScrollbar()
{
	Interface_Scrollbar_P(PSTR("Brightness"), &Test, MIN_BRIGHTNESS, MAX_BRIGHTNESS, 1);
	LCD_SetBrightness(Test);
}

const char Menu_Main_System_Text[] PROGMEM = "System";

const struct TFunction TestScrollbarFunction PROGMEM =
{
	.Function=TestScrollbar,
};

const struct TMenuEntry Menu_Main_System PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&TestScrollbarFunction,  //Interface_System_Config, // non-safe cast, but we handle it with care in the menu interfaces
	.Text = Menu_Main_System_Text,
};

//

//

int8_t BacklightBrightness = 0;

void BacklightScrollbar()
{
	Interface_Scrollbar_P(PSTR("Backlight"), &BacklightBrightness, 0, 100, 10);
	LCD_SetBacklightBrightness(BacklightBrightness);
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
	.Entries={&Menu_Main_Network, &Menu_Main_Broker,  &Menu_Main_Sensors, &Menu_Main_System, &Menu_Main_Status, &Menu_Main_Backlight},
	.Title=Menu_Main_Title,
};

inline void UI_MainMenu()
{
	Interface_Menu_P(&Menu_Main);
}