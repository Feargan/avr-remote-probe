#ifndef INTERFACE_H_INCLUDED
#define INTERFACE_H_INCLUDED

#include <avr/pgmspace.h>

enum PROGMEM
{
	MENU_ENTRY_FUNCTION=0,
	MENU_ENTRY_EEPROM_STRING, // struct containing a pointer to eemem and sizeof
	MENU_ENTRY_EEPROM_INT16, // 
	MENU_ENTRY_SWITCH, // "in the entry string place all switches, \n - delimiter, param --> int* selection
	
	MENU_ENTRY_PROGMEM=0x80,
};

typedef struct TMenuEntry
{
	void* Param;
	const char* Text;
	uint8_t Flags;
} TMenuEntry;

typedef struct TFunction
{
	void (*Function)();
} TFunction;

typedef struct TEepromString
{
	uint8_t* Pointer;
	uint8_t Size;
} TEepromString;

#define MAX_MENU_ENTRIES 8

typedef struct TMenu
{
	uint8_t NumEntries;
	const struct TMenuEntry* Entries[MAX_MENU_ENTRIES];
	const char* Title;
} TMenu;

void Interface_Menu_P(const TMenu* Menu);
uint8_t Interface_Keyboard_P(char* Buffer, uint8_t Max, const char* Title);
void Interface_Scrollbar_P(const char* Name, uint8_t* Value, uint8_t Bottom, uint8_t Top, uint8_t Step, void (*update)(uint8_t));
void Interface_Switch_P(const char* Switches, uint8_t* Selection, const char* Title);

#endif