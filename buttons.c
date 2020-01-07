#include <avr/interrupt.h>

#include "buttons.h"

#include "boolean.h"

typedef struct
{
    uint8_t Cycles:3;
    uint8_t PrevState:1;
    uint8_t NewState:1;
} BtnDebouncer;

volatile BtnDebouncer DbOK, DbReturn, DbRight, DbDown, DbUp, DbLeft;

static void ZeroDebouncer(volatile BtnDebouncer* pDb)
{
    pDb->Cycles = 0;
    pDb->PrevState = 0;
    pDb->NewState = 0;
}

static void SetPullupInput(volatile uint8_t* DDR, volatile uint8_t* Port, uint8_t Pin)
{
    *DDR &= ~(1 << Pin);
    *Port |= (1 << Pin);
}

ISR(TIMER0_OVF_vect)
{
    if(Button_OK_State() != DbOK.PrevState)
    {
        DbOK.Cycles++;
        if(DbOK.Cycles >= 4)
        {
            DbOK.Cycles = 0;
            DbOK.NewState = 1;
            DbOK.PrevState = Button_OK_State();
        }
    }
    else
        DbOK.Cycles = 0;
    if(Button_Return_State() != DbReturn.PrevState)
    {
        DbReturn.Cycles++;
        if(DbReturn.Cycles >= 4)
        {
            DbReturn.Cycles = 0;
            DbReturn.NewState = 1;
            DbReturn.PrevState = Button_Return_State();
        }
    }
    else
        DbReturn.Cycles = 0;
    if(Button_Right_State() != DbRight.PrevState)
    {
        DbRight.Cycles++;
        if(DbRight.Cycles >= 4)
        {
            DbRight.Cycles = 0;
            DbRight.NewState = 1;
            DbRight.PrevState = Button_Right_State();
        }
    }
    else
        DbRight.Cycles = 0;
    if(Button_Down_State() != DbDown.PrevState)
    {
        DbDown.Cycles++;
        if(DbDown.Cycles >= 4)
        {
            DbDown.Cycles = 0;
            DbDown.NewState = 1;
            DbDown.PrevState = Button_Down_State();
        }
    }
    else
        DbDown.Cycles = 0;
    if(Button_Up_State() != DbUp.PrevState)
    {
        DbUp.Cycles++;
        if(DbUp.Cycles >= 4)
        {
            DbUp.Cycles = 0;
            DbUp.NewState = 1;
            DbUp.PrevState = Button_Up_State();
        }
    }
    else
        DbUp.Cycles = 0;
    if(Button_Left_State() != DbLeft.PrevState)
    {
        DbLeft.Cycles++;
        if(DbLeft.Cycles >= 4)
        {
            DbLeft.Cycles = 0;
            DbLeft.NewState = 1;
            DbLeft.PrevState = Button_Left_State();
        }
    }
    else
        DbLeft.Cycles = 0;
}

void SetupButtons()
{
    TCCR0B |= (1 << CS01);
    TIMSK0 |= (1 << TOIE0);

    SetPullupInput(&BUTTON_OK_DDR, &BUTTON_OK_PORT, BUTTON_OK);
    SetPullupInput(&BUTTON_RETURN_DDR, &BUTTON_RETURN_PORT, BUTTON_RETURN);
    SetPullupInput(&BUTTON_RIGHT_DDR, &BUTTON_RIGHT_PORT, BUTTON_RIGHT);
    SetPullupInput(&BUTTON_DOWN_DDR, &BUTTON_DOWN_PORT, BUTTON_DOWN);
    SetPullupInput(&BUTTON_UP_DDR, &BUTTON_UP_PORT, BUTTON_UP);
    SetPullupInput(&BUTTON_LEFT_DDR, &BUTTON_LEFT_PORT, BUTTON_LEFT);

    ZeroDebouncer(&DbOK);
    ZeroDebouncer(&DbReturn);
    ZeroDebouncer(&DbRight);
    ZeroDebouncer(&DbDown);
    ZeroDebouncer(&DbUp);
    ZeroDebouncer(&DbLeft);
    sei();
}

void SuspendDebouncers()
{
    TIMSK0 &= ~(1 << TOIE0);
}

void RestoreDebouncers()
{
    TIMSK0 |= (1 << TOIE0);
}

bool Button_OK_Pressed()
{
    if(DbOK.NewState)
    {
        DbOK.NewState = 0;
        return DbOK.PrevState;
    }

    return 0;
}

bool Button_Return_Pressed()
{
    if(DbReturn.NewState)
    {
        DbReturn.NewState = 0;
        return DbReturn.PrevState;
    }

    return 0;
}

bool Button_Right_Pressed()
{
    if(DbRight.NewState)
    {
        DbRight.NewState = 0;
        return DbRight.PrevState;
    }

    return 0;
}

bool Button_Down_Pressed()
{
    if(DbDown.NewState)
    {
        DbDown.NewState = 0;
        return DbDown.PrevState;
    }

    return 0;
}

bool Button_Up_Pressed()
{
    if(DbUp.NewState)
    {
        DbUp.NewState = 0;
        return DbUp.PrevState;
    }

    return 0;
}

bool Button_Left_Pressed()
{
    if(DbLeft.NewState)
    {
        DbLeft.NewState = 0;
        return DbLeft.PrevState;
    }

    return 0;
}
