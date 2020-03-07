/* 
 * File:   uart_config.h
 * Author: Alex
 *
 * Created on February 1, 2020, 1:20 PM
 */

#ifndef UART1_CONFIG_H
#define	UART1_CONFIG_H

#include <stdint.h>
#include <xc.h>
#include <stdbool.h>
#define BAUD_DEFAULT 115200

#ifdef	__cplusplus
extern "C" {
#endif

void uart1_init(void);
void uart1_put(uint8_t val);
void uart1_put_c(uint8_t val);
uint8_t uart1_get();
uint8_t uart1_peek();
bool uart1_rx_empty();
uint8_t* uart1_rx_getarray();

void uart1_disable(void);
void uart1_enable(void);
bool uart1_isenabled(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART1_CONFIG_H */

