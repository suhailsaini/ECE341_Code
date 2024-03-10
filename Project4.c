/************* Project 4 *************/

#include <plib.h>
#include "CerebotMX7cK.h"
#include "Project4.h"

#define T1_PRESCALE         1
#define TOGGLES_PER_SEC     1000
#define T1_TICK             (FPB/T1_PRESCALE/TOGGLES_PER_SEC) 

int main(void)
{
    system_init();              /* Setup system Hardware. */
    unsigned int buttons, dir, mode, sm_code, step_delay;
    int step_delay_counter = 0;
    int button_delay_counter = 0;
    
    while(1)
    {
        // Button reading task
        if (button_delay_counter == 0)
        {
            buttons = read_buttons();
            decode_buttons(buttons, &step_delay, &dir, &mode);
            LATBINV = LEDB;
            button_delay_counter = 100; // Reset counter
        }
        // Motor control task
        if (step_delay_counter == 0)
        { 
            LATBINV = LEDA;
            sm_code = sw_fsm(dir, mode);
            output_sm_code(sm_code);
            LATBINV = LEDC;
            step_delay_counter = step_delay; // Reset counter
        }
        
        Timer1_delay(1); // 1 millisecond delay
        
        step_delay_counter--;
        button_delay_counter--;
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
}

int read_buttons(void)
{
    return PORTReadBits(IOPORT_G, BTN1 | BTN2);
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

void output_sm_code(const unsigned int sm_code)
{
    int temp, new_data, mask;
    
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

/* End of Project4.c */