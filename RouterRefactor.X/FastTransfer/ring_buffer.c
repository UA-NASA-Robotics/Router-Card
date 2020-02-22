#include "ring_buffer.h"
#include <string.h>

void rbuffer_init(struct ring_buffer_t* buf) {
    buf->size = 0;
    buf->head = 0;
    buf->tail = 0;
}

/*
struct ring_buffer_t rbuffer_t(void) {
    struct ring_buffer_t buf;
    rbuffer_init(&buf);
    return buf;
}
*/
uint64_t rbuffer_size(struct ring_buffer_t* buf) {
    return buf->size;
}

bool rbuffer_empty(struct ring_buffer_t* buf) {
    return buf->size == 0 ? true : false;
}

bool rbuffer_full(struct ring_buffer_t* buf) {
    return buf->size == BUFFER_SIZE ? true : false;
}

uint64_t rbuffer_increment(const uint64_t val, const uint64_t bufSize) {
    return val + 1 >= bufSize ? 0 : val + 1;
}

uint8_t rbuffer_pop(struct ring_buffer_t* buf) {
    uint8_t val;
    if (buf->size > 0)
    {
        val = buf->data[buf->tail];
        buf->tail = rbuffer_increment(buf->tail, BUFFER_SIZE);
        --buf->size;
    }
    else
        val = 0;
    return val;   
}

uint8_t rbuffer_peek(struct ring_buffer_t* buf) {
    return buf->size > 0 ? buf->data[buf->tail] : 0;
}

uint8_t rbuffer_get (struct ring_buffer_t* buf, uint32_t index)
{
	index += buf->head;
	index = index < BUFFER_SIZE ? index : index - BUFFER_SIZE;
	return buf->data[index];
}

void rbuffer_push(struct ring_buffer_t* buf, uint8_t val) {
    if (buf->size < BUFFER_SIZE)
    {
        buf->data[buf->head] = val;
        buf->head = rbuffer_increment(buf->head, BUFFER_SIZE);
        ++buf->size;
    }
}

// for FT, val1 is the index, val2 is the data to be pushed
void rbuffer_push2(struct ring_buffer_t* buf, uint8_t val1, uint8_t val2) {
    rbuffer_push(buf, val1);
    rbuffer_push(buf, val2);
}

void rbuffer_push3(struct ring_buffer_t* buf, uint8_t val1, uint8_t val2, uint8_t val3) {
    rbuffer_push(buf, val1);
    rbuffer_push(buf, val2);
    rbuffer_push(buf, val3);
}

void rbuffer_clear(struct ring_buffer_t* buf) {
    memset(buf->data, 0, buf->size*sizeof(uint8_t));
    buf->head = 0;
    buf->tail = 0;
    buf->size = 0;
}

void rbuffer_flush(struct ring_buffer_t* buf, uint8_t val) {
    memset(buf->data, val, buf->size*sizeof(uint8_t));
    buf->head = 0;
    buf->tail = 0;
    buf->size = 0;
}

void rbuffer_reset(struct ring_buffer_t* buf) {
    buf->head = 0;
    buf->tail = 0;
    buf->size = 0;
}

uint8_t* rbuffer_getarray(struct ring_buffer_t* buf) {
    return buf->data;
}