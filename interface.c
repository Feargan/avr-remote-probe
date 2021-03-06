#include "interface.h"
#include "buttons.h"
#include "lcd.h"
#include "kblayout.h"
#include "string.h"

int16_t Interface_Keyboard_P(char* Buffer, uint8_t Max, const char* Title)
{
	Max--;
	const uint8_t MaxView = Max > 14 ? 14 : Max;
	int8_t x = 0, y = 1;
	uint8_t Shift = 0;
	uint8_t Length = strlen(Buffer);
	uint8_t View = Length > 14 ? Length-14 : 0;
	uint8_t Cursor = Length;
	while(1)
	{
		LCD_Clear();
		uint8_t i;
		//LCD_DrawText(0, 0, &Buffer[(Length > 14) ? Length-14 : 0]);
		for (int i = 0; i < (Length-View >= MaxView ? MaxView : Length-View); i++)
		{
			// LCD_DrawLetter()
			char Restore = Buffer[i+View+1];
			Buffer[i+View+1] = '\0';
			LCD_DrawText(i*6, 0, &Buffer[i+View]);
			Buffer[i+View+1] = Restore;
			//
			if (View + i == Cursor)
				View != Cursor ? LCD_SetDramByte(i*6-1, 0, 0x7e) : LCD_SetDramByte(0, 0, 0x7e);
		}
		//if (Cursor == Length || View+MaxView == Cursor)
		//	Cursor == View ? LCD_SetDramByte(0, 0, 0x7e) : LCD_SetDramByte((Cursor-View)*6-1, 0, 0x7e); //
		LCD_DrawText_P(0, 1, Title);
		for(i = 0; i<14; i++)
		LCD_InvertCell(i, 1);
		if(!Shift)
			for(i = 0; i < 4; i++)
				LCD_DrawText_P(0, 2+i, KbLayout[i]);
		else
			for(i = 0; i < 4; i++)
				LCD_DrawText_P(0, 2+i, KbLayoutShift[i]);
		LCD_InvertCell(x, 2+y);
		if(Shift&SHIFTMODE_ONCE)
			LCD_InvertCell(0, 5);
		if(Shift&SHIFTMODE_ON)
			LCD_InvertCell(0, 4);
		LCD_Render();
		
		while(1)
		{
			if(Button_Return_Pressed())
			{
				//Buffer[Length] = '\0';
				return -1;
			}
			if(Button_Down_Pressed())
			{
				if(y < 3)
					y++;
				break;
			}
			if(Button_Right_Pressed())
			{
				if(x < 13)
					x++;
				break;
			}
			if(Button_Up_Pressed())
			{
				if(y > 0)
					y--;
				break;
			}
			if(Button_Left_Pressed())
			{
				if(x > 0)
					x--;
				break;
			}
			if(Button_OK_Pressed())
			{
				char Ch = pgm_read_byte(Shift ? &KbLayoutShift[y][x] : &KbLayout[y][x]);
				if(Ch >= ' ' && Length < Max)
				{
					if (Cursor != Length)
					{
						memmove(&Buffer[Cursor+1], &Buffer[Cursor], Length - Cursor);
					}
					Buffer[Cursor] = Ch;
					Cursor++;
					Length++;
					if(Shift&SHIFTMODE_ONCE)
						Shift = 0;
				}
				else
				{
					switch(Ch)
					{
						case 0x1:
							if (Cursor > 0)
							{
								Length--;
								Cursor--;
								if (Cursor != Length)
								{
									memmove(&Buffer[Cursor], &Buffer[Cursor+1], Length - Cursor);
								}
								if (View > 0)
									View--;
							}
							break;
						case 0x2:
							if(!Shift)
								Shift |= SHIFTMODE_ON;
							else
								Shift = 0;
							break;
						case 0x3:
							if(!Shift)
								Shift |= SHIFTMODE_ONCE;
							else
								Shift = 0;
							break;
						case 0x4:
							Buffer[Length] = '\0';
							return Length;
							break;
						case 0x5:
							if (Cursor > 0)
								Cursor--;
							break;
						case 0x6:
							if (Cursor < Length)
								Cursor++;
							break;
					}
				}
				break;
			}
		}
		if (Cursor > View+MaxView)
			View++;
		if (Cursor < View)
			View--;
	}
	//Buffer[Length] = '\0';
	return -1;
}

inline static const TMenuEntry* getMenuEntry(const TMenu* Menu, uint8_t i)
{
	return (const TMenuEntry*)pgm_read_ptr(&Menu->Entries[i]);
}

void Interface_Menu_P(const TMenu* Menu)
{
	int8_t Selection = 0;
	int8_t View = Selection;
	uint8_t NumEntries =  pgm_read_byte(&Menu->NumEntries);
	while(1)
	{
		LCD_Clear();
		LCD_DrawText_P(0, 0, pgm_read_ptr(&Menu->Title));
		for(uint8_t i = 0; i<((NumEntries>4) ? 4 : NumEntries); i++)
			LCD_DrawText_P(0, i+2, pgm_read_ptr(&getMenuEntry(Menu, i+View)->Text ) );
		for(uint8_t i = 0; i<14; i++)
			LCD_InvertCell(i, (Selection-View)+2);
		LCD_Render();
		
		if(Button_Up_Pressed())
			if(Selection > 0)
			{
				Selection--;
				//break; // or inputLoop = false;
			}
		if(Button_Down_Pressed())
			if(Selection < NumEntries-1)
			{
				Selection++;
				//break; // inputLoop = false;
			}
		if(Button_OK_Pressed())
		{
			//const TFunction* Callback = (const TFunction*)pgm_read_ptr(&getMenuEntry(Menu, Selection)->Param );
			if(pgm_read_ptr(&getMenuEntry(Menu, Selection)->Param) != 0 && (uint8_t)pgm_read_byte(&getMenuEntry(Menu, Selection)->Flags)==(MENU_ENTRY_FUNCTION|MENU_ENTRY_PROGMEM))
			{
				//Callback->Function();
				void (*Function)() = pgm_read_ptr(&((const TFunction*)pgm_read_ptr(&getMenuEntry(Menu, Selection)->Param))->Function);
				Function();
			}
			//break;
		}
		if(Button_Return_Pressed())
			return;
		
		if(View+3 < Selection) // follow the lower selection
			View++;
		if(Selection < View) // follow the upper selection
			View--;
		
		
	}
}

void Interface_Scrollbar_P(const char* Name, uint8_t* Value, uint8_t Bottom, uint8_t Top, uint8_t Step, void (*update)(uint8_t))
{
	int8_t Prev = *Value;
	while(1)
	{
		/*(if(*Value > Top)
			*Value = Top;
		if(*Value < Bottom)
			*Value = Bottom;*/
		
		LCD_Clear();
		LCD_DrawText_P(6, 1, Name);
		LCD_SetDramByte(6, 4, 0xff); // scrollbar startpos
		uint8_t i = 1;
		const uint16_t Fill = 70*(*Value-Bottom)/(Top-Bottom); // why 70? -> 84 - display width, 6 - cell width, 72 - scrollbar total width, 70 - scrollbar fill area
		for(; i<Fill; i++)
			LCD_SetDramByte(i+6, 4, 0xff); // fill
		for(; i<70; i++)
			LCD_SetDramByte(i+6, 4, 0x81); // empty;
		LCD_SetDramByte(76, 4, 0xff); // finish border
		//LCD_DrawText() // display current value
		LCD_Render();
		
		if(Button_Return_Pressed())
		{
			*Value = Prev;
			update(*Value);
			return;	
		}
		if(Button_OK_Pressed())
			return;
		if(Button_Left_Pressed())
		{
			if(*Value >= Step && *Value-Step >= Bottom)
				*Value-=Step;
			else
				*Value=Bottom;
			update(*Value);
		}
		if(Button_Right_Pressed())
		{
			if(*Value+Step <= Top)
				*Value+=Step;
			else
				*Value=Top;
			update(*Value);
		}
	}
}

void Interface_Switch_P(const char* Switches, uint8_t* Selection, const char* Title) // maybe return int instead of using this pointer
{
	uint8_t NumSwitches = 1;
	uint8_t Prev = *Selection;
	char Buffer[64] = { 0 };
	strcpy_P(Buffer, Switches);
	char* Tokens[8];
	Tokens[0] = strtok(Buffer, "\r");
	while (NumSwitches < 8 && (Tokens[NumSwitches] = strtok(0, "\r")))
		NumSwitches++;
	while (1)
	{
		LCD_Clear();
		LCD_DrawText(6, 3, Tokens[*Selection]);
		LCD_DrawText_P(1, 1, Title);
		LCD_Render();
		if(Button_Return_Pressed())
		{
			*Selection = Prev;
			return;
		}
		if(Button_OK_Pressed())
			return;
		if(Button_Left_Pressed())
		{
			if(*Selection > 0)
			{
				(*Selection)--;
			}
		}
		if(Button_Right_Pressed())
			if(*Selection < NumSwitches-1)
				(*Selection)++;
	}
	
}