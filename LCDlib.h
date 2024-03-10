/********************** LCDlib.h ******************************/

#define LCD_DATA (BIT_0|BIT_1|BIT_2|BIT_3|BIT_4|BIT_5|BIT_6|BIT_7) // Port E
#define LCD_EN    BIT_5     // Port D
#define LCD_RS    BIT_15    // Port B
#define LCD_RW    BIT_4     // Port D

#define COUNTS_PER_MS 8886

void system_init(void);
void sw_msDelay (unsigned int mS);
void LCD_init(void);
char readLCD(int addr);
void LCD_write(int addr, char c);
void LCD_puts(char *char_string);
int busyLCD(void);
void LCD_putc(char c);

/* End of LCDlib.h */