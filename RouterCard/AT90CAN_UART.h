/*
 * AT90CAN_UART.h
 *
 * Created: 4/27/2016 9:59:02 PM
 *  Author: reed
 */


#ifndef AT90CAN_UART_H_
#define AT90CAN_UART_H_
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct circular_buffer
{
	void *head;       // pointer to head
	void *tail;       // pointer to tail
	void *buffer;     // data buffer
	void *buffer_end; // end of data buffer
	int capacity;  // maximum number of items in the buffer
	int count;     // number of items in the buffer
	int sz;        // size of each item in the buffer

} circular_buffer;
void printUART1();
void cb_init(circular_buffer *cb, char *buf, int capacity, int sz);
void cb_free(circular_buffer *cb);
void cb_push_back(circular_buffer *cb, void *item);
void cb_pop_front(circular_buffer *cb, void *item);
int cb_size(circular_buffer *cb);

void USART0_Init( unsigned long baud);
void USART0_put_C (unsigned char data);
unsigned char USART0_get_C();
int USART0_Available();
bool isUART0_ReceiveEmpty();
unsigned char USART0_peek_C();
void USART0_Flush();

void USART1_Init( unsigned long baud);
void USART1_put_C (unsigned char data);
unsigned char USART1_get_C();
int USART1_Available();
bool isUART1_ReceiveEmpty();
unsigned char USART1_peek_C();
void USART1_Flush();




//extern static FILE *mystdout;

#endif /* AT90CAN_UART_H_ */