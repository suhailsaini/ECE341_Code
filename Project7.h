/********************** Project7.h ******************************/

#define LCD_DATA (BIT_0|BIT_1|BIT_2|BIT_3|BIT_4|BIT_5|BIT_6|BIT_7) // Port E
#define LCD_EN    BIT_5     // Port D
#define LCD_RS    BIT_15    // Port B
#define LCD_RW    BIT_4     // Port D

#define COUNTS_PER_MS 8886
#define T1_PRESCALE         1
#define TOGGLES_PER_SEC     1000
#define T1_TICK             (FPB/T1_PRESCALE/TOGGLES_PER_SEC)
#define T1_INTR_RATE 10000

void system_init(void);
void sw_msDelay (unsigned int mS);
void LCD_init(void);
char readLCD(int addr);
void LCD_write(int addr, char c);
void LCD_puts(char *char_string);
int busyLCD(void);
void LCD_putc(char c);

int decode_buttons(unsigned int buttons, unsigned int *step_delay, unsigned int *dir, unsigned int *mode);
unsigned int sw_fsm(unsigned int dir, unsigned int mode);
void output_sm_code(unsigned int sm_code);
void Timer1_delay(int delay);
void Timer1_interrupt_init(void);
void __ISR(_CHANGE_NOTICE_VECTOR, IPL2) Timer1Handler(void);
void CN_interrupt_init(void);
void __ISR(_CHANGE_NOTICE_VECTOR, IPL1) CNIntHandler(void);

#define CW 1
#define CCW 2
#define FS 3
#define HS 4

/* End of Project7.h */