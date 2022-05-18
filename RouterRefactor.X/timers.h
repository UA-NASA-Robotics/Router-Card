/* 
 * File:   Timers.h
 * Author: Alex
 *
 * Created on February 8, 2020, 10:32 PM
 */

#ifndef TIMERS_H
#define	TIMERS_H

#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned long long timerLength;
    unsigned long long prevTime;
} timer_t;

extern volatile unsigned long long ms;
void timer1_init(void);
unsigned long long get_ms(void);
unsigned long long millis(void);
unsigned long long getTimeElapsed(timer_t*);
void setTimerInterval(timer_t*, unsigned long long);
void ResetTimer(timer_t*);
int timerDone(timer_t* t, int resetFlag);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

