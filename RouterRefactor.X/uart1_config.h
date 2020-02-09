/* 
 * File:   uart_config.h
 * Author: Alex
 *
 * Created on February 1, 2020, 1:20 PM
 */

#ifndef UART_CONFIG_H
#define	UART_CONFIG_H

#include <stdint.h>
#include "defs.h"
#include "ft.h"
#include "ring_buffer.h"
#include <xc.h>

#define BAUD_DEFAULT 115200

#ifdef	__cplusplus
extern "C" {
#endif

//void uart1_init(int baud);
void uart1_init(void);
void uart1_put(uint8_t val);
uint8_t uart1_get();
uint8_t uart1_peek();

#ifdef	__cplusplus
}
#endif

#endif	/* UART_CONFIG_H */

