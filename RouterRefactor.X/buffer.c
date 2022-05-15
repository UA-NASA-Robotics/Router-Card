

#include <stdint.h>
#include "buffer.h"

void Buffer_init (Buffer_t * buffer)
{
	buffer->size = 0;
	buffer->head = 0;
	buffer->tail = 0;
}

size_t Buffer_size (Buffer_t * buffer)
{
	return buffer->size;
}

bool Buffer_full (Buffer_t * buffer)
{
	return Buffer_size(buffer) == MAX_PCKT_SZ;
}

bool Buffer_empty (Buffer_t * buffer)
{
	return Buffer_size(buffer) == 0;
}

void Buffer_push (Buffer_t * buffer, uint8_t value)
{
	if (Buffer_full(buffer))
		return;
	buffer->data[buffer->tail++] = value;
	++buffer->size;
	if (buffer->tail == MAX_PCKT_SZ)
		buffer->tail = 0;
}

uint8_t Buffer_pop (Buffer_t * buffer)
{
	--buffer->size;
	uint8_t value = buffer->data[buffer->head];
	++buffer->head;
	if (buffer->head == MAX_PCKT_SZ)
		buffer->head = 0;
	return value;
}

uint8_t Buffer_get (Buffer_t * buffer, unsigned int index)
{
	index += buffer->head;
	index = (index < MAX_PCKT_SZ) ? index : index - MAX_PCKT_SZ;
	return buffer->data[index];
}

uint8_t * Buffer_get_array (Buffer_t * buffer)
{
	return buffer->data;
}

void Buffer_clear(Buffer_t *buf)
{
	buf->size = 0;
	buf->head = 0;
	buf->tail = 0;
}

