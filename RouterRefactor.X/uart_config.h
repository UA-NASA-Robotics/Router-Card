/* 
 * File:   uart_config.h
 * Author: Alex
 *
 * Created on February 1, 2020, 1:20 PM
 */

#ifndef UART_CONFIG_H
#define	UART_CONFIG_H

#include "config.h"

#ifdef	__cplusplus
extern "C" {
#endif

void UART_INIT(int con, int sta, int baud);
void uart_init(int baud);


#ifdef	__cplusplus
}
#endif

#endif	/* UART_CONFIG_H */

