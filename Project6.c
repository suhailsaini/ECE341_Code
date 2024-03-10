/************* Project 6 *************/

#include <plib.h>
#include "CerebotMX7cK.h"
#include "LCDlib.h"

int main(void)
{
    system_init();
    char string1[] = "Does Dr J prefer PIC32 or FPGA??";
    char string2[] = "Answer: \116\145\151\164\150\145\162\041";
    
    while(1)
    {
        LCD_puts(string1);
        sw_msDelay(5000);
        LCD_write(0, 0x01);
        LCD_puts(string2);
        sw_msDelay(5000);
        LCD_write(0, 0x01);        
    }
    return 1;
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

/* End of Project6.c */