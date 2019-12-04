/*
 * AT90CAN_UART.c
 *
 * Created: 4/27/2016 9:57:15 PM
 *  Author: reed
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "AT90CAN_UART.h"
#include "LEDs.h"
#define F_CPU 16000000UL


#define UART1_TX_Buffer_Size 25
char uart1_TXbuff[UART1_TX_Buffer_Size];
circular_buffer UART1_TX_Buffer;

#define UART1_RX_Buffer_Size 100
char uart1_RXbuff[UART1_RX_Buffer_Size];
circular_buffer UART1_RX_Buffer;

#define UART0_TX_Buffer_Size 50
char uart0_TXbuff[UART0_TX_Buffer_Size];
circular_buffer UART0_TX_Buffer;

#define UART0_RX_Buffer_Size 5
char uart0_RXbuff[UART0_RX_Buffer_Size];
circular_buffer UART0_RX_Buffer;

char received;
int uart_putchar(char c, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void myMemCpy(void *dest, void *src, size_t n)
{
	// Typecast src and dest addresses to (char *)
	char *csrc = (char *)src;
	char *cdest = (char *)dest;

	// Copy contents of src[] to dest[]
	for (int i=0; i<n; i++)
		cdest[i] = csrc[i];
}
int tmp;
void printUART1()
{
	while(!isUART1_ReceiveEmpty())
	{
		tmp = USART1_get_C();
		//tmp = tmp+1;
		//if(tmp != 0xFF)
		//USART0_put_C ((unsigned char*)UART1_RX_Buffer.head);
	}
}
int uart_putchar(char c, FILE *stream)
{
	if (c == '\n') uart_putchar('\r', stream);

	USART0_put_C (c);

	return 0;
}

void cb_init(circular_buffer *cb, char *buf, int capacity, int sz)
{
	cb->buffer = buf;//malloc(capacity * sz);
	if(cb->buffer == 0)
	{
		//PORTA &= ~(1<<6); //turn on error 2
	}
	// handle error
	cb->buffer_end = (int *)(buf + capacity * sz);
	cb->capacity = capacity;
	cb->count = 0;
	cb->sz = sz;
	cb->head = buf;
	cb->tail = buf;
}

// void cb_free(circular_buffer *cb)
// {
//  //free(cb->buffer);
//  // clear out other fields too, just to be safe
// }

void cb_push_back(circular_buffer *cb, void *item)
{
	char *val = (char*)item;
	// handle error
	memcpy(cb->head, val, cb->sz);
	cb->head = (int*)(cb->head + cb->sz);
	if(cb->head >= cb->buffer_end)
		cb->head = cb->buffer;
	if(cb->count < cb->capacity)
	{
		cb->count++;
	}

}

void cb_pop_front(circular_buffer *cb, void *item)
{
	*(unsigned char *)item = 0;
	if(cb->count == 0)
	{
		return;
	}
	// handle error
	memcpy(item, cb->tail, cb->sz);
	cb->tail = (int*)(cb->tail + cb->sz);
	if(cb->tail == cb->buffer_end)
		cb->tail = cb->buffer;
	cb->count--;
	//return *item;
}

void cb_peek_front(circular_buffer *cb, void *item)
{
	//int *item = NULL;
	if(cb->count == 0)

		memcpy(item, cb->tail, cb->sz);
	//return *item;
}

int cb_size(circular_buffer *cb) {
	return cb->count;
}

void USART0_Init( unsigned long baud) {
	cb_init(&UART0_TX_Buffer,uart0_TXbuff, UART0_TX_Buffer_Size, 1);
	cb_init(&UART0_RX_Buffer,uart0_RXbuff, UART0_RX_Buffer_Size, 1);
	switch (baud)
	{
	case 115200:
		UBRR0 = 16;
		break;
	case 57600:
		UBRR0 = 34;
		break;
	case 38400:
		UBRR0 = 51;
		break;
	case 19200:
		UBRR0 = 103;
		break;
	default:
		UBRR0 = 207; //9600
	}
	UCSR0A = (1<<U2X0);
	UCSR0C  = (0<<UMSEL0) | (0<<UPM0) | (1<<USBS0) | (1<<UCSZ00) | (1<<UCSZ01);
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);

	stdout = &mystdout;

}

void USART1_Init( unsigned long baud) {
	cb_init(&UART1_RX_Buffer, uart1_RXbuff, UART1_RX_Buffer_Size, 1);
	cb_init(&UART1_TX_Buffer,uart1_TXbuff, UART1_TX_Buffer_Size, 1);

	switch (baud)
	{
	case 115200:
		UBRR1 = 16;
		break;
	case 57600:
		UBRR1 = 34;
		break;
	case 38400:
		UBRR1 = 51;
		break;
	case 19200:
		UBRR1 = 103;
		break;
	default:
		UBRR1 = 207; //9600
	}

	UCSR1A = (1<<U2X1);
	UCSR1C  = (0<<UMSEL1) | (0<<UPM1) | (0<<USBS1) | (1<<UCSZ10) | (1<<UCSZ11);
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);


}
void mon_putc (char ch)
{
	USART0_put_C(ch);
}
void uartPush(circular_buffer * cb, unsigned char data)
{

	*(int*)(cb->head) = data;
	cb->head = (int*)(cb->head + cb->sz);
	if(cb->head >= cb->buffer_end-1)
		cb->head = cb->buffer;
	if(cb->count < cb->capacity)
	{
		cb->count++;
	}

}
void USART0_put_C (unsigned char data)
{
	UCSR0B &=  ~(1<<UDRIE0); // must disable interrupt to ensure adding character isn't interrupted.
	cb_push_back(&UART0_TX_Buffer, &data);
	UCSR0B |= (1<<UDRIE0);

}

void USART1_put_C (unsigned char data)
{
	UCSR1B &=  ~(1<<UDRIE1); // must disable interrupt to ensure adding character isn't interrupted.
	cb_push_back(&UART1_TX_Buffer, &data);
	UCSR1B |= (1<<UDRIE1);
}
unsigned char val;
unsigned char USART1_get_C() {

	//UCSR1B &=  ~(1<<UDRIE1); // must disable interrupt to ensure adding character isn't interrupted.
	cb_pop_front(&UART1_RX_Buffer, &val);
	//UCSR1B |= (1<<UDRIE1);
	return val;
}

unsigned char USART0_get_C() {
	char val;
	cb_pop_front(&UART0_RX_Buffer, &val);
	return val;
}

unsigned char USART0_peek_C() {
	char val;
	cb_peek_front(&UART0_RX_Buffer, &val);
	return val;
}

unsigned char USART1_peek_C() {
	char val;
	cb_peek_front(&UART1_RX_Buffer, &val);
	return val;
}

int USART0_Available()
{
	return cb_size(&UART0_RX_Buffer);
}

int USART1_Available()
{
	return cb_size(&UART1_RX_Buffer);
}
bool isUART0_ReceiveEmpty()
{
	if(UART0_RX_Buffer.count <=0)
		return 1;
	else
		return 0;
}
bool isUART1_ReceiveEmpty()
{
	return USART1_Available() <=0;
}
void USART0_Flush()
{
	char temp;
	while (cb_size(&UART0_RX_Buffer)>0)
	{
		cb_pop_front(&UART0_RX_Buffer, &temp);
	}
}

void USART1_Flush()
{
	char temp;
	while (cb_size(&UART1_RX_Buffer)>0)
	{
		cb_pop_front(&UART1_RX_Buffer, &temp);
	}
}


ISR(USART1_UDRE_vect)
/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
{
	if (cb_size(&UART1_TX_Buffer) > 0)
	{
		char temp;
		cb_pop_front(&UART1_TX_Buffer, &temp);
		UDR1 = temp;
	}
	else
	{
		UCSR1B &=  ~(1<<UDRIE1); //disable sending when buffer empty
	}
}
bool uart0POP(circular_buffer *cb, unsigned char *val)
{

	if(cb->count == 0)
	{
		return false;
	}
	// handle error
	//memcpy(item, cb->tail, cb->sz);
	*val = *(unsigned char *)cb->tail;
	cb->tail = (int*)cb->tail + cb->sz;
	if(cb->tail == cb->buffer_end)
		cb->tail = cb->buffer;
	cb->count--;
	return *val;
}
unsigned char tempTX = 0;
ISR(USART0_UDRE_vect)
/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose:  called when the UART is ready to transmit the next byte
**************************************************************************/
{
	if (UART0_TX_Buffer.count > 0)
	{

		//if(uart0POP(&UART0_TX_Buffer, &tempTX))
		cb_pop_front(&UART0_TX_Buffer, &tempTX);
		UDR0 = tempTX;
	}
	else
	{
		UCSR0B &=  ~(1<<UDRIE0); //disable sending when buffer empty
	}
}

// ISR(USART1_TX_vect) {
// //not used
// }
char received1;
int* HeadPtr;
ISR(USART1_RX_vect) {


	received1 = UDR1; // might be able to eliminate this line.
	toggleLED(LED6);
	cb_push_back(&UART1_RX_Buffer, &received1);

}

char received0;
ISR(USART0_RX_vect) {
	received0 = UDR0; // might be able to eliminate this line.
	cb_push_back(&UART0_RX_Buffer, &received0);
}