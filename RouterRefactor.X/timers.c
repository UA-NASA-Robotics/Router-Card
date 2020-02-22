#include "timers.h"


volatile unsigned long long ms;

void timer1_init(void) {
    T1CONbits.TCKPS = 0b10; // 1:64 prescale
    PR1 = 938;              //ms delay
    IPC0bits.T1IP = 1;
    IEC0bits.T1IE = 1;
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 1;
}

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) {
    ms++;
    if (ms > 65535)
        ms=0;
    IFS0bits.T1IF = 0;
}

unsigned long long get_ms(void) {
    return ms;
}

unsigned long long getTimeElapsed(timer_t* t) {
    return ms - t->prevTime;
}

void setTimerInterval(timer_t* t, unsigned long long l) {
    t->timerLength = l;
}

void ResetTimer(timer_t* t) {
    unsigned long long currentTime = ms;
    t->prevTime = currentTime;
}

int timerDone(timer_t* t, int reset) {
    unsigned long long currentTime = ms;
    if (currentTime >= t->prevTime + t->timerLength)
    {
        if (reset)
            t->prevTime = currentTime;
        return 1;
    }
    else
        return 0;
}
