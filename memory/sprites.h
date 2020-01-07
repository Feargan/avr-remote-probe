#ifndef SPRITES_H_INCLUDED
#define SPRITES_H_INCLUDED

#include <avr/pgmspace.h>

const uint8_t WlanEncrypted[] PROGMEM = { 0xf8,0x8e,0xe9,0xa9,0x8e,0xf8 };
const uint8_t WlanOpen[] PROGMEM = { 0xf8,0x8e,0xe9,0xa9,0x88,0xf8 };
const uint8_t WlanSigVeryWeak[] PROGMEM = { 0x00,0x00,0x80,0x80,0x00,0x00 };
const uint8_t WlanSigWeak[] PROGMEM = { 0x00,0x00,0xa0,0xa0,0x00,0x00 };
const uint8_t WlanSigGood[] PROGMEM = { 0x10,0x08,0xa8,0xa8,0x08,0x10 };
const uint8_t WlanSigStrong[] PROGMEM = { 0x14,0x0a,0xaa,0xaa,0x0a,0x14 };


/*const uint8_t TankUp[] PROGMEM = { 0x7c,0x54,0xfe,0x02,0xe2,0x2f,0x28,0xa8,0xaf,0xe2,0x02,0xfe,0x54,0x7c,0x1f,0x35,
0x3f,0x20,0x33,0x2a,0x2a,0x2a,0x2a,0x33,0x20,0x3f,0x35,0x1f };
const uint8_t TankDown[] PROGMEM = { 0xbe,0xab,0xff,0x01,0xf3,0x15,0x15,0x55,0x55,0xf3,0x01,0xff,0xab,0xbe,0x0f,0x0a,
0x1f,0x10,0x11,0x3d,0x05,0x05,0x3d,0x11,0x10,0x1f,0x0a,0x0f };
const uint8_t TankLeft[] PROGMEM = { 0x20,0x3c,0x27,0xe5,0x07,0xf5,0x17,0x74,0x17,0xf5,0x07,0xe5,0x17,0xfe,0x01,0x0f,
0x39,0x29,0x38,0x2b,0x3a,0x0a,0x3a,0x2b,0x38,0x29,0x3a,0x1f};*/

#endif // SPRITES_H_INCLUDED
