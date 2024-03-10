#include <plib.h>
#include "LCDlib.h"

void LCD_init(void)
{
    int mS = 50;
    
    sw_msDelay(mS);
    int addr = 0;
    LCD_write(addr, 0x38);
    sw_msDelay(mS);
    LCD_write(addr, 0x0f);
    sw_msDelay(mS);
    LCD_write(addr, 0x01);
}

void LCD_write(int addr, char c)
{
    while(busyLCD());    // Wait for LCD to be ready
    PMPSetAddress(addr); // Set LCD RS control
    PMPMasterWrite(c);   // initiate write sequence
}

char readLCD(int addr)
{
    PMPSetAddress(addr);            // Set LCD RS control
    mPMPMasterReadByte();           // initiate dummy read sequence
    return mPMPMasterReadByte();    // read actual data
}

void LCD_puts(char *char_string)
{
    while(*char_string)             // Look for end of string NULL character
    {
        LCD_putc(*char_string);     // Write character to LCD
        char_string++;              // Increment string pointer
    }
}

void LCD_putc(char c)
{
    while(busyLCD());
    char addr = readLCD(0);
    switch(c)
    {
        case('\r'):
            if(0x00 <= addr <= 0x0f)
            {
                LCD_write(0, 0x80);
            }
            if(0x40 <= addr <= 0x4f)
            {
                LCD_write(0, 0xC0);
            }
            break;
        case('\n'):
            if(0x00 <= addr <= 0x0f)
            {
                LCD_write(0, 0xC0);
            }
            if(0x40 <= addr <= 0x4f)
            {
                LCD_write(0, 0x80);
            }
            break;
        default:
            if(addr == 0x10)
            {
                LCD_write(c, 0xC0);
            }
            if(addr == 0x50)
            {
                LCD_write(c, 0x02);
            }
            LCD_write(1, c);
            break;            
    }
}

int busyLCD(void)
{
    char busy = readLCD(0);
    int busyflag = (busy & BIT_7);      // busy & 0x08
    return busyflag;
}