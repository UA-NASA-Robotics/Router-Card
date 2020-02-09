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

timer1_init(void);
unsigned long get_ms(void);

#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

