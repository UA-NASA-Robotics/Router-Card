#include "interrupt.h"


extern volatile unsigned long ms;

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    ms++;
    if (ms > 65535)
        ms=0;
    IFS0bits.T1IF = 0;
}

unsigned long get_MS(void) {
    return ms;
}

