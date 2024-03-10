/************* Project 5 *************/

#include <plib.h>
#include "CerebotMX7cK.h"
#include "Project5.h"

unsigned int buttons, dir, mode;
unsigned int step_delay = 25;

int main(void)
{
    system_init();              /* Setup system Hardware. */
    unsigned int sm_code;
    while(1)
    {
 
    }
    return 1;           /* Returning a value is expected but this statement
                        * should never execute */
}

void system_init(void)
{
    Cerebot_mx7cK_setup();
    PORTSetPinsDigitalIn(IOPORT_G, BTN1 | BTN2);
    PORTSetPinsDigitalOut(IOPORT_B, SM_LEDS);
    OpenTimer1(T1_ON | T1_PS_1_1, (T1_TICK-1));
    LATBCLR = SM_LEDS;
    LATBCLR = SM_COILS;
    
    Timer1_interrupt_init();
    CN_interrupt_init();

    INTEnableSystemMultiVectoredInt();
    INTEnableInterrupts();
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

/* End of Project5.c */