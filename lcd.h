#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

#define LCD_DDR DDRB
#define LCD_PORT PORTB
#define LCD_SCE PB5
#define LCD_RESET PB4
#define LCD_DC PB3
#define LCD_MOSI PB2
#define LCD_CLOCK PB1

#include <avr/io.h>

#define LCD_BACKLIGHT_OCR OCR2B
#define LCD_BACKLIGHT_DDR DDRD
#define LCD_BACKLIGHT_PORT PORTD
#define LCD_BACKLIGHT PD3

#define DRAM_WIDTH 84
#define DRAM_HEIGHT 6

#define MAX_BRIGHTNESS 65
#define MIN_BRIGHTNESS 50

#define MAX_BACKLIGHT_BRIGHTNESS 100
#define MIN_BACKLIGHT_BRIGHTNESS 0

#include <inttypes.h>

void LCD_Init();
void LCD_SetupBacklight();
void LCD_SendCommand(uint8_t Command);
void LCD_SendData(uint8_t Data);
void LCD_SetBrightness(uint8_t Brightness);
void LCD_SetBacklightBrightness(uint8_t Brightness);
//void LCD_DrawSprite(int8_t PosX, int8_t PosY, uint8_t** Buffer, int8_t Width, int8_t Height);
void LCD_DrawSprite_P(int8_t PosX, int8_t PosY, const uint8_t* Buffer, int8_t Width, int8_t Height);
void LCD_DrawText(int PosX, int PosY, const char* Text);
void LCD_DrawText_P(int PosX, int PosY, const char* Text);
void LCD_InvertCell(int8_t PosX, int8_t PosY);
void LCD_SetPixel(int8_t PosX, int8_t PosY, uint8_t Pixel);
void LCD_SetDramByte(int8_t DramX, int8_t DramY, uint8_t Byte);
void LCD_Clear();
void LCD_Render();

#endif // LCD_H_INCLUDED
