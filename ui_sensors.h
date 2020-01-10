#ifndef UI_SENSORS_H_
#define UI_SENSORS_H_

#include "lcd.h"
#include "interface.h"
#include "eecfg.h"

uint8_t SelectedSensorOpt;

const char Menu_Sensors_Text[] PROGMEM = "Sensors";


// switch
// expression
// name

const char Menu_Sensor_IOSwitch_Text[] PROGMEM = "Switch";

void Screen_IOSwitch()
{
	uint8_t Sel = (eeprom_read_byte(&CfgSwitches)>>SelectedSensorOpt)&1;
		Interface_Switch_P(PSTR("DISABLED\rENABLED"), &Sel, Menu_Sensor_IOSwitch_Text);
	if(Sel)
		Sel = eeprom_read_byte(&CfgSwitches)|(1<<SelectedSensorOpt);
	else
		Sel = eeprom_read_byte(&CfgSwitches)&(~(1<<SelectedSensorOpt));
	eeprom_update_byte(&CfgSwitches, Sel);
}

const struct TFunction IOSwitchFunction PROGMEM =
{
	.Function=Screen_IOSwitch,
};

const struct TMenuEntry Menu_Sensor_IOSwitch PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&IOSwitchFunction,
	.Text = Menu_Sensor_IOSwitch_Text,
};

/*

*/

void Screen_IOName()
{
	char Name[8];
	eeprom_read_block(Name, &CfgSensorNames[SelectedSensorOpt][0], 8);
	if(Interface_Keyboard_P(Name, 8, PSTR("Enter name")))
		eeprom_update_block(Name, &CfgSensorNames[SelectedSensorOpt][0], 8);
}

const char Menu_Sensor_IOName_Text[] PROGMEM = "Name";

const struct TFunction IONameFunction PROGMEM =
{
	.Function=Screen_IOName,
};

const struct TMenuEntry Menu_Sensor_IOName PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&IONameFunction,
	.Text = Menu_Sensor_IOName_Text,
};

/*

*/

void Screen_IOExpression()
{
	char Expression[64];
	eeprom_read_block(Expression, &CfgExprs[SelectedSensorOpt][0], 64);
	if(Interface_Keyboard_P(Expression, 64, PSTR("Expression")))
		eeprom_update_block(Expression, &CfgExprs[SelectedSensorOpt][0], 64);
}

const char Menu_Sensor_IOExpression_Text[] PROGMEM = "Expression";

const struct TFunction IOExpressionFunction PROGMEM =
{
	.Function=Screen_IOExpression,
};

const struct TMenuEntry Menu_Sensor_IOExpression PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&IOExpressionFunction,
	.Text = Menu_Sensor_IOExpression_Text,
};

/*

*/

const struct TMenu Menu_SensorConfig PROGMEM =
{
	.NumEntries=3,
	.Entries={&Menu_Sensor_IOSwitch, &Menu_Sensor_IOName, &Menu_Sensor_IOExpression},
	.Title=Menu_Sensors_Text,
};

/*

*/

void Screen_Sensor0()
{
	SelectedSensorOpt = 0;
	Interface_Menu_P(&Menu_SensorConfig);
}

const char Menu_Sensor0_Text[] PROGMEM = "Input 1";

const struct TFunction Sensor0Function PROGMEM =
{
	.Function=Screen_Sensor0,
};

const struct TMenuEntry Menu_Sensor0 PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&Sensor0Function,
	.Text = Menu_Sensor0_Text,
};

/*

*/

void Screen_Sensor1()
{
	SelectedSensorOpt = 1;
	Interface_Menu_P(&Menu_SensorConfig);
}

const char Menu_Sensor1_Text[] PROGMEM = "Input 2";

const struct TFunction Sensor1Function PROGMEM =
{
	.Function=Screen_Sensor1,
};

const struct TMenuEntry Menu_Sensor1 PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&Sensor1Function,
	.Text = Menu_Sensor1_Text,
};

/*

*/

void Screen_Trigger0()
{
	SelectedSensorOpt = 2;
	Interface_Menu_P(&Menu_SensorConfig);
}

const char Menu_Trigger0_Text[] PROGMEM = "Output 1";

const struct TFunction Trigger0Function PROGMEM =
{
	.Function=Screen_Trigger0,
};

const struct TMenuEntry Menu_Trigger0 PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&Trigger0Function,
	.Text = Menu_Trigger0_Text,
};

/*

*/

void Screen_Trigger1()
{
	SelectedSensorOpt = 3;
	Interface_Menu_P(&Menu_SensorConfig);
}

const char Menu_Trigger1_Text[] PROGMEM = "Output 2";

const struct TFunction Trigger1Function PROGMEM =
{
	.Function=Screen_Trigger1,
};

const struct TMenuEntry Menu_Trigger1 PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&Trigger1Function,
	.Text = Menu_Trigger1_Text,
};

/*

*/

const struct TMenu Menu_Sensors PROGMEM =
{
	.NumEntries=4,
	.Entries={&Menu_Sensor0, &Menu_Sensor1, &Menu_Trigger0, &Menu_Trigger1},
	.Title=Menu_Sensors_Text,
};

#endif