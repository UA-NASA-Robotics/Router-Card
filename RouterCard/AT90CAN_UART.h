/*
 * AT90CAN_UART.h
 *
 * Created: 4/27/2016 9:59:02 PM
 *  Author: reed
 */ 


#ifndef AT90CAN_UART_H_
#define AT90CAN_UART_H_

typedef struct circular_buffer
{
	void *buffer;     // data buffer
	void *buffer_end; // end of data buffer
	int capacity;  // maximum number of items in the buffer
	int count;     // number of items in the buffer
	int sz;        // size of each item in the buffer
	void *head;       // pointer to head
	void *tail;       // pointer to tail
} circular_buffer;

void cb_init(circular_buffer *cb, int capacity, int sz);
void cb_free(circular_buffer *cb);
void cb_push_back(circular_buffer *cb, void *item);
void cb_pop_front(circular_buffer *cb, void *item);
int cb_size(circular_buffer *cb);

void USART0_Init( unsigned long baud);
void USART0_put_C (unsigned char data);
unsigned char USART0_get_C();
int USART0_Available();
unsigned char USART0_peek_C(); 
void USART0_Flush(); 

void USART1_Init( unsigned long baud);
void USART1_put_C (unsigned char data);
unsigned char USART1_get_C();
int USART1_Available();
unsigned char USART1_peek_C(); 
void USART1_Flush(); 



#endif /* AT90CAN_UART_H_ */