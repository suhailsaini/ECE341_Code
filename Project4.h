/********************** Project 4 ******************************/

#define COUNTS_PER_MS 8886  /* Exact value is to be determined */

/* Function prototypes */
void system_init(void);             /* Hardware Installation */
int read_buttons(void);             /* Read Buttons function */
int decode_buttons(unsigned int buttons, unsigned int *step_delay, unsigned int *dir, unsigned int *mode);
/* Decode Buttons function */
unsigned int sw_fsm(unsigned int dir, unsigned int mode);
void output_sm_code(unsigned int sm_code);
void Timer1_delay(int delay);       /* 1 ms timer function */

#define CW 1
#define CCW 2
#define FS 3
#define HS 4

/* End of Project4.h */