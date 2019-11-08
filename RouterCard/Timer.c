/*
 * Timer.c
 *
 * Created: 5/5/2017 12:39:32 AM
 *  Author: Zac
 */

#include "Timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "LEDs.h"

//#define PRESCALER8    (1<<CS01)       //TOO FAST
//#define PRESCALER64   (1<<CS01)|(1<<CS00)   //TIMER INTERVAL ISR 1ms
//#define PRESCALER256  (1<<CS02)       //TIMER INTERVAL ISR 4ms
#define PRESCALER1024 (1<<CS02)|(1<<CS00)   //TIMER INTERVAL ISR 16ms

#ifdef PRESCALER8
#define PRESCALER PRESCALER8
#elif PRESCALER64
#define PRESCALER PRESCALER64
#elif PRESCALER256
#define PRESCALER PRESCALER256
#elif PRESCALER1024
#define PRESCALER PRESCALER1024
#else
#define PRESCALER PRESCALER256    //Default interval 4ms
#endif

volatile unsigned long long globalTime=0;

void setTimerInterval(timer_t * t, unsigned long long l)
{
	t->timerLength=l;
}

void resetTimer(timer_t * t)
{
	unsigned long long timenow=globalTime;
	t->prevTime=timenow;
}
unsigned int getTimeElepsed(timer_t * t)
{
	return globalTime - t->prevTime;
}
bool timerDone(timer_t * t)
{
	unsigned long long timenow=globalTime;
	if(timenow >= t->prevTime+t->timerLength)
	{
		t->prevTime=timenow;
		return true;
	}
	else
	{
		return false;
	}
}
bool timerDone_NoReset(timer_t * t)
{
	unsigned long long timenow=globalTime;
	if(timenow >= t->prevTime+t->timerLength)
	{

		return true;
	}
	else
	{
		return false;
	}
}
void initTimer0(void)
{
	TCCR0A = PRESCALER;   //Set the prescaler
	TIMSK0 = (1<<TOIE0);    //ENABLE Timer Overflow interrupt
}

ISR(TIMER0_OVF_vect)
{
#ifdef PRESCALER1024
	globalTime+=16;
#elif PRESCALER256
	globalTime+=4;
#elif PRESCALER64
	globalTime++;
#endif

	TIFR0 = (0<<TOV0);  //Reset timer0 overflow interrupt flag
}