/************* Project 7 *************/

#include <stdio.h>
#include <string.h>
#include <plib.h>
#include "CerebotMX7cK.h"
#include "comm.h"
#include "Project7.h"

unsigned int buttons, dir, mode;
unsigned int step_delay = 25;

main(void)
{
    char str[16];
    int rpm;
    char mode_s[5], dir_s[4];
    
    system_init();
    
    putsU1("CW FS 5");
    
    while(1)
    {
        while(!getstrU1(str, sizeof(str)));
        sscanf(str,"%s %s %d", dir_s, mode_s, &rpm);

        mCNIntEnable(0);            // Disable CN Interrupts
        LCD_write(0,1);             // Clear LCD
        
        putsU1("\r\n");             // echo to LCD
        putsU1(str);
        LCD_puts(str);
        
        if ((strcmp(dir_s, "CW")==0))
        {
            dir = CW;
        } else if ((strcmp(dir_s, "CCW")==0))
        {
            dir = CCW;
        }
        
        if ((strcmp(mode_s, "FS")==0))
        {
            mode = FS;
            step_delay = 60000/(rpm*100*1);
        } else if ((strcmp(mode_s, "HS")==0))
        {
            mode = HS;
            step_delay = 60000/(rpm*100*2);
        }
        
        mCNIntEnable(1);
    }
    return(1);    
}

void system_init(void)
{
    Cerebot_mx7cK_setup();
    
    int cfg1 = PMP_ON|PMP_READ_WRITE_EN|PMP_READ_POL_HI|PMP_WRITE_POL_HI;
    int cfg2 = PMP_DATA_BUS_8 | PMP_MODE_MASTER1 | PMP_WAIT_BEG_1
               | PMP_WAIT_MID_2 | PMP_WAIT_END_1;
    int cfg3 = PMP_PEN_0;           // only PMA0 enabled
    int cfg4 = PMP_INT_OFF;         // no interrupts used
    
    mPMPOpen(cfg1, cfg2, cfg3, cfg4);
    LCD_init();
    
    PORTSetPinsDigitalIn(IOPORT_G, BTN1 | BTN2);
    PORTSetPinsDigitalOut(IOPORT_B, SM_LEDS);
    OpenTimer1(T1_ON | T1_PS_1_1, (T1_TICK-1));
    LATBCLR = SM_LEDS;
    LATBCLR = SM_COILS;
    
    Timer1_interrupt_init();
    CN_interrupt_init();

    INTEnableSystemMultiVectoredInt();
    INTEnableInterrupts();
    
    initialize_uart1(19200, ODD_PARITY); 
}

void sw_msDelay (unsigned int mS)
{
int i;
    while(mS --)
    {
        for (i = 0; i< COUNTS_PER_MS; i++)
        {
            // do nothing
        }
        //LATBINV = LEDB;
    }
}

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

void Timer1_interrupt_init(void)
{
    OpenTimer1(T1_ON | T1_SOURCE_INT | T1_PS_1_1, T1_INTR_RATE-1);
    mT1SetIntPriority(2);
    mT1SetIntSubPriority(0);
    mT1IntEnable(1);
}

void __ISR(_TIMER_1_VECTOR, IPL2) Timer1Handler(void)
{
    LATBINV = LEDA;
    
    if(step_delay > 0)
    {
        step_delay--;
    }
    else if(step_delay == 0)
    {
        unsigned int sm_code = sw_fsm(dir, mode);
        output_sm_code(sm_code);
        step_delay = 25;
    }
    mT1ClearIntFlag();
}

void CN_interrupt_init(void)
{
    unsigned int dummy;
   
    mCNOpen(CN_ON,(CN8_ENABLE | CN9_ENABLE), 0);
    
    mCNSetIntPriority(1);
    mCNSetIntSubPriority(0);
    
    dummy = PORTReadBits(IOPORT_G, BTN1 | BTN2);
    mCNClearIntFlag();
    mCNIntEnable(1);
}

int read_buttons(void)
{
    return PORTReadBits(IOPORT_G, BTN1 | BTN2);
}

void __ISR(_CHANGE_NOTICE_VECTOR, IPL1) CNIntHandler(void)
{
    LATBINV = LEDC;
    
    Timer1_delay(20);
    buttons = read_buttons();
    decode_buttons(buttons, &step_delay, &dir, &mode);
    LATBCLR = LEDC;
    mCNClearIntFlag();
}

int decode_buttons(unsigned int buttons, unsigned int *step_delay, unsigned int *dir, unsigned int *mode)
{
    switch(buttons)
    {
        case 0:
            *dir = CW;
            *mode = FS;
            *step_delay = 50;
            break;
        case BTN1:
            *dir = CW;
            *mode = HS;
            *step_delay = 25;
            break;
        case BTN2:
            *dir = CCW;
            *mode = HS;
            *step_delay = 33;
            break;
        case BTN1 | BTN2:
            *dir = CCW;
            *mode = FS;
            *step_delay = 42;
            break;
    }
    return 0;
}

unsigned int sw_fsm(unsigned int dir, unsigned int mode)
{
    enum {S0_5=0, S1, S1_5, S2, S2_5, S3, S3_5, S4};
    static unsigned int pstate;
    const unsigned int sm_code[] = {0x02, 0x06, 0x04, 0x05, 0x01, 0x09, 0x08, 0xA};
    
    switch (pstate) // Next State Logic
    {
        case S0_5:
            if (dir==CW)
            {
                if (mode == HS) pstate = S1;
                else // FS
                    pstate = S1_5;
            }
            else // CCW
            {
                if (mode == HS) pstate = S4;
                else // FS
                    pstate = S3_5;
            }
            break;
            
            case S1:
            if (dir==CW)
            {
                if (mode == HS) pstate = S1_5;
                else // FS
                    pstate = S2;
            }
            else // CCW
            {
                if (mode == HS) pstate = S0_5;
                else // FS
                    pstate = S4;
            }
            break;
            
        case S1_5:
            if (dir==CW)
            {
                if (mode == HS) pstate = S2;
                else // FS
                    pstate = S2_5;
            }
            else // CCW
            {
                if (mode == HS) pstate = S1;
                else // FS
                    pstate = S0_5;
            }
            break;
            
            case S2:
            if (dir==CW)
            {
                if (mode == HS) pstate = S2_5;
                else // FS
                    pstate = S3;
            }
            else // CCW
            {
                if (mode == HS) pstate = S1_5;
                else // FS
                    pstate = S1;
            }
            break;
            
        case S2_5:
            if (dir==CW)
            {
                if (mode == HS) pstate = S3;
                else // FS
                    pstate = S3_5;
            }
            else // CCW
            {
                if (mode == HS) pstate = S2;
                else // FS
                    pstate = S1_5;
            }
            break;
            
            case S3:
            if (dir==CW)
            {
                if (mode == HS) pstate = S3_5;
                else // FS
                    pstate = S4;
            }
            else // CCW
            {
                if (mode == HS) pstate = S2_5;
                else // FS
                    pstate = S2;
            }
            break;
            
        case S3_5:
            if (dir==CW)
            {
                if (mode == HS) pstate = S4;
                else // FS
                    pstate = S0_5;
            }
            else // CCW
            {
                if (mode == HS) pstate = S3;
                else // FS
                    pstate = S2_5;
            }
            break;
            
            case S4:
            if (dir==CW)
            {
                if (mode == HS) pstate = S0_5;
                else // FS
                    pstate = S1;
            }
            else // CCW
            {
                if (mode == HS) pstate = S3_5;
                else // FS
                    pstate = S3;
            }
            break;
    }
    return sm_code[pstate];
}

void output_sm_code(unsigned int sm_code)
{
    int temp, new_data, mask;
    LATBINV = LEDB;
    mask = SM_COILS;
    new_data = (sm_code << 7);
    temp = LATB;
    temp = temp & ~mask;
    temp = temp | new_data;
    LATB = temp;
}

void Timer1_delay(int delay)
{
    while(delay--)
    {
        while(!mT1GetIntFlag());        // Wait for interrupt flag to be set
        mT1ClearIntFlag();              // Clear the interrupt flag
    }
}

/* End of Project7.c */