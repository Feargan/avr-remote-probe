#ifndef UI_QUICK_SETUP_H_
#define UI_QUICK_SETUP_H_

#include "interface.h"
#include "str_util.h"

#include <avr/pgmspace.h>

extern uint8_t SelectedSensorOpt;

const char Menu_Sensor_IOQuickSetup_Text[] PROGMEM = "Quick setup";

const char ExprTemplate_Thermistor[] PROGMEM = "%a/(%b+log(3.3/%s-1)+%d)-273.15";
const char ExprTemplate_Photoresistor[] PROGMEM = "10/((3.3/%s-1)*(%a/%b))^%c";
const char ExprTemplate_Linear[] PROGMEM = "(%s-%b)*%c";
const char ExprTemplate_Hysteresis[] PROGMEM = "(%s>%b)|(%t&%s>%d)";

void Screen_QSThermistor()
{
	char ValueBuffer[10] = { 0 };
	float Value;
	char Buffer[64];
	strcpy_P(Buffer, ExprTemplate_Thermistor);
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("Beta")) == -1)
		return;
	Value = atof(ValueBuffer);
	str_replace(Buffer, PSTR("%a"), ValueBuffer); // %a -> beta
	ValueBuffer[0] = 0;
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("T0 [K]")) == -1)
		return;
	dtostrf(Value/atof(ValueBuffer), 0, 3, ValueBuffer); 
	str_replace(Buffer, PSTR("%d"), ValueBuffer); // %d -> beta/T0
	
	ValueBuffer[0] = 0;
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("Rdiv [Ohm]")) == -1)
		return;
	Value = atof(ValueBuffer);
	ValueBuffer[0] = 0;
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("RT0 [Ohm]")) == -1)
		return;
	dtostrf(log(Value/atof(ValueBuffer)), 0, 3, ValueBuffer); // %b -> log(R/RT0)
	str_replace(Buffer, PSTR("%b"), ValueBuffer);
	
	SelectedSensorOpt == 0 ? str_replace(Buffer, PSTR("%s"), "s1") : str_replace(Buffer, PSTR("%s"), "s2");
	
	if(Interface_Keyboard_P(Buffer, 64, PSTR("Expression")) != -1)
		eeprom_update_block(Buffer, &CfgExprs[SelectedSensorOpt][0], 64);
}

const char Menu_QSThermistor_Text[] PROGMEM = "Thermistor";

const struct TFunction QSThermistorFunction PROGMEM =
{
	.Function=Screen_QSThermistor,
};

const struct TMenuEntry Menu_QSThermistor PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&QSThermistorFunction,
	.Text = Menu_QSThermistor_Text,
};

/*

*/

void Screen_QSLinear()
{
	char ValueBuffer[10] = { 0 };
	char Buffer[64];
	strcpy_P(Buffer, ExprTemplate_Linear);
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("Volt. offset")) == -1)
		return;
	str_replace(Buffer, PSTR("%b"), ValueBuffer); // %b -> v_off
	ValueBuffer[0] = 0;
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("Units per Volt")) == -1)
		return;
	str_replace(Buffer, PSTR("%c"), ValueBuffer); // %c -> units per volt
	
	SelectedSensorOpt == 0 ? str_replace(Buffer, PSTR("%s"), "s1") : str_replace(Buffer, PSTR("%s"), "s2");
	
	if(Interface_Keyboard_P(Buffer, 64, PSTR("Expression")) != -1)
		eeprom_update_block(Buffer, &CfgExprs[SelectedSensorOpt][0], 64);
}

const char Menu_QSLinear_Text[] PROGMEM = "Linear";

const struct TFunction QSLinearFunction PROGMEM =
{
	.Function=Screen_QSLinear,
};

const struct TMenuEntry Menu_QSLinear PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&QSLinearFunction,
	.Text = Menu_QSLinear_Text,
};

/*

*/

void Screen_QSPhotoresistor()
{
	char ValueBuffer[10] = { 0 };
	char Buffer[64];
	strcpy_P(Buffer, ExprTemplate_Thermistor);
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("Rdiv [Ohm]")) == -1)
		return;
	str_replace(Buffer, PSTR("%a"), ValueBuffer); // %a -> R_divider
	ValueBuffer[0] = 0;
	
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("R10 [Ohm]")) == -1)
		return;
	str_replace(Buffer, PSTR("%b"), ValueBuffer); // %b -> R at 10 lux
	ValueBuffer[0] = 0;
	
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("Gamma")) == -1)
		return;
	dtostrf(1/atof(ValueBuffer), 0, 3, ValueBuffer); 
	str_replace(Buffer, PSTR("%c"), ValueBuffer); // %c -> 1/gamma
	
	SelectedSensorOpt == 0 ? str_replace(Buffer, PSTR("%s"), "s1") : str_replace(Buffer, PSTR("%s"), "s2");
	
	if(Interface_Keyboard_P(Buffer, 64, PSTR("Expression")) != -1)
	eeprom_update_block(Buffer, &CfgExprs[SelectedSensorOpt][0], 64);
}

const char Menu_QSPhotoresistor_Text[] PROGMEM = "Photoresistor";

const struct TFunction QSPhotoresistorFunction PROGMEM =
{
	.Function=Screen_QSPhotoresistor,
};

const struct TMenuEntry Menu_QSPhotoresistor PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&QSPhotoresistorFunction,
	.Text = Menu_QSPhotoresistor_Text,
};

/*

*/

const struct TMenu Menu_QuickSetupInput PROGMEM =
{
	.NumEntries=3,
	.Entries={&Menu_QSLinear, &Menu_QSThermistor, &Menu_QSPhotoresistor},
	.Title=Menu_Sensor_IOQuickSetup_Text,
};

/*

*/

void Screen_QSHysteresis()
{
	char ValueBuffer[10] = { 0 };
	char Buffer[64];
	strcpy_P(Buffer, ExprTemplate_Hysteresis);
	uint8_t Sensor = 0;
	Interface_Switch_P(PSTR("Input 1\rInput 2"), &Sensor, PSTR("Sensor"));
	if(Sensor == 0)
		strcpy_P(ValueBuffer, PSTR("s1"));
	else
		strcpy_P(ValueBuffer, PSTR("s2"));
	str_replace(Buffer, PSTR("%s"), ValueBuffer);
	
	ValueBuffer[0] = 0;
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("Lower limit")) == -1)
		return;
	str_replace(Buffer, PSTR("%c"), ValueBuffer); // %c -> lower
	ValueBuffer[0] = 0;
	if(Interface_Keyboard_P(ValueBuffer, 10, PSTR("Upper limit")) == -1)
		return;
	str_replace(Buffer, PSTR("%b"), ValueBuffer); // %b -> upper
	
	str_replace(Buffer, PSTR("%c"), ValueBuffer); // %c -> lower
	
	SelectedSensorOpt == 2 ? str_replace(Buffer, PSTR("%t"), "t1") : str_replace(Buffer, PSTR("%s"), "t2");
	
	if(Interface_Keyboard_P(Buffer, 64, PSTR("Expression")) != -1)
		eeprom_update_block(Buffer, &CfgExprs[SelectedSensorOpt][0], 64);
}

const char Menu_QSHysteresis_Text[] PROGMEM = "Hysteresis";

const struct TFunction QSHysteresisFunction PROGMEM =
{
	.Function=Screen_QSHysteresis,
};

const struct TMenuEntry Menu_QSHysteresis PROGMEM =
{
	.Flags = MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM,
	.Param = (void*)&QSHysteresisFunction,
	.Text = Menu_QSHysteresis_Text,
};

/*

*/

const struct TMenu Menu_QuickSetupOutput PROGMEM =
{
	.NumEntries=1,
	.Entries={&Menu_QSHysteresis},
	.Title=Menu_Sensor_IOQuickSetup_Text,
};

#endif /* UI_QUICK_SETUP_H_ */