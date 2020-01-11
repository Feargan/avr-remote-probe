#include "lcd.h"

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "font.h"
#include <avr/eeprom.h>

uint8_t DisplayBuffer[DRAM_WIDTH][DRAM_HEIGHT];

static void LCD_WriteByte(uint8_t Byte)
{
    int8_t i;
    for(i = 7; i>=0; i--)
    {
        if((Byte >> i)&1)
            LCD_PORT |= (1 << LCD_MOSI);
        else
            LCD_PORT &= ~(1 << LCD_MOSI);
        LCD_PORT |= 1 << LCD_CLOCK;
        LCD_PORT &= ~(1 << LCD_CLOCK);
    }
}

void LCD_Clear()
{
    uint8_t i,j;
    for(i = 0; i<DRAM_HEIGHT; i++)
        for(j = 0; j<DRAM_WIDTH; j++)
            DisplayBuffer[j][i] = 0x0;
}

void LCD_Init()
{
    LCD_DDR |= (1 << LCD_SCE)|(1 << LCD_DC)|(1 << LCD_RESET)|(1 << LCD_MOSI)|(1 << LCD_CLOCK);
    //LCD_PORT &= ~(1 << LCD_SCE);
    LCD_PORT &= ~((1 << LCD_SCE)|(1 << LCD_DC)|(1 << LCD_RESET)|(1 << LCD_MOSI)|(1 << LCD_CLOCK));
    LCD_PORT |= 1 << LCD_RESET;
    _delay_ms(100);

    LCD_SendCommand(0x20 | 0x1); // function set + ext instructions
    LCD_SendCommand(0x14); // bias
    LCD_SendCommand(0xbf); // vop
    LCD_SendCommand(0x20); // fs
    LCD_SendCommand(0xc); // display normal
    LCD_Clear();
}

void LCD_SendCommand(uint8_t Command)
{
    LCD_PORT &= ~(1 << LCD_DC);
    LCD_PORT &= ~(1 << LCD_SCE);
    LCD_WriteByte(Command);
    LCD_PORT |= (1 << LCD_SCE);
}

void LCD_SendData(uint8_t Data)
{
    LCD_PORT |= (1 << LCD_DC);
    LCD_PORT &= ~(1 << LCD_SCE);
    LCD_WriteByte(Data);
    LCD_PORT |= (1 << LCD_SCE);
}

void LCD_DrawText(int PosX, int PosY, const char* Text) // to cut out or make LCD_DrawLetter() or sth
{
    for(; *Text!=0x0; Text++)
    {
        uint8_t Chr = *Text-' ';
        if(Chr <= '~'-' ')
        {
            uint8_t i;
            for(i = 0; i<6; i++)
            {
                if(PosX >= DRAM_WIDTH)
                {
                    PosX = 0;
                    PosY++;
                }
                if(PosY >= DRAM_HEIGHT)
                    return;
                DisplayBuffer[PosX][PosY] = pgm_read_byte(&Font[Chr*6+i]);
                PosX++;
            }
        }
    }
}

void LCD_DrawText_P(int PosX, int PosY, const char* Text)
{
    uint8_t i;
    for(; pgm_read_byte(Text)!=0x0; Text++)
    {
        uint8_t Chr = pgm_read_byte(Text);
		if(Chr == '\r')
			return;
        for(i = 0; i<6; i++)
        {
            if(PosX >= DRAM_WIDTH)
            {
                PosX = 0;
                PosY++;
            }
            if(PosY >= DRAM_HEIGHT)
                return;

            if(Chr >= 0x01 && Chr <= 0x05)
                DisplayBuffer[PosX][PosY] = pgm_read_byte(&FontEx[Chr*6+i]);
            else if(Chr <= '~')
            {
                DisplayBuffer[PosX][PosY] = pgm_read_byte(&Font[(Chr-' ')*6+i]);
            }
            PosX++;
        }
    }
}

void LCD_SetPixel(int8_t PosX, int8_t PosY, uint8_t Pixel)
{
    if(Pixel&1)
        DisplayBuffer[PosX][PosY/8] |= (1 << PosY%8);
    else
        DisplayBuffer[PosX][PosY/8] &= ~(1 << PosY%8);
}

void LCD_SetDramByte(int8_t DramX, int8_t DramY, uint8_t Byte)
{
	DisplayBuffer[DramX][DramY] = Byte;
}

void LCD_DrawSprite_P(int8_t PosX, int8_t PosY, const uint8_t* Buffer, int8_t Width, int8_t Height)
{
    uint8_t i,j,k;
    for(i = 0; i<Height/8; i++)
    {
        for(j = 0; j<Width && PosX+j < DRAM_WIDTH; j++)
        {
            uint8_t Byte = pgm_read_byte(&Buffer[i*Width+j]);
            for(k = 0; k<8 && PosY+k+i*8 < DRAM_HEIGHT*8; k++)
            {
                LCD_SetPixel(PosX+j, PosY+k+i*8, (Byte>>k));
            }
        }
    }
    i = Height/8;
    for(j = 0; j<Width && PosX+j < DRAM_WIDTH; j++)
    {
        uint8_t Byte = pgm_read_byte(&Buffer[i*Width+j]);
        for(k = 0; k<Height%8 && PosY+k+i*8 < DRAM_HEIGHT*8; k++)
        {
                LCD_SetPixel(PosX+j, PosY+k+i*8, (Byte>>k));
        }
    }

}

void LCD_InvertCell(int8_t PosX, int8_t PosY)
{
    uint8_t i;
    for(i = 0; i<6; i++)
        DisplayBuffer[PosX*6+i][PosY] ^= 0xff;
}

void LCD_Render()
{
    LCD_PORT |= (1 << LCD_DC);
    LCD_PORT &= ~(1 << LCD_SCE);
    uint8_t i,j;
    for(i = 0; i<DRAM_HEIGHT; i++)
        for(j = 0; j<DRAM_WIDTH; j++)
            LCD_WriteByte(DisplayBuffer[j][i]);
    LCD_PORT |= (1 << LCD_SCE);
}

void LCD_SetBrightness(uint8_t Brightness)
{
	if(Brightness > MAX_BRIGHTNESS)
		Brightness = MAX_BRIGHTNESS;
	if(Brightness < MIN_BRIGHTNESS)
		Brightness = MIN_BRIGHTNESS;
	LCD_SendCommand(0x20 | 0x1); // function set + ext instructions
	LCD_SendCommand(0x80|(0x7f&Brightness)); // vop
	LCD_SendCommand(0x20); // fs
}

void LCD_SetupBacklight()
{
	LCD_BACKLIGHT_DDR |= (1 << LCD_BACKLIGHT);
	
	// wrap into macros!! ---v
	TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); //LCD_BACKLIGHT_TCCRA
	TCCR2B |= (1 << WGM22) | (1 << WGM13) | (1 << CS22) | (1 << CS20); //LCD_BACKLIGHT_TCCRB
	OCR2A = 100; // LCD_BACKLIGHT_OCR_CMP
	//
	
	LCD_BACKLIGHT_OCR = MAX_BACKLIGHT_BRIGHTNESS; // eeprom_read_byte(ConfigBacklight);
}

void LCD_SetBacklightBrightness(uint8_t Brightness)
{
	if(Brightness > MAX_BACKLIGHT_BRIGHTNESS)
		Brightness = MAX_BACKLIGHT_BRIGHTNESS;
	if(Brightness < MIN_BACKLIGHT_BRIGHTNESS)
		Brightness = MIN_BACKLIGHT_BRIGHTNESS;
	LCD_BACKLIGHT_OCR = 100-Brightness;
}