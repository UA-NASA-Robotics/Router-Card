/* 
 * File:   ring_buffer.h
 * Author: Alex
 *
 * Created on February 8, 2020, 6:03 PM
 */

#ifndef RING_BUFFER_H
#define	RING_BUFFER_H

#include <stdint.h>

#define BUFFER_SIZE 200UL

#ifdef	__cplusplus
extern "C" {
#endif

    struct ring_buffer_t
    {
        uint8_t data[BUFFER_SIZE];
        uint64_t size;
        uint64_t head;
        uint64_t tail;
    };

    void init_rbuffer(struct ring_buffer_t*);
    struct ring_buffer_t rbuffer_t(void);
    uint64_t rbuffer_size(struct ring_buffer_t*);
    uint64_t rbuffer_increment(const uint64_t, const uint64_t);
    uint8_t rbuffer_pop(struct ring_buffer_t*);
    uint8_t rbuffer_peek(struct ring_buffer_t*);
    void rbuffer_push(struct ring_buffer_t*, uint8_t data);
    void rbuffer_push2(struct ring_buffer_t*, uint8_t, uint8_t);
    void rbuffer_push3(struct ring_buffer_t*, uint8_t, uint8_t, uint8_t);
    void rbuffer_clear(struct ring_buffer_t*);
    uint8_t* rbuffer_getarray(struct ring_buffer_t*);

#ifdef	__cplusplus
}
#endif

#endif	/* RING_BUFFER_H */

