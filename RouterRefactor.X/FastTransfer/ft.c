#include "ft.h"
#include "crc.h"
#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

/*
 * Functions for interacting with FastTransfer instances.
 */
#ifndef SAFE
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
	rbuffer_init(&handle->receive_buf);
	rbuffer_init(&handle->transmit_buf);
}
#endif

#ifdef SAFE
void FT_Init (FT_t * handle,
							uint8_t address,
							void(*put)(uint8_t), uint8_t(*get)(), int(*empty)())
{
	// initialize local data:
	handle->address = address;

	int i;
	for (i = 0; i < ARRAY_SZ; ++i) {
		handle->array[i] = 0;
		handle->flags[i] = 0;
	}

	// initialize callback function members:
	handle->put = put;
	handle->get = get;
	handle->empty = empty;

	// initialize receive/transmit buffers:
	init_rbuffer(&handle->receive_buf);
	init_rbuffer(&handle->transmit_buf);
}
#endif

int16_t FT_Read (FT_t * handle, uint8_t index)
{
	#ifndef SAFE
	handle->flags[index] = false;
	#endif
	#ifdef SAFE
	handle->flags[index] = 0;
	#endif
	return (int16_t)(handle->array[index]);
}
#ifndef SAFE
bool FT_Modified (FT_t * handle, uint8_t index)
{
	bool temp = handle->flags[index];
	handle->flags[index] = false;
	return temp;
}
#endif
#ifdef SAFE
int FT_Modified (FT_t * handle, uint8_t index)
{
	int temp = handle->flags[index];
	handle->flags[index] = 0;
	return temp;
}
#endif
unsigned char val;
unsigned char tmpVal;
unsigned int FT_Receive (FT_t * handle)
{
	// transfer bytes from serial buffer into local buffer
	while (!handle->empty() && !rbuffer_full(&handle->receive_buf)) {
		val = handle->get();
		handle->receive_buf.data[(unsigned int)(handle->receive_buf.tail)] = (unsigned char)val;
		handle->receive_buf.tail = handle->receive_buf.tail + 1;
		if (handle->receive_buf.tail == MAX_PCKT_SZ)
			handle->receive_buf.tail = 0;
		if(handle->receive_buf.size <= MAX_PCKT_SZ)
			handle->receive_buf.size = handle->receive_buf.size + 1;
	}
	// parse/extract messages from local buffer and update local array
	return parse(&handle->receive_buf, handle->array, handle->flags, ARRAY_SZ, handle->address);
}

void FT_ToSend (FT_t * handle, uint8_t index, int16_t data)
{
	uint16_t combined = (uint16_t)data;
	uint8_t msb = (combined >> 8);
	uint8_t lsb = combined & 0x00FF;
	rbuffer_push(&handle->transmit_buf, index);
	rbuffer_push(&handle->transmit_buf, lsb);
	rbuffer_push(&handle->transmit_buf, msb);

}

void FT_Send (FT_t * handle, uint8_t address)
{
	size_t payload_sz = (size_t)rbuffer_size(&handle->transmit_buf);

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
		uint8_t next_byte = rbuffer_pop(&handle->transmit_buf);
		payload[i] = next_byte;
		handle->put(next_byte);
	}

	// add the CRC code
	handle->put(crc(payload, payload_sz));

}
/*
uint16_t* FT_GetArray(void) {
    return FT_t->array;
}
*/
#ifndef SAFE
void FT_ClearFlag(FT_t* handle, uint8_t index) {
	handle->flags[index] = false;
}
#endif

#ifdef SAFE
void FT_ClearFlag(FT_t* handle, uint8_t index) {
	handle->flags[index] = 0;
}
#endif