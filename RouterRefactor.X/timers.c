#include "timers.h"

void timer1_init(void) {
    T1CONbits.TCKPS = 0b10; // 1:64 prescale
    PR1 = 938;              //ms delay
    IPC0bits.T1IP = 1;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 1;
}

extern volatile unsigned long ms;

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    ms++;
    if (ms > 65535)
        ms=0;
    IFS0bits.T1IF = 0;
}

unsigned long get_ms(void) {
    return ms;
}