#ifndef UI_SENSORS_H_
#define UI_SENSORS_H_

#include "lcd.h"
#include "interface.h"
#include "eecfg.h"

uint8_t SelectedSensor;

const char Menu_Sensors_Text[] PROGMEM = "Sensors";


// analog input
// expression

const struct TMenu Menu_SensorConfig PROGMEM =
{
	.NumEntries=0,
	.Entries={NULL},
	.Title=Menu_Sensors_Text,
};

/*

*/

void Screen_Sensor0()
{
	SelectedSensor = 0;
	Interface_Menu_P(&Menu_SensorConfig);
}

const char Menu_Sensor_0_Text[] PROGMEM = "Input 1";

const struct TFunction Sensor0Function PROGMEM =
{
	.Function=Screen_Sensor0,
};

const struct TMenuEntry Menu_Sensor0 PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&Sensor0Function,
	.Text = Menu_Sensor_0_Text,
};

/*

*/

void Screen_Sensor1()
{
	SelectedSensor = 1;
	Interface_Menu_P(&Menu_SensorConfig);
}

const char Menu_Sensor_1_Text[] PROGMEM = "Input 2";

const struct TFunction Sensor1Function PROGMEM =
{
	.Function=Screen_Sensor1,
};

const struct TMenuEntry Menu_Sensor1 PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&Sensor1Function,
	.Text = Menu_Sensor_1_Text,
};

/*

*/

const struct TMenu Menu_Sensors PROGMEM =
{
	.NumEntries=2,
	.Entries={&Menu_Sensor0, &Menu_Sensor1},
	.Title=Menu_Sensors_Text,
};

#endif