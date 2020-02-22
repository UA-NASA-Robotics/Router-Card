/* 
 * File:   uart2_config.h
 * Author: Alex
 *
 * Created on February 8, 2020, 11:06 PM
 */

#ifndef UART2_CONFIG_H
#define	UART2_CONFIG_H

#include "defs.h"
#include "FastTransfer/ring_buffer.h"
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif

//void uart2_init(int baud);
void uart2_init(void);
void uart2_put(uint8_t val);
void uart2_put_c(uint8_t val);
uint8_t uart2_get();
uint8_t uart2_peek();
bool uart2_rx_empty();
uint8_t* uart2_rx_getarray();

#ifdef	__cplusplus
}
#endif

#endif	/* UART2_CONFIG_H */

