#ifndef BUTTONS_H_INCLUDED
#define BUTTONS_H_INCLUDED

#include <avr/io.h>

#include "boolean.h"

#define BUTTON_OK PD7
#define BUTTON_OK_PIN PIND
#define BUTTON_OK_PORT PORTD
#define BUTTON_OK_DDR DDRD

/*#define BUTTON_RETURN PC5
#define BUTTON_RETURN_PIN PINC
#define BUTTON_RETURN_PORT PORTC
#define BUTTON_RETURN_DDR DDRC*/

#define BUTTON_RETURN PB0
#define BUTTON_RETURN_PIN PINB
#define BUTTON_RETURN_PORT PORTB
#define BUTTON_RETURN_DDR DDRB

#define BUTTON_RIGHT PD4
#define BUTTON_RIGHT_PIN PIND
#define BUTTON_RIGHT_PORT PORTD
#define BUTTON_RIGHT_DDR DDRD

#define BUTTON_DOWN PD6
#define BUTTON_DOWN_PIN PIND
#define BUTTON_DOWN_PORT PORTD
#define BUTTON_DOWN_DDR DDRD

#define BUTTON_UP PD5
#define BUTTON_UP_PIN PIND
#define BUTTON_UP_PORT PORTD
#define BUTTON_UP_DDR DDRD

#define BUTTON_LEFT PD2
#define BUTTON_LEFT_PIN PIND
#define BUTTON_LEFT_PORT PORTD
#define BUTTON_LEFT_DDR DDRD

#define DEBOUNCE_TIME 4

void SetupButtons();
void SuspendDebouncers();
void RestoreDebouncers();
inline bool Button_OK_State()
{
    return !(BUTTON_OK_PIN&(1 << BUTTON_OK));
}
bool Button_OK_Pressed();
inline bool Button_Return_State()
{
    return !(BUTTON_RETURN_PIN&(1 << BUTTON_RETURN));
}
bool Button_Return_Pressed();
inline bool Button_Right_State()
{
    return !(BUTTON_RIGHT_PIN&(1 << BUTTON_RIGHT));
}
bool Button_Right_Pressed();
inline bool Button_Down_State()
{
    return !(BUTTON_DOWN_PIN&(1 << BUTTON_DOWN));
}
bool Button_Down_Pressed();
inline bool Button_Up_State()
{
    return !(BUTTON_UP_PIN&(1 << BUTTON_UP));
}
bool Button_Up_Pressed();
inline bool Button_Left_State()
{
    return !(BUTTON_LEFT_PIN&(1 << BUTTON_LEFT));
}
bool Button_Left_Pressed();

#endif // BUTTONS_H_INCLUDED
