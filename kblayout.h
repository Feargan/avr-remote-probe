#ifndef KBLAYOUT_H_INCLUDED
#define KBLAYOUT_H_INCLUDED

#define SHIFTMODE_ON 0x1
#define SHIFTMODE_ONCE 0x2

const char KbLayout[4][14] PROGMEM =
{
    { "`1234567890-=\x01"       },
    { " qwertyuiop[]\x04"       },
    { "\x02 asdfghjkl;'\\"      },
    { "\x03 zxcvbnm,./ \x05"    },
};

const char KbLayoutShift[4][14] PROGMEM =
{
    { "~!@#$%^&*()_+\x01"       },
    { " QWERTYUIOP{}\x04"       },
    { "\x02 ASDFGHJKL:\"|"      },
    { "\x03 ZXCVBNM<>? \x05"    },
};

#endif // KBLAYOUT_H_INCLUDED
