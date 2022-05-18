//#include "../../AT90CAN_UART.h"
#include "ft.h"
#include "crc.h"
#include "parser.h"
//#include <util/delay.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
//#include <avr/io.h>

/*
 * Functions for interacting with FastTransfer instances.
 */

void FT_Init (FT_t * handle,
							uint8_t address,
							void(*put)(uint8_t), uint8_t(*get)(), bool(*empty)())
{
	// initialize local data:
	handle->address = address;

	int i;
	for (i = 0; i < ARRAY_SZ; ++i) {
		handle->array[i] = 0;
		handle->flags[i] = false;
	}

	// initialize callback function members:
	handle->put = put;
	handle->get = get;
	handle->empty = empty;

	// initialize receive/transmit buffers:
	Buffer_init(&handle->receive_buf);
	Buffer_init(&handle->transmit_buf);
}

int16_t FT_Read (FT_t * handle, uint8_t index)
{
	handle->flags[index] = false;
	return (int16_t)(handle->array[index]);
}

bool FT_Modified (FT_t * handle, uint8_t index)
{
	bool temp = handle->flags[index];
	handle->flags[index] = false;
	return temp;
}
unsigned char val;
unsigned char tmpVal;
bool FT_Receive (FT_t * handle)
{
	//printUART1 ();
	//PORTA ^= (1 << 5);
	// transfer bytes from serial buffer into local buffer
	while (!handle->empty() && !Buffer_full(&handle->receive_buf)) {
		val = handle->get();
		handle->receive_buf.data[(unsigned int)(handle->receive_buf.tail)] = (unsigned char)val;
		handle->receive_buf.tail = handle->receive_buf.tail + 1;
		if (handle->receive_buf.tail == MAX_PCKT_SZ)
			handle->receive_buf.tail = 0;
		if(handle->receive_buf.size <= MAX_PCKT_SZ)
			handle->receive_buf.size = handle->receive_buf.size + 1;
	}
	// parse/extract messages from local buffer and update local array
	parse(&handle->receive_buf, handle->array, handle->flags, ARRAY_SZ, handle->address);
    return true;
}


void FT_ToSend (FT_t * handle, uint8_t index, int16_t data)
{
	uint16_t combined = (uint16_t)data;
	uint8_t msb = (combined >> 8);
	uint8_t lsb = combined & 0x00FF;
	Buffer_push(&handle->transmit_buf, index);
	Buffer_push(&handle->transmit_buf, lsb);
	Buffer_push(&handle->transmit_buf, msb);

}

void FT_Send (FT_t * handle, uint8_t address)
{
	size_t payload_sz = Buffer_size(&handle->transmit_buf);

	// abandon send if there is no payload
	if (payload_sz == 0)
		return;

	// populate header
	handle->put(0x06);
	handle->put(0x85);
	handle->put(address);
	handle->put(handle->address);
	handle->put(payload_sz);

	// add the payload
	uint8_t payload [MAX_PCKT_SZ];
	unsigned int i;
	for (i = 0; i < payload_sz; ++i)
	{
		uint8_t next_byte = Buffer_pop(&handle->transmit_buf);
		payload[i] = next_byte;
		handle->put(next_byte);
	}

	// add the CRC code
	handle->put(crc(payload, payload_sz));

}

#ifndef SAFE
void FT_ClearFlag(FT_t* handle, uint8_t index) {
	handle->flags[index] = false;
}
#endif
