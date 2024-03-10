/********************** Project 5 ******************************/

#define COUNTS_PER_MS 8886  /* Exact value is to be determined */
#define T1_PRESCALE         1
#define TOGGLES_PER_SEC     1000
#define T1_TICK             (FPB/T1_PRESCALE/TOGGLES_PER_SEC)
#define T1_INTR_RATE 10000

/* Function prototypes */
void system_init(void);             /* Hardware Installation */
int decode_buttons(unsigned int buttons, unsigned int *step_delay, unsigned int *dir, unsigned int *mode);
unsigned int sw_fsm(unsigned int dir, unsigned int mode);
void output_sm_code(unsigned int sm_code);
void Timer1_delay(int delay);       /* 1 ms timer function */
void Timer1_interrupt_init(void);
void __ISR(_CHANGE_NOTICE_VECTOR, IPL2) Timer1Handler(void);
void CN_interrupt_init(void);
void __ISR(_CHANGE_NOTICE_VECTOR, IPL1) CNIntHandler(void);

#define CW 1
#define CCW 2
#define FS 3
#define HS 4

/* End of Project5.h */