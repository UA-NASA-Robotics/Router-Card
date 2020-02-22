/* 
 * File:   ring_buffer.h
 * Author: Alex
 *
 * Created on February 8, 2020, 6:03 PM
 */

#ifndef RING_BUFFER_H
#define	RING_BUFFER_H

#define FT_DEFS


#ifdef FT_DEFS
#include "ft_config.h"
#endif

#include <stdint.h>
#include <stdbool.h>

#ifdef FT_DEFS
#define BUFFER_SIZE MAX_PCKT_SZ
#endif
#ifndef FT_DEFS
#define BUFFER_SIZE 200
#endif


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
    
    typedef struct ring_buffer_t rbuffer_t;
    
    void rbuffer_init(struct ring_buffer_t*);
    //struct ring_buffer_t rbuffer_t(void);
    uint64_t rbuffer_size(struct ring_buffer_t*);
    uint64_t rbuffer_increment(const uint64_t, const uint64_t);
    uint8_t rbuffer_pop(struct ring_buffer_t*);
    uint8_t rbuffer_peek(struct ring_buffer_t*);
    uint8_t rbuffer_get(struct ring_buffer_t*, uint32_t);
    void rbuffer_push(struct ring_buffer_t*, uint8_t data);
    void rbuffer_push2(struct ring_buffer_t*, uint8_t, uint8_t);
    void rbuffer_push3(struct ring_buffer_t*, uint8_t, uint8_t, uint8_t);
    void rbuffer_clear(struct ring_buffer_t*);
    void rbuffer_flush(struct ring_buffer_t*, uint8_t val);
    uint8_t* rbuffer_getarray(struct ring_buffer_t*);
    bool rbuffer_empty(struct ring_buffer_t*);
    bool rbuffer_full(struct ring_buffer_t*);
    void rbuffer_reset(struct ring_buffer_t*);

#ifdef	__cplusplus
}
#endif

#endif	/* RING_BUFFER_H */

