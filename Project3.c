/************* Project 3 *************/

#include <plib.h>
#include "CerebotMX7cK.h"
#include "Project3.h"

int main(void)
{
    int mS = 100;
    unsigned int buttons, dir, mode, sm_code, step_delay;
    
    system_init();              /* Setup system Hardware. */
    PORTSetPinsDigitalIn(IOPORT_G, BTN1 | BTN2);
    
    while(1)
    {
        buttons = read_buttons();
        decode_buttons(buttons, &step_delay, &dir, &mode);
        sm_code = sw_fsm(dir, mode);
        output_sm_code(sm_code);
        LATBINV = LEDA;
        sw_msDelay (step_delay);        /* Software only delay */
    }
    
    return 0;           /* Returning a value is expected but this statement
                        * should never execute */
}

void system_init(void)
{
    Cerebot_mx7cK_setup();
    PORTSetPinsDigitalOut(IOPORT_B, SM_LEDS);
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
            *step_delay = 40;
            break;
        case BTN1:
            *dir = CW;
            *mode = HS;
            *step_delay = 20;
            break;
        case BTN2:
            *dir = CCW;
            *mode = HS;
            *step_delay = 20;
            break;
        case BTN1 | BTN2:
            *dir = CCW;
            *mode = FS;
            *step_delay = 40;
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

void sw_msDelay (unsigned int mS)
{
int i;
    while(mS --)
    {
        for (i = 0; i< COUNTS_PER_MS; i++)      // 1 ms delay loop
        {
            // do nothing
        }
        LATBINV = LEDB;         // Toggle LEDA each ms for instrumentation
    }
}

/* End of Project3.c */